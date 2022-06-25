#include "BK_IO.h"

#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "stdbool.h"

static const char *TAG = "bk_io";

void init_bk_io(void)
{
    ESP_LOGI(TAG,"Initializing IO");
    gpio_config_t gpio_conf;
    gpio_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_conf.mode = GPIO_MODE_INPUT;
    gpio_conf.pull_down_en = GPIO_PULLDOWN_ENABLE;
    gpio_conf.pull_up_en = GPIO_PULLUP_DISABLE;

    gpio_conf.pin_bit_mask = (1ULL << LEFT_SIG_PIN);
    gpio_config(&gpio_conf);

    gpio_conf.pin_bit_mask = (1ULL << RIGHT_SIG_PIN);
    gpio_config(&gpio_conf);

    gpio_conf.pin_bit_mask = (1ULL << HORN_SIG_PIN);
    gpio_config(&gpio_conf);
}

turn_signal_t turn_signal_state(void)
{
    if(gpio_get_level(LEFT_SIG_PIN)){ return left_turn_t; }
    else if(gpio_get_level(RIGHT_SIG_PIN)){ return right_turn_t; } 
    else{ return none_turn_t; } 
}

bool horn_state(void)
{
    return gpio_get_level(HORN_SIG_PIN);
}