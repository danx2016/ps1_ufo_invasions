#ifndef _BULLETS_H
#define _BULLETS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t speed_y;
    bool is_actived;
} BULLET;

extern BULLET ship_bullet;

#define ENEMIES_NUM_MAX_BULLETS 10
extern BULLET enemies_bullets[ENEMIES_NUM_MAX_BULLETS];
extern uint32_t next_enemy_bullet_time;

extern void bullets_reset(void);
extern void bullets_fixed_update(void);
extern void bullets_render(void);
extern void bullets_disable(BULLET *bullet);

#endif /* _BULLETS_H */