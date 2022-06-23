#include "Frame_Thread.h"

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


#include "utils.h"

#include "stdbool.h"

static const char *TAG = "frame";

bool led_on = true;
int wr_led_peak = 0;
int show_led_peak = 0;
uint32_t wr_led_peak_timestamp = 0;

bool wr_num_led = false;
int wr_num_led_cnt = 0;

led_strip_t *update_num_led(uint32_t num_led);

led_strip_t *update_num_led(uint32_t num_led)
{
    ESP_LOGI(TAG,"Updating NUM led to %d",num_led);
    rmt_config_t config = RMT_DEFAULT_CONFIG_TX(GPIO_NUM_16, RMT_TX_CHANNEL);
    // set counter clock to 40MHz
    config.clk_div = 2;

    ESP_ERROR_CHECK(rmt_config(&config));
    ESP_ERROR_CHECK(rmt_driver_install(config.channel, 0, 0));

    // install ws2812 driver
    led_strip_config_t strip_config = LED_STRIP_DEFAULT_CONFIG(num_led, (led_strip_dev_t)config.channel);
    led_strip_t *strip = led_strip_new_rmt_ws2812(&strip_config);
    if (!strip) {
        ESP_LOGE(TAG, "install WS2812 driver failed");
    }

    // Clear LED strip (turn off all LEDs)
    ESP_ERROR_CHECK(strip->clear(strip, 100));
    ESP_ERROR_CHECK(strip->refresh(strip, 0));

    return strip;
}

void generate_frame_buff(int num_row, int num_col, Frame* frame){
    ESP_LOGI(TAG,"Allocating map");

    uint32_t start_timestamp = xTaskGetTickCount();

    // Update panel info
    frame->num_row = num_row;
    frame->num_col = num_col;
    frame->strip = update_num_led(num_row*num_col);
   
    // Allocate index map and frame buff 
    frame->index_map = malloc(num_row*sizeof(int));
    frame->frame_buff = malloc(num_row*sizeof(int));
    bool dir = true;

    for(int i = 0;i<num_row;i++){
        frame->index_map[i] = malloc(num_col*sizeof(int));
        for(int j = 0;j<num_col;j++){
            if(dir){
               frame->index_map[i][j] = (i*num_col) + j;
            } else {
                frame->index_map[i][j] = (i*num_col) + (num_col-j-1);
            }
            // ESP_LOGI(TAG,"index_map[%d][%d] = %d",i,j,frame->index_map[i][j]);
        }
        dir = !dir;
    }
    uint32_t stop_timestamp = xTaskGetTickCount();
 
    ESP_LOGI(TAG,"Allocated index map of size %d bytes", (num_row*num_col*4));
    ESP_LOGI(TAG,"Allocated frame buff of size %d bytes", (num_row*num_col*4*3));
    ESP_LOGI(TAG,"-------------------------------------");
    ESP_LOGI(TAG,"Allocation completed in %dms.", pdMS_TO_TICKS(stop_timestamp-start_timestamp));
}

void print_index_map(Frame *frame)
{
    for(int j = frame->num_col-1;j>=0;j--){
        char line_print[1024];
        uint32_t char_count = 0;
        int index;
 
        for(int i = frame->num_row-1;i>=0;i--){
            index = coord_to_index(i, j, frame->index_map);
            char_count += sprintf(line_print+char_count, "%03d ",index);
        }
 
        ESP_LOGI(TAG,"%s",line_print);
    }
}

int coord_to_index(int x, int y, int **index_map){
    return (index_map[x][y]);  
}
 
void color_set_row(int row_sel, uint32_t red, uint32_t green, uint32_t blue, Frame* frame){
    for(int i = 0;i<frame->num_col;i++){
        ESP_ERROR_CHECK(frame->strip->set_pixel(frame->strip, coord_to_index(row_sel,i,frame->index_map), red, green, blue));
    }
}

