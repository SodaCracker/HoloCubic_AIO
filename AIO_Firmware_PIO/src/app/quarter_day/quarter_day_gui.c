#include "quarter_day_gui.h"

static lv_style_t default_style;
static lv_style_t time_sytle;
static lv_style_t label_style;

static lv_obj_t *screen = NULL;
static lv_obj_t *time_label;
static lv_obj_t *city_label;

static const char *city_names[2] = {"Lunds", "Shanghai"};

void quarter_day_gui_init(void)
{
    lv_style_init(&default_style);
    lv_style_init(&time_sytle);
    lv_style_init(&label_style);
}

// void display_time(struct DisplayInfo displayInfo)
// {
    // lv_label_set_text_fmt(time_label, "%d:%d", displayInfo.hour, displayInfo.minute);
    // lv_label_set_text_fmt(city_label, "%s", 0 == displayInfo.city_idx ? &city_names[0] : &city_names[1]);
// }