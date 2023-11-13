#include <stdio.h>
#include "input.h"

static uint32_t buttons_pressed_states = 0;
static uint32_t buttons_just_pressed_states = 0;

void input_init(void)
{
    PadInit(0);
}

void input_fixed_update(void)
{
    uint32_t buttons_states = PadRead(0);
    buttons_just_pressed_states = (buttons_states ^ buttons_pressed_states) & buttons_states;
    buttons_pressed_states = buttons_states;
}

bool input_is_action_pressed(uint32_t controller_id, INPUT_ACTION action)
{
    return (buttons_pressed_states & _PAD(controller_id, action));
}

bool input_is_action_just_pressed(uint32_t controller_id,INPUT_ACTION action)
{
    //printf("buttons_pressed = %d \n", buttons_pressed);
    return (buttons_just_pressed_states & _PAD(controller_id, action));
}
