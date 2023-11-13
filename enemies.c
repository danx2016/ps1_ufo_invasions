#include <stdio.h>
#include <rand.h>

#include "gfx.h"
#include "sfx.h"
#include "cd.h"
#include "scene_manager.h"
#include "game.h"
#include "ship.h"
#include "bullets.h"
#include "enemies.h"

int16_t enemies_last_row;
int16_t enemies_x;
int16_t enemies_y;
int16_t enemies_dir;

int32_t num_enemies_alive;

int16_t enemies_min_col;
int16_t enemies_max_col;

ENEMY enemies[ENEMY_ROWS][ENEMY_COLS];
int32_t showing_enemy_id;
int32_t last_showing_enemy_id;


void enemies_reset(void)
{
    showing_enemy_id = -1;
    last_showing_enemy_id = -1;

    enemies_x = 32;
    enemies_y = ENEMIES_START_Y;
    enemies_dir = 1;

    num_enemies_alive = ENEMY_COLS * ENEMY_ROWS;

    enemies_min_col = 0;
    enemies_max_col = 7;
    enemies_last_row = ENEMY_ROWS - 1;

    for (int row = 0; row < ENEMY_ROWS; row++)
    {
        for (int col = 0; col < ENEMY_COLS; col++)
        {
            enemies[row][col].col = col;
            enemies[row][col].row = row;
            enemies[row][col].exploding_time = 0;
            enemies[row][col].state = ENEMY_STATE_DEAD;
        }
    }
}

void enemies_reset_all_explosions(void)
{
    for (int row = 0; row < ENEMY_ROWS; row++)
    {
        for (int col = 0; col < ENEMY_COLS; col++)
        {
            if (enemies[row][col].state == ENEMY_STATE_EXPLODING)
            {
                enemies[row][col].state = ENEMY_STATE_DEAD;
            }
        }
    }
}

bool enemies_exceeded_max_y_limit(void)
{
    return (enemies_y + (enemies_last_row + 1) * 20 - 6) >= ENEMIES_MAY_Y;
}

void enemies_fixed_update(void)
{
    if (num_enemies_alive <= 2)
    {
        enemies_x += 2 * enemies_dir;
    }
    else
    {
        enemies_x += enemies_dir;
    }

    if (enemies_dir > 0 && enemies_x > 256 - enemies_max_col * 24 - 24)
    {
        enemies_dir = -1;
        enemies_x = 256 - enemies_max_col * 24 - 24;
        enemies_y += 3;
    }
    else if (enemies_dir < 0 && enemies_x < -enemies_min_col * 24)
    {
        enemies_dir = 1;
        enemies_x = -enemies_min_col * 24;
        enemies_y += 3;
    }

    num_enemies_alive = 0;
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            switch (enemies[row][col].state)
            {
                case ENEMY_STATE_ALIVE:
                {
                    const int32_t bx = 10 + ship_bullet.x;
                    const int32_t by = 5 + ship_bullet.y;
                    const int32_t sx = enemies_x + col * 24 + 3;
                    const int32_t sy = enemies_y + row * 20 + 3;
                    const int32_t sw = 20 - 6;
                    const int32_t sh = 16 - 6;

                    if(game_check_collision(bx, by, sx, sy, sw, sh))
                    {
                        enemies[row][col].state = ENEMY_STATE_EXPLODING;
                        enemies[row][col].exploding_time = scene_frame_count + 30;
                        ship_bullet.is_actived = false;
                        ship_bullet.x = -8;
                        ship_bullet.y = -8;
                        sfx_play_sound(GAME_SOUND_ID_ENEMY_EXPLOSION);
                        game_add_score((4 - row) * 50);
                    }

                    num_enemies_alive++;
                    break;
                }

                case ENEMY_STATE_EXPLODING:
                {
                    if (scene_frame_count >= enemies[row][col].exploding_time)
                    {
                        enemies[row][col].state = ENEMY_STATE_DEAD;
                    }
                    break;
                }
            }
        }
    }

    // check game cleared    
    if (num_enemies_alive == 0)
    {
        game_state = GAME_STATE_LEVEL_CLEARED;
        game_state_time = scene_frame_count + 300;
        bullets_reset();
        cd_stop_music_xa();
        return;
    }

    // update enemies_min_col
    for (size_t col = enemies_min_col; col < 8; col++)
    {
        for (size_t row = 0; row < 4; row++)
        {
            enemies_min_col = col;
            if (enemies[row][col].state != ENEMY_STATE_DEAD)
            {
                goto exit_outer_loop_a;
            }
        }
    }
    exit_outer_loop_a:

    // update enemies_max_col
    for (int col = enemies_max_col; col >= 0; col--)
    {
        for (int row = 0; row < 4; row++)
        {
            enemies_max_col = col;
            if (enemies[row][col].state != ENEMY_STATE_DEAD)
            {
                goto exit_outer_loop_b;
            }
        }
    }
    exit_outer_loop_b:

    // update enemies_last_row
    for (size_t row = enemies_last_row; row >= 0; row--)
    {
        enemies_last_row = row;
        for (size_t col = 0; col < 8; col++)
        {
            if (enemies[row][col].state == ENEMY_STATE_ALIVE)
            {
                goto exit_outer_loop_c;
            }
        }
    }
    exit_outer_loop_c:

    // check if enemies exceeded max y limit
    if (num_enemies_alive > 0 && enemies_exceeded_max_y_limit())
    {
        game_state = GAME_STATE_ENEMIES_EXCEEDED_MAX_Y;
        game_state_time = scene_frame_count + 120;
        return;
    }
    //printf("limit = %d / enemies y = %d \n", ENEMIES_MAY_Y, enemies_y + (enemies_last_row + 1) * 20 - 8);
}

void enemies_render(void)
{
    int32_t anim_frame_a = (scene_frame_count % 30) > 15 ? 5 : 0;
    int32_t anim_frame_b = (scene_frame_count % 20) > 10 ? 5 : 0;

    int16_t dx = enemies_x;
    int16_t dy = enemies_y;

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            switch (enemies[row][col].state)
            {
                case ENEMY_STATE_ALIVE:
                {
                    gfx_draw_tile(row + anim_frame_a, 24 * col + dx, 20 * row + dy);
                    break;
                }

                case ENEMY_STATE_EXPLODING:
                {
                    gfx_draw_tile(4 + anim_frame_b, 24 * col + dx, 20 * row + dy);
                    break;
                }
            }
        }
    }
}

ENEMY *enemies_get_random_alive(void)
{
    if (num_enemies_alive > 0)
    {
        int32_t next_random_alive_enemy_count = rand() % num_enemies_alive;

        for (size_t col = 0; col < 8; col++)
        {
            for (size_t row = 0; row < 4; row++)
            {
                if (enemies[row][col].state == ENEMY_STATE_ALIVE)
                {
                    if (next_random_alive_enemy_count == 0)
                    {
                        return &enemies[row][col];
                    }
                    next_random_alive_enemy_count--;
                }
            }
        }
    }
    else
    {
        return NULL;
    }
}