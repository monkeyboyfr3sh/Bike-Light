#include "utils.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

int decay_pos(int pos_set, int decay_rate_ms, int decay_coef)
{
    static uint32_t timestamp = 0;
    static int pos;

    pos = pos_set; 
    if(pos){
        if( (xTaskGetTickCount()-timestamp) > pdMS_TO_TICKS(decay_rate_ms) ){
            timestamp = xTaskGetTickCount();
            pos -= (pos > decay_coef) ? decay_coef : pos;
        }
    }

    return pos;
}

int proportional_pos(int prev_pos,float proportional_coef)
{
    return (proportional_coef * ( (float)prev_pos) ); 
}

int integral_pos(int prev_pos,float integral_coef)
{
    return (integral_coef * ( (float)prev_pos) ); 
}

int derivative_pos(int error,float derivative_coef)
{
    return (derivative_coef * ( (float)error) ); 
}

void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b)
{
    h %= 360; // h -> [0,360]
    uint32_t rgb_max = v * 0.08f;
    uint32_t rgb_min = rgb_max * (100 - s) / 100.0f;

    uint32_t i = h / 60;
    uint32_t diff = h % 60;

    // RGB adjustment amount by hue
    uint32_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i) {
    case 0:
        *r = rgb_max;
        *g = rgb_min + rgb_adj;
        *b = rgb_min;
        break;
    case 1:
        *r = rgb_max - rgb_adj;
        *g = rgb_max;
        *b = rgb_min;
        break;
    case 2:
        *r = rgb_min;
        *g = rgb_max;
        *b = rgb_min + rgb_adj;
        break;
    case 3:
        *r = rgb_min;
        *g = rgb_max - rgb_adj;
        *b = rgb_max;
        break;
    case 4:
        *r = rgb_min + rgb_adj;
        *g = rgb_min;
        *b = rgb_max;
        break;
    default:
        *r = rgb_max;
        *g = rgb_min;
        *b = rgb_max - rgb_adj;
        break;
    }
}