#include <stdio.h>

#include "scene_factory.h"
#include "gfx.h"
#include "gfx_font.h"
#include "cd.h"
#include "sfx.h"
#include "input.h"
#include "game.h"

static uint32_t blink_time;
static uint32_t blink_v;
static uint32_t blink_lt;
static uint32_t start_state;
static uint32_t start_time;

static uint8_t hud_hi_score[64];

static void scene_title_init(void)
{
    // do nothing
}

static void scene_title_on_enter(void)
{
    cd_play_music_xa(GAME_ASSET_FILENAME_XA_MUSIC_TITLE);
    start_state = 0;
    blink_v = 30;
    blink_lt = 10;
}

static void scene_title_on_exit(void)
{
    // do nothing
}

static void scene_title_on_transition_finished(void)
{
    blink_time = scene_frame_count;
}

static void scene_title_fixed_update(void)
{
    if (!scene_is_transition_finished)
    {
        return;
    }

    switch (start_state)
    {
        case 0:
        {
            if (input_is_action_just_pressed(0, INPUT_ACTION_START))
            {
                start_time = scene_frame_count + 25;
                blink_time = scene_frame_count;
                start_state = 1;
                blink_v = 10;
                blink_lt = 5;
                cd_stop_music_xa();
                sfx_play_sound(GAME_SOUND_ID_START);
            }
            break;
        }

        case 1:
        {
            if (scene_frame_count >= start_time)
            {
                game_reset();
                scene_switch_to(SCENE_ID_PLAYING);
            }
            break;
        }
    }
}

static void scene_title_render(void)
{
    gfx_use_tileset(GAME_IMAGE_ID_TITLE);
    gfx_draw_tile(GAME_TILE_ID_TITLE, 0, 16);

    gfx_font_use(GAME_FONT_ID_SMALL);
    
    sprintf(hud_hi_score, "HI-SCORE:%06d", game_hi_score); 
    gfx_font_draw_text(hud_hi_score, 8, 8);    

    if (scene_is_transition_finished && ((scene_frame_count - blink_time) % blink_v) < blink_lt)
    {
        gfx_font_draw_text("press START button", 56, 160);    
    }

    gfx_font_draw_text("(c)2023 danx2016", 64, 208);    
}

SCENE* scene_title_create(void)
{
    SCENE *scene_title = scene_create();

    scene_title->init = scene_title_init;
    scene_title->on_enter = scene_title_on_enter;
    scene_title->on_exit = scene_title_on_exit;
    scene_title->on_transition_finished = scene_title_on_transition_finished;
    scene_title->fixed_update = scene_title_fixed_update;
    scene_title->render = scene_title_render;

    return scene_title;
}