#ifndef _FMV_H
#define _FMV_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

extern bool fmv_is_play_finished;

extern void fmv_init(uint8_t *filename, uint32_t frame_end);
extern void fmv_render_next_frame();

#endif /* _FMV_H */