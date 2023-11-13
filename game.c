#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <rand.h>
#include <stdio.h>
#include <libapi.h>
#include <libgte.h>

#include "mem.h"
#include "cd.h"
#include "sfx.h"
#include "asset.h"
#include "gfx.h"
#include "gfx_font.h"
#include "input.h"
#include "scene_factory.h"
#include "game.h"

static void init_all_subsystems(void)
{
    _96_remove();
    mem_init();    
    gfx_init();
    InitGeom();
    cd_init();
    sfx_init();
    input_init();
}

static void load_all_assets_from_cd(void)
{
    TIM_IMAGE *image_title = asset_load_image(GAME_ASSET_FILENAME_IMAGE_TITLE);
    TIM_IMAGE *image_sprites = asset_load_image(GAME_ASSET_FILENAME_IMAGE_SPRITES);

    gfx_add_tileset(GAME_IMAGE_ID_TITLE, image_title, 1, 1);
    gfx_add_tileset(GAME_IMAGE_ID_SPRITES, image_sprites, 5, 5);

    FONT *font_big = asset_load_font(GAME_ASSET_FILENAME_FONT_BIN_BIG, GAME_ASSET_FILENAME_FONT_TIM_BIG);
    FONT *font_small = asset_load_font(GAME_ASSET_FILENAME_FONT_BIN_SMALL, GAME_ASSET_FILENAME_FONT_TIM_SMALL);
    gfx_font_add(GAME_FONT_ID_BIG, font_big, GAME_IMAGE_ID_FONT_BIG);
    gfx_font_add(GAME_FONT_ID_SMALL, font_small, GAME_IMAGE_ID_FONT_SMALL);

    uint8_t *sound_start = asset_load(GAME_ASSET_FILENAME_SOUND_START);
    uint8_t *sound_ship_laser = asset_load(GAME_ASSET_FILENAME_SOUND_SHIP_LASER);
    uint8_t *sound_ship_explosion = asset_load(GAME_ASSET_FILENAME_SOUND_SHIP_EXPLOSION);
    uint8_t *sound_enemy_laser = asset_load(GAME_ASSET_FILENAME_SOUND_ENEMY_LASER);
    uint8_t *sound_enemy_explosion = asset_load(GAME_ASSET_FILENAME_SOUND_ENEMY_EXPLOSION);

    sfx_add_sound(GAME_SOUND_ID_START, sound_start);
    sfx_add_sound(GAME_SOUND_ID_SHIP_LASER, sound_ship_laser);
    sfx_add_sound(GAME_SOUND_ID_SHIP_EXPLOSION, sound_ship_explosion);
    sfx_add_sound(GAME_SOUND_ID_ENEMY_LASER, sound_enemy_laser);
    sfx_add_sound(GAME_SOUND_ID_ENEMY_EXPLOSION, sound_enemy_explosion);

    mem_free(sound_start);
    mem_free(sound_ship_laser);
    mem_free(sound_ship_explosion);
    mem_free(sound_enemy_laser);
    mem_free(sound_enemy_explosion);
}

static SVECTOR background_stars[2][10];

static void init_common_background(void)
{
    for (int l = 0; l < 2; l++)
    {
        for (int i = 0; i < 10; i++)
        {
            SVECTOR *star = &background_stars[l][i];
            star->vx = rand() % 1024;
            star->vy = rand() % 240;
        }
    }
}

static void render_common_background(void)
{
    for (int l = 0; l < 2; l++)
    {
        for (int i = 0; i < 10; i++)
        {
            SVECTOR *star = &background_stars[l][i];
            star->vx -= l + 1;
            if (star->vx < -8)
            {
                star->vx = 1032;
            }
            uint8_t c = l == 0 ? 32 : 128;
            gfx_fill_rect(star->vx >> 2, star->vy, 2, 2, c, c, c);
        }
    }
}

static void init_all_scenes(void)
{
    init_common_background();

    scene_is_common_background_visible = false;
    scene_render_common_background = render_common_background;

    SCENE *scene_init = scene_init_create();
    SCENE *scene_title = scene_title_create();
    SCENE *scene_playing = scene_playing_create();

    scene_add(SCENE_ID_INIT, scene_init);
    scene_add(SCENE_ID_TITLE, scene_title);
    scene_add(SCENE_ID_PLAYING, scene_playing);
    
    scene_init_all();
}

void game_entry_point(void)
{
    init_all_subsystems();
    load_all_assets_from_cd();
    init_all_scenes();

    // start game with 'init' scene
    scene_switch_to(SCENE_ID_INIT);

    // TODO start game with playing scene
    //game_reset();
    //scene_switch_to(SCENE_ID_PLAYING);

    // game main loop
    while (true)
    {
        input_fixed_update();
        scene_fixed_update();
        scene_render();
        gfx_swap_buffers();
    }
}

// ---

bool game_check_collision(int16_t px, int16_t py, int16_t rx, int16_t ry, int16_t rw, int16_t rh)
{
    return (px >= rx && px < rx + rw && py > ry && py <= ry + rh);
}

GAME_STATE game_state;
uint32_t game_state_time;

uint32_t game_level;
uint32_t game_lives;
uint32_t game_score;
uint32_t game_hi_score = GAME_INITIAL_HISCORE;

void game_reset()
{
    game_level = 1;
    game_lives = 3;
    game_score = 0;
}

void game_add_score(uint32_t points)
{
    game_score += points;
}

void game_update_hiscore()
{
    if (game_score > game_hi_score)
    {
        game_hi_score = game_score;
    }
}

void game_next_level()
{
    game_level++;
    if (game_level > 999)
    {
        game_level = 999;
    }
}