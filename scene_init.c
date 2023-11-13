#include "scene_factory.h"
#include "gfx.h"
#include "gfx_font.h"
#include "fmv.h"
#include "input.h"
#include "game.h"

static uint32_t state = 0;
static uint32_t start_time = 0;

static void scene_init_init(void)
{
    // do nothing
}

static void scene_init_on_enter(void)
{
    fmv_init(GAME_ASSET_FILENAME_FMV_INIT, GAME_ASSET_END_FRAME_FMV_INIT);
}

static void scene_init_on_exit(void)
{
    scene_is_common_background_visible = true;
}

static void scene_init_on_transition_finished(void)
{
    // do nothing
    start_time = scene_frame_count + 180;
}

static void scene_init_fixed_update(void)
{
    if (fmv_is_play_finished)
    {
        scene_switch_to(SCENE_ID_TITLE);
    }
}

static void scene_init_render(void)
{
    fmv_render_next_frame();
}

SCENE* scene_init_create(void)
{
    SCENE *scene_init = scene_create();

    scene_init->init = scene_init_init;
    scene_init->on_enter = scene_init_on_enter;
    scene_init->on_exit = scene_init_on_exit;
    scene_init->on_transition_finished = scene_init_on_transition_finished;
    scene_init->fixed_update = scene_init_fixed_update;
    scene_init->render = scene_init_render;

    return scene_init;
}