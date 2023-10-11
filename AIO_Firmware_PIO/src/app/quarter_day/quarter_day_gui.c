#include "quarter_day_gui.h"

static lv_style_t default_style;
static lv_style_t time_sytle;
static lv_style_t label_style;

static lv_obj_t *screen = NULL;
static lv_obj_t *time_label;
static lv_obj_t *city_label;

void quarter_day_gui_init(void)
{
    lv_style_init(&default_style);
    lv_style_init(&time_sytle);
    lv_style_init(&label_style);
}

void display_time(struct TimeInfo timeInfo)
{
    lv_label_set_text_fmt(time_label, "%02d:%02d", timeInfo.hour, timeInfo.minute);
    lv_label_set_text_fmt(city_label, "%s", timeInfo.location);
}