void frame_buff_task(void *pvParameters)
{
    uint32_t red = 0;
    uint32_t green = 0;
    uint32_t blue = 0;
    uint16_t hue = 0;
    uint16_t start_rgb = 0;

    ESP_LOGI(TAG, "Thread %s LED Rainbow Chase Start",(char *)pvParameters);

    // Show simple rainbow chasing pattern
    uint32_t print_timestamp = xTaskGetTickCount();
    uint32_t frame_timestamp = xTaskGetTickCount();
    uint32_t hue_timestamp = xTaskGetTickCount();
    uint32_t pix_timestamp = xTaskGetTickCount();
    
    const int box_width = 4;
    int front_num_pix = box_width;
    int back_num_pix = 0;
    int offset = 1;

    Frame frame_buff;
    generate_frame_buff(NUM_ROW,NUM_COL,&frame_buff); 
    // print_index_map(&frame_buff);

    while(1){

        // Update frame
        if((xTaskGetTickCount() - frame_timestamp) > pdMS_TO_TICKS(FRAME_TIME_MS-9)){
            frame_timestamp = xTaskGetTickCount();
            // for (int i = 0; i < 1; i++) {
            //     for (int j = i; j < CONFIG_EXAMPLE_STRIP_LED_NUMBER; j += 1) {

            //         // Write RGB values to strip driver
            //         if(j<num_pix){
            //             ESP_ERROR_CHECK((frame_buff.strip)->set_pixel((frame_buff.strip), j, (int)(red/DIV_FACTOR), (int)(green/DIV_FACTOR), (int)(blue/DIV_FACTOR)));
            //         } else {
            //             ESP_ERROR_CHECK((frame_buff.strip)->set_pixel((frame_buff.strip), j, 0, 0, 0));
            //         }
            //     }
            //     // Flush RGB values to LEDs
            //     ESP_ERROR_CHECK((frame_buff.strip)->refresh((frame_buff.strip), 0));
            // }
 
            for (int i = 0; i < NUM_ROW; i++) {            
                for (int j = 0; j < NUM_COL; j++) {                
            
                    // Build RGB values
                    // hue = 360 + start_rgb + (i*NUM_COL)+j;
                    hue = start_rgb + ((i*NUM_COL)+j)*4;
                    led_strip_hsv2rgb(hue, 100, 100, &red, &green, &blue);

                    // Write RGB values to strip driver
                    int index = coord_to_index(i, j, frame_buff.index_map);
                    // ESP_LOGI(TAG,"map[%d][%d] = %d",i,j,index);
                    if((back_num_pix<=i)&&(i<front_num_pix)){
                        ESP_ERROR_CHECK((frame_buff.strip)->set_pixel(frame_buff.strip, index, (int)(red/DIV_FACTOR), (int)(green/DIV_FACTOR), (int)(blue/DIV_FACTOR)));
                    } else {
                        ESP_ERROR_CHECK((frame_buff.strip)->set_pixel(frame_buff.strip, index, 0, 0, 0));
                    }
                }
            }
            // Flush RGB values to LEDs
            ESP_ERROR_CHECK((frame_buff.strip)->refresh((frame_buff.strip), 0));
        }

        // Update hue
        if((xTaskGetTickCount() - hue_timestamp) > pdMS_TO_TICKS(HUE_TIME_MS)){
            hue_timestamp = xTaskGetTickCount();
            start_rgb += RGB_OFFSET*(-offset);
        }

        // Update pixel location
        if((xTaskGetTickCount() - pix_timestamp) > pdMS_TO_TICKS(PIX_TIME_MS)){
            pix_timestamp = xTaskGetTickCount();
            front_num_pix += offset;
            back_num_pix += offset;

            if(front_num_pix>=NUM_ROW){
                offset = -1;
                
                front_num_pix = NUM_ROW;
                back_num_pix = NUM_ROW-box_width;
            }

            if(front_num_pix-box_width <= 0){
                offset = 1;
                front_num_pix = box_width;
                back_num_pix = 0;
            }
        }

        vTaskDelay(1);
    }
}
