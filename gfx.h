#ifndef _GFX_H
#define _GFX_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <libgpu.h>

#define GFX_SCREEN_WIDTH 256
#define GFX_SCREEN_HEIGHT 240

#define GFX_PRIMITIVES_BUFFER_SIZE 65535

#define GFX_MAX_TILESETS 8

typedef struct
{
    uint16_t cols;
    uint16_t rows;
    uint16_t tile_width;
    uint16_t tile_height;
    TIM_IMAGE *tim_image;
    DR_TPAGE tpage;
} TILESET;

extern uint32_t gfx_db;

extern void gfx_init(void);
extern void gfx_swap_buffers(void);

extern void gfx_add_tileset(size_t tileset_id, TIM_IMAGE *tim_image, uint16_t cols, uint16_t rows);
extern void gfx_use_tileset(size_t tileset_id);
extern void gfx_draw_tile(size_t tile_id, int16_t x, int16_t y);
extern void gfx_draw_custom_tile(int16_t src_x, int16_t src_y, int16_t src_w, int16_t src_h, int16_t dst_x, int16_t dst_y);

extern void gfx_fill_rect(int16_t x, int16_t y, uint16_t w, uint16_t h, uint8_t r,  uint8_t g,  uint8_t b);

#endif /* _GFX_H */