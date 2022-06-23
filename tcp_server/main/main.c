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

static const char *TAG = "bk-lite";

void app_main(void)
{
    ESP_LOGI(TAG,"************** Main thread enter **************");
    
    ESP_LOGI(TAG,"Forking led task");
    xTaskCreate(frame_buff_task, "led_task", 8192, (void*)"lt_1", 5, NULL);
    xTaskCreate(OTA_task, "ota_task", 8192, (void*)"ota_1", 5, NULL);
    
    ESP_LOGI(TAG,"************** Main thread exit ***************");
}
