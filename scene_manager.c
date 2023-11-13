#include "scene_manager.h"
#include "mem.h"
#include "gfx.h"

uint32_t scene_frame_count = 0;
bool scene_is_transition_finished = true;

bool scene_is_common_background_visible = true;
void (*scene_render_common_background)(void) = NULL;

typedef enum
{
    SCENE_TRANSITION_STATE_FINISHED,
    SCENE_TRANSITION_STATE_IN,
    SCENE_TRANSITION_STATE_OUT
} SCENE_TRANSITION_STATE;

static SCENE *scenes[SCENES_MAX_NUM];
static SCENE *current_scene = NULL;
static SCENE *next_scene = NULL;
static SCENE_TRANSITION_STATE scene_transition_state = SCENE_TRANSITION_STATE_FINISHED;
static int32_t scene_transition_progress = 0;

SCENE *scene_create(void)
{
    SCENE *scene = (SCENE*) mem_alloc(sizeof(SCENE));
    return scene;
}

void scene_init_all(void)
{
    for (int i = 0; i < SCENES_MAX_NUM; i++)
    {
        SCENE *scene = scenes[i];
        if (scene != NULL && scene->init != NULL)
        {
            scene->init();
        }
    }
}

void scene_add(size_t scene_id, SCENE *scene)
{
    scenes[scene_id] = scene;
}

void scene_switch_to(size_t scene_id)
{
    next_scene = scenes[scene_id];
    scene_transition_progress = 0;
    scene_transition_state = SCENE_TRANSITION_STATE_IN;
    scene_is_transition_finished = false;
}

void scene_fixed_update(void)
{
    switch (scene_transition_state)
    {
        case SCENE_TRANSITION_STATE_FINISHED:
        {
            if (current_scene != NULL)
            {
                current_scene->fixed_update();
            }
            break;            
        }

        case SCENE_TRANSITION_STATE_IN:
        {
            scene_transition_progress += 1;
            if (scene_transition_progress >= 130)
            {
                scene_transition_progress = 130;
                scene_transition_state = SCENE_TRANSITION_STATE_OUT;
                if (current_scene != NULL)
                {
                    current_scene->on_exit();
                }
                current_scene = next_scene;
                current_scene->on_enter();
                next_scene = NULL;
            }
            break;
        }

        case SCENE_TRANSITION_STATE_OUT:
        {
            scene_transition_progress -= 1;
            if (scene_transition_progress <= 0) {
                scene_transition_progress = 0;
                scene_transition_state = SCENE_TRANSITION_STATE_FINISHED;
                scene_is_transition_finished = true;
                current_scene->on_transition_finished();
            }
            break;
        }
    }

    scene_frame_count++;
}

// progress = 0~20
static void scene_draw_transition(int32_t progress)
{
    const int32_t cell_size = 16;
    const int32_t cell_half_size = cell_size / 2;
    const int32_t cols = GFX_SCREEN_WIDTH / cell_size;
    const int32_t rows = GFX_SCREEN_HEIGHT / cell_size;
    for (int row = 0; row < rows; row++)
    {
        for (int col = 0; col < cols; col++)
        {
            int32_t p = (row * cols + col) / 4 + progress - (cols * rows) / 4;
            p = p < 0 ? 0 : p > cell_size ? cell_size : p;
            int32_t half_p = p / 2;
            int16_t rx = col * cell_size + cell_half_size - half_p;
            int16_t ry = row * cell_size + cell_half_size - half_p;
            gfx_fill_rect(rx, ry, p, p, 0, 0, 0);
        }
    }
}

void scene_render(void)
{
    if (scene_is_common_background_visible && scene_render_common_background != NULL)
    {
        scene_render_common_background();
    }
    
    if (current_scene != NULL)
    {
        current_scene->render();
    }

    if (scene_transition_progress > 0)
    {
        scene_draw_transition(scene_transition_progress);
    }
}
