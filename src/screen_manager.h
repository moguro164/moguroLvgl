#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "lvgl.h"

typedef enum {
    SCR_HOME,
    SCR_MAIN,
    SCR_SETTINGS,
    SCR_DETAIL,
    SCR_COUNT
} screen_id_t;

void screen_manager_init(void);
void navigate_to(screen_id_t id, lv_scr_load_anim_t anim);

#endif // SCREEN_MANAGER_H
