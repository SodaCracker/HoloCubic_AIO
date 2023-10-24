#ifndef APP_QUARTER_DAY_GUI_H
#define APP_QUARTER_DAY_GUI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define ANIEND                      \
    while (lv_anim_count_running()) \
        lv_task_handler();

   struct DisplayInfo
    {
        int hour;
        int minute;
        char *city_name;
    };

    void quarter_day_gui_init(void);
    void quarter_day_gui_release(void);
    void time_display(struct DisplayInfo display_info);
    void time_display_init(lv_scr_load_anim_t anim_type);
    
    extern const lv_img_dsc_t app_quarter_day;

#ifdef __cplusplus
}
#endif

#endif