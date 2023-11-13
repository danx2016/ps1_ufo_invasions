#ifndef _ENEMIES_H
#define _ENEMIES_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define ENEMIES_START_Y  24
#define ENEMIES_MAY_Y   168

extern int16_t enemies_last_row;
extern int16_t enemies_x;
extern int16_t enemies_y;
extern int16_t enemies_dir;

extern int32_t num_enemies_alive;

extern int16_t enemies_min_col;
extern int16_t enemies_max_col;

typedef enum
{
    ENEMY_STATE_ALIVE,
    ENEMY_STATE_EXPLODING,
    ENEMY_STATE_DEAD
} ENEMY_STATE;

typedef struct
{
    int16_t col;
    int16_t row;
    uint32_t exploding_time;
    ENEMY_STATE state;
} ENEMY;

#define ENEMY_ROWS 4
#define ENEMY_COLS 8
#define ENEMY_NUMS (ENEMY_COLS * ENEMY_ROWS)

extern ENEMY enemies[ENEMY_ROWS][ENEMY_COLS];
extern int32_t showing_enemy_id;
extern int32_t last_showing_enemy_id;

extern void enemies_reset(void);
extern void enemies_reset_all_explosions(void);

extern bool enemies_exceeded_max_y_limit(void);

extern void enemies_fixed_update(void);
extern void enemies_render(void);

extern ENEMY *enemies_get_random_alive(void);
#endif /* _ENEMIES_H */