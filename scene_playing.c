#include <stdio.h>
#include <rand.h>
#include <libetc.h>

#include "scene_factory.h"
#include "gfx.h"
#include "gfx_font.h"
#include "sfx.h"
#include "input.h"
#include "cd.h"
#include "game.h"
#include "ship.h"
#include "bullets.h"
#include "enemies.h"
#include "shields.h"

// hud
static uint8_t hud_lives[64];
static uint8_t hud_score[64];
static uint8_t hud_game_level[64];

static void scene_playing_init(void)
{
    // do nothing
}

static void scene_playing_on_enter(void)
{
    srand(VSync(-1));
    shields_reset();
    ship_reset();
    enemies_reset();
    bullets_reset();
    game_state = GAME_STATE_SHOW_ENEMIES;
}

static void scene_playing_on_exit(void)
{
    if (game_state == GAME_STATE_LEVEL_CLEARED)
    {
        game_next_level();
    }    
}

static void scene_playing_on_transition_finished(void)
{
    cd_play_music_xa(GAME_ASSET_FILENAME_XA_MUSIC_PLAYING);
    next_enemy_bullet_time = scene_frame_count + (rand() % 180);
}

static void scene_playing_fixed_update(void)
{
    switch (game_state)
    {
        case GAME_STATE_ENEMIES_EXCEEDED_MAX_Y:
        {
            if (scene_frame_count >= game_state_time)
            {
                ship_hit();
            }
            break;
        }

        case GAME_STATE_ENEMIES_TO_TOP:
        {
            enemies_y -= 1;
            if (enemies_y <= ENEMIES_START_Y)
            {
                enemies_y = ENEMIES_START_Y;
                game_state = GAME_STATE_READY;
            }
            break;
        }

        case GAME_STATE_SHOW_ENEMIES:
        {
            showing_enemy_id++;
            
            if ((showing_enemy_id >> 2) > last_showing_enemy_id)
            {
                last_showing_enemy_id = showing_enemy_id >> 2;
                size_t enemy_col = last_showing_enemy_id % ENEMY_COLS;
                size_t enemy_row = last_showing_enemy_id / ENEMY_COLS;
                enemies[enemy_row][enemy_col].state = ENEMY_STATE_ALIVE;
            }
            if (last_showing_enemy_id >= ENEMY_NUMS - 1)
            {
                game_state = GAME_STATE_READY;
                game_state_time = scene_frame_count + 120;
                enemies_reset_all_explosions();
            }
            break;
        }

        case GAME_STATE_READY:
        {
            if (scene_frame_count >= game_state_time)
            {
                game_state = GAME_STATE_PLAYING;
            }
            break;
        }

        case GAME_STATE_PLAYING:
        {
            bullets_fixed_update();
            ship_fixed_update();
            enemies_fixed_update();
            break;
        }

        case GAME_STATE_SHIP_EXPLODING:
        {
            if (scene_frame_count >= game_state_time)
            {
                game_lives--;
                if (game_lives <= 0)
                {
                    cd_stop_music_xa();
                    game_state = GAME_STATE_GAME_OVER;
                    game_state_time = scene_frame_count + 420;
                    cd_play_music_xa(GAME_ASSET_FILENAME_XA_MUSIC_GAME_OVER);
                    enemies_reset_all_explosions();
                }
                else
                {
                    ship_exploding = false;
                    game_state_time = scene_frame_count + 120;
                    cd_play_music_xa(GAME_ASSET_FILENAME_XA_MUSIC_PLAYING);
                    enemies_reset_all_explosions();

                    if (enemies_exceeded_max_y_limit())
                    {
                        game_state = GAME_STATE_ENEMIES_TO_TOP;
                    }
                    else 
                    {
                        game_state = GAME_STATE_READY;
                    }
                }
            }
            break;
        }

        case GAME_STATE_LEVEL_CLEARED:
        {
            if (scene_frame_count >= game_state_time)
            {
                scene_switch_to(SCENE_ID_PLAYING);
            }
            break;
        }

        case GAME_STATE_GAME_OVER:
        {
            if (scene_frame_count >= game_state_time)
            {
                game_update_hiscore();
                scene_switch_to(SCENE_ID_TITLE);
            }
            break;
        }
    }
}

static void render_hud()
{
    // draw lives
    for (int i = 0; i < game_lives; i++)
    {
        gfx_draw_tile(17, 8 + 20 * i, 224);
    }

    sprintf(hud_score, "SCORE:%06d", game_score); 
    sprintf(hud_game_level, "LEVEL:%03d", game_level); 

    gfx_font_use(GAME_FONT_ID_SMALL);
    gfx_font_draw_text(hud_score, 8, 8);    
    gfx_font_draw_text(hud_game_level, 176, 8);
}

static void render_game_state()
{
    if (game_state == GAME_STATE_READY)
    {
        gfx_font_use(GAME_FONT_ID_BIG);
        gfx_font_draw_text("READY?!", 86, 112);    
    }
    else if (game_state == GAME_STATE_LEVEL_CLEARED)
    {
        gfx_font_use(GAME_FONT_ID_BIG);
        gfx_font_draw_text("LEVEL CLEARED", 50, 112);    
    }
    else if (game_state == GAME_STATE_GAME_OVER)
    {
        gfx_font_use(GAME_FONT_ID_BIG);
        gfx_font_draw_text("GAME OVER", 74, 112);    
    }
}

static void scene_playing_render(void)
{
    // draw max enemies y 
    if (game_state == GAME_STATE_ENEMIES_EXCEEDED_MAX_Y)
    {
        if ((scene_frame_count % 10) < 5)
        {
            gfx_fill_rect(0, ENEMIES_MAY_Y, 256, 240 - ENEMIES_MAY_Y, 128, 0, 0);
        }
    }
    gfx_fill_rect(4, ENEMIES_MAY_Y, 248, 2, 64, 0, 0);

    gfx_use_tileset(GAME_IMAGE_ID_SPRITES);
    shields_render();
    enemies_render();
    ship_render();
    bullets_render();
    render_hud();
    render_game_state();

    // draw a line between ship and lives
    gfx_fill_rect(4, 220, 248, 2, 63, 86, 255);
}

SCENE* scene_playing_create(void)
{
    SCENE *scene_playing = scene_create();

    scene_playing->init = scene_playing_init;
    scene_playing->on_enter = scene_playing_on_enter;
    scene_playing->on_exit = scene_playing_on_exit;
    scene_playing->on_transition_finished = scene_playing_on_transition_finished;
    scene_playing->fixed_update = scene_playing_fixed_update;
    scene_playing->render = scene_playing_render;

    return scene_playing;
}