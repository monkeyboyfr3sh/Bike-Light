#ifndef _OTA_THREAD_H_
#define _OTA_THREAD_H_S

#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#define OTA_DATA_QUEUE_SIZE             256

extern SemaphoreHandle_t ota_barrier;
extern QueueHandle_t ota_data_queue;

typedef struct ota_data_t{
    uint32_t size;
    uint8_t ota_data[128];
    uint32_t packet_cnt;
}ota_data_t;

void OTA_task(void *pvParameter);

#endif