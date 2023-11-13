#ifndef _INPUT_H
#define _INPUT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <libetc.h>


typedef enum
{
    INPUT_ACTION_UP     = PADLup,
    INPUT_ACTION_DOWN   = PADLdown,
    INPUT_ACTION_LEFT   = PADLleft,
    INPUT_ACTION_RIGHT  = PADLright,
    INPUT_ACTION_FIRE   = PADRdown,
    INPUT_ACTION_START  = PADstart,
    INPUT_ACTION_SELECT = PADselect
} INPUT_ACTION;

extern void input_init(void);
extern void input_fixed_update(void);
extern bool input_is_action_pressed(uint32_t controller_id, INPUT_ACTION action);
extern bool input_is_action_just_pressed(uint32_t controller_id,INPUT_ACTION action);

#endif /* _INPUT_H */