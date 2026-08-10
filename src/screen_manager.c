#include "screen_manager.h"
#include "home_screen.h"

typedef lv_obj_t* (*screen_create_fn_t)(void);

typedef struct {
    screen_create_fn_t create_fn;
    bool cache;          // true: 常駐, false: 都度生成
    lv_obj_t *instance;  // 生成済みならセットされる
} screen_entry_t;

static screen_entry_t g_screens[SCR_COUNT] = {
    [SCR_HOME]     = { create_home_screen,     true,  NULL },
    [SCR_MAIN]     = { create_main_screen,     true,  NULL },
    [SCR_SETTINGS] = { create_settings_screen, false, NULL },
    [SCR_DETAIL]   = { create_detail_screen,   false, NULL },
};

void navigate_to(screen_id_t id, lv_scr_load_anim_t anim) {
    screen_entry_t *entry = &g_screens[id];

    if (entry->cache) {
        // 常駐: なければ生成、あれば使い回す
        if (entry->instance == NULL) {
            entry->instance = entry->create_fn();
        }
        lv_scr_load_anim(entry->instance, anim, 300, 0, false);
    } else {
        // 都度生成: 毎回作って古いものは自動破棄
        lv_obj_t *scr = entry->create_fn();
        lv_scr_load_anim(scr, anim, 300, 0, true);
    }
}

void screen_manager_init(void) {
    g_screens[SCR_HOME].instance = g_screens[SCR_HOME].create_fn();

    // 他の常駐画面(SCR_MAINなど)は初回アクセス時に生成するので、
    // ここでは何もしない(instanceはNULLのまま)

    // 最初の画面をロード(アニメーションなし)
    lv_scr_load(g_screens[SCR_HOME].instance);
}
