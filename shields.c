#include "gfx.h"
#include "game.h"
#include "shields.h"
#include "shields.h"

bool shields[2][16];

void shields_reset(void)
{
    const int32_t shield_cols[] = { 3, 4, 7, 8, 11, 12 };
    for (int32_t row = 0; row < 2; row++)
    {
        for (int32_t col = 0; col < 16; col++)
        {
            shields[row][col] = false;
            for (int32_t s = 0; s < 8; s++)
            {
                if (col == shield_cols[s])
                {
                    shields[row][col] = true;
                    break;
                }
            }
        }
    }
}

void shields_check_collision(BULLET *bullet)
{
    for (int32_t row = 0; row < 2; row++)
    {
        for (int32_t col = 0; col < 16; col++)
        {
            if (shields[row][col])
            {
                if (game_check_collision(10 + bullet->x, 7 + bullet->y, col * SHIELD_WIDTH, row * SHIELD_HEIGHT + SHIELDS_Y, SHIELD_WIDTH, SHIELD_HEIGHT))
                {
                    shields[row][col] = false;
                    bullet->x = -8;
                    bullet->y = -8;
                    bullet->is_actived = false;
                    goto outer_loop;
                }
            }
        }
    }    
    outer_loop:
}

void shields_render(void)
{
    for (int row = 0; row < 2; row++)
    {
        for (int col = 0; col < 16; col++)
        {
            if (shields[row][col])
            {
                gfx_fill_rect(col * 16, row * 8 + SHIELDS_Y, 16, 8, (col - 16) * 16, (row + 1) * 64, col * 16);
            }
        }
    }    
}