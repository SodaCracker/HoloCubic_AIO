#include "ArduinoJson.h"
#include "ESP32Time.h"
#include "network.h"
#include "sys/app_controller.h"
#include "jetlag_gui.h"
#include "WString.h"

#define JETLAG_NAME "Jet Lag"
#define TIME_API "http://worldtimeapi.org/api/timezone/"
#define CITY_SIZE 2
#define TIME_UPDATE_INTERVAL 900000

static const char timezones[2][32] = {"Asia/Shanghai", "Europe/Stockholm"};
static const char city_names[2][16] = {"Shanghai", "Lund"};

struct RuneTimeData
{
    int city_idx;
    unsigned long prev_time;
    long long prev_local_time;
    long long prev_net_time;
    unsigned int forced_update;
    unsigned int update_type;
    ESP32Time rtc;
    DisplayInfo display_info;

    BaseType_t xReturned_task_update; // 更新数据的异步任务
    TaskHandle_t xHandle_task_update; // 更新数据的异步任务
};
RuneTimeData *runtime_data = NULL;

enum UpdateType
{
    UPDATE_NTP,
};

static void rtc_update(ESP32Time time)
{
    if (time.getTime() == 0)
    {
        runtime_data->prev_net_time = runtime_data->prev_net_time + (GET_SYS_MILLIS() - runtime_data->prev_local_time);
        runtime_data->prev_local_time = GET_SYS_MILLIS();
        return;
    }

    runtime_data->rtc = time;
    runtime_data->display_info.hour = time.getHour(true);
    runtime_data->display_info.minute = time.getMinute();
    strcpy(runtime_data->display_info.city_name, city_names[runtime_data->city_idx]);
}

static ESP32Time timestamp_get(int city_idx)
{
    ESP32Time time;
    if (WL_CONNECTED != WiFi.status())
    {
        return time;
    }

    HTTPClient http;
    http.setTimeout(1000);
    String api = TIME_API + String(timezones[city_idx]);
    http.begin(api);

    int http_code = http.GET();
    if (HTTP_CODE_OK == http_code)
    {
        Serial.printf("[HTTP] GET WorldTime success, timezone: {0}\n", timezones[city_idx]);
        String payload = http.getString();
        DynamicJsonDocument doc(512);
        deserializeJson(doc, payload);

        int unixtime = doc["unixtime"];
        time.setTime(unixtime);

        return time;
    }
    else
    {
        Serial.printf("[HTTP] GET WorldTime failed, error: %s\n", http.errorToString(http_code).c_str());
        return time;
    }
}

static int jetlag_init(AppController *sys)
{
    tft->setSwapBytes(true);
    jetlag_gui_init();

    runtime_data = (RuneTimeData *)calloc(1, sizeof(RuneTimeData));
    memset((char *)&runtime_data->display_info, 0, sizeof(DisplayInfo));

    runtime_data->city_idx = 0x00;

    runtime_data->prev_local_time = GET_SYS_MILLIS();
    runtime_data->prev_net_time = GET_SYS_MILLIS();

    runtime_data->forced_update = 0x01;
    runtime_data->update_type = 0x00;

    return 0;
}

static void jetlag_process(AppController *sys, const ImuAction *act_info)
{
    lv_scr_load_anim_t anim_type = LV_SCR_LOAD_ANIM_NONE;

    if (RETURN == act_info->active)
    {
        sys->app_exit(); // 退出APP
        return;
    }

    if (GO_FORWORD == act_info->active)
    {
        runtime_data->forced_update = 0x01;
        delay(500);
    }
    else if (SHAKE == act_info->active)
    {
        anim_type = LV_SCR_LOAD_ANIM_FADE_IN;
        runtime_data->forced_update = 0x01;
        runtime_data->city_idx = (runtime_data->city_idx + 1) % CITY_SIZE;
        delay(500);
    }

    if (0x01 == runtime_data->forced_update || doDelayMillisTime(TIME_UPDATE_INTERVAL, &runtime_data->prev_time, false))
    {
        // 尝试同步网络时钟
        sys->send_to(JETLAG_NAME, CTRL_NAME, APP_MESSAGE_WIFI_CONN, (void *)UPDATE_NTP, NULL);
    }
    else if (GET_SYS_MILLIS() - runtime_data->prev_local_time > 400)
    {
        rtc_update(timestamp_get(runtime_data->city_idx));
        time_display(runtime_data->display_info, anim_type);
    }

    runtime_data->forced_update = 0x00;
    delay(30);
}

static void jetlag_background_task(AppController *sys, const ImuAction *act_info)
{
    // 本函数为后台任务，主控制器会间隔一分钟调用此函数
    // 本函数尽量只调用"常驻数据",其他变量可能会因为生命周期的缘故已经释放

    // 发送请求。如果是wifi相关的消息，当请求完成后自动会调用 example_message_handle 函数
    // sys->send_to(EXAMPLE_APP_NAME, CTRL_NAME,
    //              APP_MESSAGE_WIFI_CONN, (void *)run_data->val1, NULL);

    // 也可以移除自身的后台任务，放在本APP可控的地方最合适
    // sys->remove_backgroud_task();

    // 程序需要时可以适当加延时
    // delay(300);量只调用常驻数据
}

static int jetlag_exit_callback(void *param)
{
    jetlag_gui_release();

    // 查杀异步任务
    if (pdPASS == runtime_data->xReturned_task_update)
    {
        vTaskDelete(runtime_data->xHandle_task_update);
    }

    // 释放运行数据
    if (NULL != runtime_data)
    {
        free(runtime_data);
        runtime_data = NULL;
    }
    return 0;
}

static void jetlag_message_handle(const char *from, const char *to, APP_MESSAGE_TYPE msg_type, void *message, void *ext_info)
{
    switch (msg_type)
    {
        case APP_MESSAGE_WIFI_CONN:
        {
            Serial.println("wifi connected.\n");
            int evt_id = (int)message;
            switch (evt_id)
            {
                case UPDATE_NTP:
                Serial.println("update ntp.\n");
                rtc_update(timestamp_get(runtime_data->city_idx));
                time_display(runtime_data->display_info, LV_SCR_LOAD_ANIM_NONE);
                break;
            }
        }
        break;
    }
}

APP_OBJ jetlag_app = {
    JETLAG_NAME,
    NULL,
    "Author CY\nVersion 1.0.0\n",
    jetlag_init,
    jetlag_process,
    jetlag_background_task,
    jetlag_exit_callback,
    jetlag_message_handle};