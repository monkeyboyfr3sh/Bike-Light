#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "driver/gpio.h"

#include "driver/rmt.h"
#include "led_strip.h"

#include "stdbool.h"

#include "Frame_Thread.h"
#include "OTA_Thread.h"
#include "BK_IO.h"

static const char *TAG = "bk-lite";

void app_main(void)
{
    ESP_LOGI(TAG,"************** Main thread enter **************");
    
    ESP_LOGI(TAG,"Forking led task");
    xTaskCreate(frame_buff_task, "led_task", 8192, (void*)"lt_1", 5, NULL);
    // xTaskCreate(OTA_task, "ota_task", 8192, (void*)"ota_1", 5, NULL);
    
    init_bk_io();
    turn_signal_t prev_signal_state = none_turn_t;
    turn_signal_t signal_state = none_turn_t;
    
    bool prev_horn = false;
    bool horn = false;
    while(1)
    {
        prev_signal_state = signal_state; 
        signal_state = turn_signal_state();
        if(signal_state != prev_signal_state)
        {
            ESP_LOGI(TAG,"Turn signal: %d",signal_state);
        }

        prev_horn = horn;
        horn = horn_state();
        if(horn != prev_horn)
        {
            ESP_LOGI(TAG,"Horn state: %d",horn);
        }

        vTaskDelay(1);
    }

    ESP_LOGI(TAG,"************** Main thread exit ***************");
}
