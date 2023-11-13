#include "gfx.h"
#include "sfx.h"
#include "cd.h"
#include "input.h"
#include "scene_manager.h"
#include "game.h"
#include "ship.h"
#include "bullets.h"

int16_t ship_x;
int16_t ship_y;

bool ship_exploding;
uint32_t ship_exploding_time;

void ship_reset(void)
{
    ship_x = 108;
    ship_y = 200;
    ship_exploding = false;
    ship_exploding_time = 0;
}

void ship_fixed_update(void)
{
    if (input_is_action_pressed(0, INPUT_ACTION_LEFT))
    {
        ship_x -= 2;
    }
    if (input_is_action_pressed(0, INPUT_ACTION_RIGHT))
    {
        ship_x += 2;
    }

    // keep ship inside screen area
    if (ship_x < 0)
    {
        ship_x = 0;
    }
    if (ship_x > 256 - 40)
    {
        ship_x = 256 - 40;
    }  
}

void ship_render(void)
{
    int32_t anim_frame = (scene_frame_count % 20) > 10 ? 5 : 0;
    if (ship_exploding)
    {
        gfx_draw_tile(15 + anim_frame, ship_x, ship_y);
        gfx_draw_tile(16 + anim_frame, ship_x + 20, ship_y);
    }
    else 
    {
        gfx_draw_tile(10, ship_x, ship_y);
        gfx_draw_tile(11, ship_x + 20, ship_y);
    }
}

void ship_hit(void)
{
    ship_exploding = true;
    ship_exploding_time = scene_frame_count + 180;
    sfx_play_sound(GAME_SOUND_ID_SHIP_EXPLOSION);

    game_state = GAME_STATE_SHIP_EXPLODING;
    game_state_time = scene_frame_count + 120;
    bullets_reset();
    cd_stop_music_xa();
}