#ifndef _SHIP_H
#define _SHIP_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

extern int16_t ship_x;
extern int16_t ship_y;

extern bool ship_exploding;
extern uint32_t ship_exploding_time;

extern void ship_reset(void);
extern void ship_fixed_update(void);
extern void ship_render(void);
extern void ship_hit(void);

#endif /* _SHIP_H */