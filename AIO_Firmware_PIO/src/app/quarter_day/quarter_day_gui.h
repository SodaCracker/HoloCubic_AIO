#ifndef APP_QUARTER_DAY_GUI_H
#define APP_QUARTER_DAY_GUI_H

struct TimeInfo
{
    int hour;
    int minute;
    char *location;
};

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

#define ANIEND                      \
    while (lv_anim_count_running()) \
        lv_task_handler(); // 等待动画完成

    void quarter_day_gui_init(void);
    void display_time(TimeInfo time);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

    extern const lv_img_dsc_t app_quarter_day;

#ifdef __cplusplus
}
#endif

#endif