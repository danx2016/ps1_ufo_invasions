#ifndef _SHIELDS_H
#define _SHIELDS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "bullets.h"

#define SHIELDS_Y      180
#define SHIELD_HEIGHT    8
#define SHIELD_WIDTH    16

extern bool shields[2][16];

extern void shields_reset(void);
extern void shields_check_collision(BULLET *bullet);
extern void shields_render(void);

#endif /* _SHIELDS_H */