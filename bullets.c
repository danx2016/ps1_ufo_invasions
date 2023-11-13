#include <rand.h>

#include "gfx.h"
#include "sfx.h"
#include "input.h"
#include "scene_manager.h"
#include "game.h"
#include "ship.h"
#include "enemies.h"
#include "shields.h"
#include "bullets.h"

BULLET ship_bullet;

BULLET enemies_bullets[ENEMIES_NUM_MAX_BULLETS];
uint32_t next_enemy_bullet_time;

void bullets_reset()
{
    bullets_disable(&ship_bullet);

    for (int i = 0; i < ENEMIES_NUM_MAX_BULLETS; i++)
    {
        enemies_bullets[i].speed_y = (i % 2) + 1;
        bullets_disable(&enemies_bullets[i]);
    }
}

static void fixed_update_ship_bullet()
{
    if (!ship_bullet.is_actived && input_is_action_just_pressed(0, INPUT_ACTION_FIRE))
    {
        ship_bullet.x = ship_x + 9;
        ship_bullet.y = 200;
        ship_bullet.is_actived = true;
        sfx_play_sound(GAME_SOUND_ID_SHIP_LASER);
    }

    if (ship_bullet.is_actived)
    {
        ship_bullet.y -= 4;

        if (ship_bullet.y < -8)
        {
            bullets_disable(&ship_bullet);
        }

        shields_check_collision(&ship_bullet);
    }
}

static bool spawn_enemy_bullet(int16_t enemy_alive_col, int16_t enemy_alive_row)
{
    for (int32_t i = 0; i < ENEMIES_NUM_MAX_BULLETS; i++)
    {
        BULLET *bullet = &enemies_bullets[i];
        if (!bullet->is_actived)
        {
            bullet->is_actived = true;
            bullet->x = enemies_x + enemy_alive_col * 24 + 10;
            bullet->y = enemies_y + enemy_alive_row * 20 + 5;
            return true;
        }
    } 
    return false;
}

static void try_spawn_next_enemy_bullet()
{
    if (num_enemies_alive > 0 && scene_frame_count >= next_enemy_bullet_time)
    {
        ENEMY *enemy_alive = enemies_get_random_alive();
        if (enemy_alive != NULL && spawn_enemy_bullet(enemy_alive->col, enemy_alive->row))
        {
            sfx_play_sound(GAME_SOUND_ID_ENEMY_LASER);
        }
        next_enemy_bullet_time = scene_frame_count + (rand() % 90);
    }
}

static void fixed_update_enemies_bullets()
{
    try_spawn_next_enemy_bullet();
    
    // move enemy bullets
    for (int32_t i = 0; i < ENEMIES_NUM_MAX_BULLETS; i++)
    {
        BULLET *bullet = &enemies_bullets[i];
        if (bullet->is_actived)
        {
            bullet->y += bullet->speed_y;

            if (bullet->y > 240)
            {
                bullets_disable(bullet);
            }
        }

        // check collision with ship
        if (bullet->is_actived)
        {
            const int16_t bx = 10 + bullet->x;
            const int16_t by = 10 + bullet->y;
            const int16_t sx = ship_x + 3;
            const int16_t sy = ship_y;
            const int16_t sw = 40 - 6;
            const int16_t sh = 16;

            if (!ship_exploding && game_check_collision(bx, by, sx, sy, sw, sh))
            {
                ship_hit();
                bullets_disable(bullet);
            }
        }

        // check collision with shields
        if (bullet->is_actived)
        {
            shields_check_collision(bullet);
        }
    }  
}

void bullets_fixed_update(void)
{
    fixed_update_ship_bullet();
    fixed_update_enemies_bullets();    
}

void bullets_render(void)
{
    // draw enemies bullets
    for (int32_t i = 0; i < ENEMIES_NUM_MAX_BULLETS; i++)
    {
        BULLET *bullet = &enemies_bullets[i];
        if (bullet->is_actived)
        {
            gfx_draw_tile(13, bullet->x, bullet->y);
        }

    }     

    // draw ship bullet
    if (ship_bullet.is_actived)
    {
        gfx_draw_tile(12, ship_bullet.x, ship_bullet.y);
    }    
}

void bullets_disable(BULLET *bullet)
{
    bullet->x = -8;
    bullet->y = -8;                
    bullet->is_actived = false;
}