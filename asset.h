#ifndef _ASSET_H
#define _ASSET_H

#include <stddef.h>
#include <stdint.h>
#include <libgpu.h>

#include "gfx_font.h"

extern uint8_t *asset_load(uint8_t *filename);
extern TIM_IMAGE *asset_load_image(uint8_t *filename);
extern FONT *asset_load_font(uint8_t *bin_filename, uint8_t *tim_filename);

#endif /* _ASSET_H */