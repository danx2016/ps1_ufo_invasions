#ifndef _SCENE_MANAGER_H
#define _SCENE_MANAGER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define SCENES_MAX_NUM 8

typedef struct
{
    void (*init)(void);
    void (*on_enter)(void);
    void (*on_exit)(void);
    void (*on_transition_finished)(void);
    void (*fixed_update)(void);
    void (*render)(void);
} SCENE;

extern uint32_t scene_frame_count;
extern bool scene_is_transition_finished;

extern SCENE *scene_create(void);

extern void scene_init_all(void);
extern void scene_add(size_t scene_id, SCENE *scene);
extern void scene_switch_to(size_t scene_id);

extern void scene_fixed_update(void);
extern void scene_render(void);

extern bool scene_is_common_background_visible;
extern void (*scene_render_common_background)(void);

#endif /* _SCENE_MANAGER_H */