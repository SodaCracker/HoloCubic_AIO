#ifndef APP_JETLAG_GUI_H
#define APP_JETLAG_GUI_H

struct DisplayInfo
{
    int hour;
    int minute;
    char city_name[16];
};

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define ANIEND                      \
    while (lv_anim_count_running()) \
        lv_task_handler();

    void jetlag_gui_init(void);
    void jetlag_gui_release(void);
    void time_display(struct DisplayInfo display_info, lv_scr_load_anim_t anim_type);
    void time_display_init(lv_scr_load_anim_t anim_type);
    
    extern const lv_img_dsc_t app_jetlag;

#ifdef __cplusplus
}
#endif

#endif