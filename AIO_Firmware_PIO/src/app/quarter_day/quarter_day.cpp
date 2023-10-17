#include "ArduinoJson.h"
#include "ESP32Time.h"
#include "network.h"
#include "sys/app_controller.h"
#include "quarter_day_gui.h"

#define QUARTER_DAY_NAME "Quarter Day"
#define TIME_API "http://worldtimeapi.org/api/timezone/"

static String timezones[] = {"Asia/Shanghai", "Europe/Stockholm"};

struct RuneTimeData
{
    long long prev_local_time;
    long long prev_net_time;
    ESP32Time rtc;
};
static RuneTimeData *runtime_data;

static int quarter_day_init(AppController *sys)
{
    return 0;
}

static void quarter_day_process(AppController *sys, const ImuAction *act_info)
{
    if (RETURN == act_info->active)
    {
        sys->app_exit(); // 退出APP
        return;
    }
}

static void quarter_day_background_task(AppController *sys, const ImuAction *act_info)
{
}

static int quarter_day_exit_callback(void *param)
{
    if (NULL != runtime_data)
    {
        free(runtime_data);
        runtime_data = NULL;
    }
    return 0;
}

static void quarter_day_message_handle(const char *from, const char *to, APP_MESSAGE_TYPE type, void *message, void *ext_info)
{
    switch (type)
    {
    case APP_MESSAGE_WIFI_CONN:
    {
        Serial.println("wifi connected");
        int evt_id = (int)message;
        switch (evt_id)
        {
        }
    }
    break;
    case APP_MESSAGE_WIFI_DISCONN:
    {
        // todo
    }
    break;
    case APP_MESSAGE_WIFI_AP:
    {
        // todo
    }
    break;
    case APP_MESSAGE_WIFI_ALIVE:
    {
        // todo
    }
    break;
    case APP_MESSAGE_UPDATE_TIME:
    {
        // todo
    }
    break;
    case APP_MESSAGE_MQTT_DATA:
    {
        // todo
    }
    break;
    case APP_MESSAGE_GET_PARAM:
    {
        // todo
    }
    break;
    case APP_MESSAGE_SET_PARAM:
    {
        // todo
    }
    break;
    case APP_MESSAGE_READ_CFG:
    {
        // todo
    }
    break;
    case APP_MESSAGE_WRITE_CFG:
    {
        // todo
    }
    break;
    default:
        break;
    }
}

static void rtc_update(ESP32Time time)
{
    if (time.getTime() == 0)
    {
        runtime_data->prev_net_time = runtime_data->prev_net_time + (GET_SYS_MILLIS() - runtime_data->prev_local_time);
        runtime_data->prev_local_time = GET_SYS_MILLIS();
        return;
    }

    struct DisplayInfo display_info;
    runtime_data->rtc = time;
    display_info.hour = time.getHour(true);
    display_info.city_idx = 0;
}

static ESP32Time timestamp_get(String timezone)
{
    ESP32Time time;
    if (WL_CONNECTED != WiFi.status())
    {
        return time;
    }

    HTTPClient http;
    http.setTimeout(1000);
    http.begin(TIME_API + timezone);

    int http_code = http.GET();
    if (HTTP_CODE_OK == http_code)
    {
        Serial.printf("[HTTP] GET WorldTime success, timezone: {0}\n", timezone);
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

APP_OBJ quarter_day_app = {
    QUARTER_DAY_NAME,
    NULL,
    "Author CY\nVersion 1.0.0\n",
    quarter_day_init,
    quarter_day_process,
    quarter_day_background_task,
    quarter_day_exit_callback,
    quarter_day_message_handle};