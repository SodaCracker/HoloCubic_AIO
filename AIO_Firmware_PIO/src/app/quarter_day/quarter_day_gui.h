#ifndef APP_QUARTER_DAY_GUI_H
#define APP_QUARTER_DAY_GUI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "lvgl.h"

   struct DisplayInfo
    {
        int hour;
        int minute;
        int city_idx;
    };

    extern const lv_img_dsc_t app_quarter_day;

    void quarter_day_gui_init(void);
    void display_time(struct DisplayInfo displayInfo);

#ifdef __cplusplus
}
#endif

#endif