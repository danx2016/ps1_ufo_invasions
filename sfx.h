#ifndef _SFX_H
#define _SFX_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define SFX_SPU_START_ADDR 0x1010
#define SFX_MAX_SOUNDS_NUM 32
#define SFX_VAG_HEADER_SIZE 48

extern void sfx_init(void);

extern void sfx_add_sound(size_t sound_id, uint8_t *sound_data);
extern void sfx_play_sound(size_t sound_id);


#endif /* _SFX_H */