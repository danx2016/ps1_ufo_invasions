#include <libetc.h>
#include <libgpu.h>

#include "mem.h"
#include "asset.h"
#include "gfx.h"

static uint32_t primitives_buffer[2][GFX_PRIMITIVES_BUFFER_SIZE / 4];
static uint8_t *next_primitive;
static uint8_t *last_primitive;
static uint32_t root_primitive[2];

static DRAWENV draw[2];
static DISPENV disp[2];
uint32_t gfx_db = 0;

static TILESET *tilesets[GFX_MAX_TILESETS];
static TILESET *current_tileset = NULL;

// kindly provided by @spicyjpeg
static bool is_gpu_in_pal_mode(void) {
    return (*((uint32_t *) 0x1f801814) >> 20) & 1;
}

void gfx_init(void)
{
    bool pal_mode = is_gpu_in_pal_mode();
    ResetGraph(0);
    
    SetDefDispEnv(&disp[0], 0, 0, GFX_SCREEN_WIDTH, GFX_SCREEN_HEIGHT);
    SetDefDispEnv(&disp[1], 0, GFX_SCREEN_HEIGHT, GFX_SCREEN_WIDTH, GFX_SCREEN_HEIGHT);
    SetDefDrawEnv(&draw[0], 0, GFX_SCREEN_HEIGHT, GFX_SCREEN_WIDTH, GFX_SCREEN_HEIGHT);
    SetDefDrawEnv(&draw[1], 0, 0, GFX_SCREEN_WIDTH, GFX_SCREEN_HEIGHT);
    
    int32_t display_height = pal_mode ? 256 : 240;
    disp[0].screen.h = display_height;
    disp[1].screen.h = display_height;

    draw[0].isbg = 1;
    draw[1].isbg = 1;
    setRGB0(&draw[0], 0, 0, 0);
    setRGB0(&draw[1], 0, 0, 0);

    // clear vram
    RECT rect = { 0, 0, 1024, 512 };
    ClearImage(&rect, 0, 0, 0);

    next_primitive = (uint8_t*) primitives_buffer[gfx_db];
    last_primitive = (uint8_t*) &root_primitive[gfx_db];

    SetDispMask(1);
}

void gfx_swap_buffers()
{
    DrawSync(0);
    VSync(0);

    PutDispEnv(&disp[gfx_db]);
    PutDrawEnv(&draw[gfx_db]);

    TermPrim(last_primitive);
    DrawOTag((uint32_t*) &root_primitive[gfx_db]);

    gfx_db = !gfx_db;

    next_primitive = (uint8_t*) primitives_buffer[gfx_db];
    last_primitive = (uint8_t*) &root_primitive[gfx_db];
}

void gfx_add_tileset(size_t tileset_id, TIM_IMAGE *tim_image, uint16_t cols, uint16_t rows)
{
    TILESET *tileset = mem_alloc(sizeof(TILESET));
    tileset->cols = cols;
    tileset->rows = rows;
    tileset->tile_width = (tim_image->prect->w << 2 - (tim_image->mode & 3)) / cols;
    tileset->tile_height = tim_image->prect->h / rows;
    tileset->tim_image = tim_image;

    // pre-create dr_tpage
    uint16_t tp = getTPage(tim_image->mode & 3, 0, tim_image->prect->x, tim_image->prect->y);
    setDrawTPage(&tileset->tpage, 0, 0, tp);

    tilesets[tileset_id] = tileset;
}

void gfx_use_tileset(size_t tileset_id)
{
    current_tileset = tilesets[tileset_id];
    CatPrim(last_primitive, &current_tileset->tpage);
    last_primitive = (uint8_t*) &current_tileset->tpage;
}

void gfx_draw_tile(size_t tile_id, int16_t x, int16_t y)
{
    TIM_IMAGE *tim_image = current_tileset->tim_image;

    SPRT *sprt = (SPRT*) next_primitive;
    setSprt(sprt);
    setXY0(sprt, x, y);
    uint8_t u = (tile_id % current_tileset->cols) * current_tileset->tile_width;
    uint8_t v = (tile_id / current_tileset->cols) * current_tileset->tile_height;
    setUV0(sprt, u, v);
    setWH(sprt, current_tileset->tile_width, current_tileset->tile_height);
    setRGB0(sprt, 128, 128, 128);
    setClut(sprt, tim_image->crect->x, tim_image->crect->y);
    CatPrim(last_primitive, sprt);
    last_primitive = next_primitive;
    next_primitive += sizeof(SPRT);
}

void gfx_draw_custom_tile(int16_t src_x, int16_t src_y, int16_t src_w, int16_t src_h, int16_t dst_x, int16_t dst_y)
{
    TIM_IMAGE *tim_image = current_tileset->tim_image;

    SPRT *sprt = (SPRT*) next_primitive;
    setSprt(sprt);
    setXY0(sprt, dst_x, dst_y);
    setWH(sprt, src_w, src_h);
    setUV0(sprt, src_x, src_y);
    setRGB0(sprt, 128, 128, 128);
    setClut(sprt, tim_image->crect->x, tim_image->crect->y);
    CatPrim(last_primitive, sprt);
    last_primitive = next_primitive;
    next_primitive += sizeof(SPRT);
}

void gfx_fill_rect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t r,  uint8_t g,  uint8_t b)
{
    TILE *tile = (TILE*) next_primitive;
    setTile(tile);
    setXY0(tile, x, y);
    setWH(tile, w, h);
    setRGB0(tile, r, g, b);
    CatPrim(last_primitive, tile);
    last_primitive = next_primitive;
    next_primitive += sizeof(SPRT);
}