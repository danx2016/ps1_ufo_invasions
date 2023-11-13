#ifndef _CD_H
#define _CD_H

#include <stddef.h>
#include <stdint.h>

extern void cd_init(void);
extern void *cd_load_file(uint8_t *filename);

extern void cd_play_music_xa(uint8_t *filename);
extern void cd_stop_music_xa();

#endif /* _CD_H */