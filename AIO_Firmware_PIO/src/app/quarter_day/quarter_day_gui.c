#include "quarter_day_gui.h"

static lv_style_t default_style;
static lv_style_t time_sytle;
static lv_style_t city_style;

static lv_obj_t *scr = NULL;
static lv_obj_t *time_label;
static lv_obj_t *city_label;

static const char *city_names[2] = {"Lunds", "Shanghai"};

void quarter_day_gui_init(void)
{
    lv_style_init(&default_style);
    lv_style_init(&time_sytle);
    lv_style_init(&city_style);
}

void quarter_day_gui_release(void)
{
    if (NULL != scr)
    {
        lv_obj_clean(scr);
        scr = NULL;
        time_label = NULL;
        city_label = NULL;
    }
}

void time_display(struct DisplayInfo display_info)
{
    time_display_init(LV_SCR_LOAD_ANIM_NONE);
    lv_label_set_text_fmt(time_label, "%d:%d", display_info.hour, display_info.minute);
    lv_label_set_text(city_label, display_info.city_name);
    return;
}

void time_display_init(lv_scr_load_anim_t anim_type)
{
    lv_obj_t *act_obj = lv_scr_act();
    if (act_obj == scr)
    {
        return;
    }

    quarter_day_gui_release();
    lv_obj_clean(act_obj);

    scr = lv_obj_create(NULL);
    time_label = lv_label_create(scr);
    city_label = lv_label_create(scr);

    lv_obj_align(time_label, LV_ALIGN_TOP_LEFT, 10, 50);
    lv_obj_align(city_label, LV_ALIGN_LEFT_MID, 10, 100);
}