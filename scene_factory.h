#ifndef _SCENE_TITLE_H
#define _SCENE_TITLE_H

#include "scene_manager.h"

#define SCENE_ID_INIT    0
#define SCENE_ID_TITLE   1
#define SCENE_ID_PLAYING 2

extern SCENE* scene_init_create(void);
extern SCENE* scene_title_create(void);
extern SCENE* scene_playing_create(void);

#endif /* _SCENE_TITLE_H */