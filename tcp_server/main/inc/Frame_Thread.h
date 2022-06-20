#ifndef _FRAME_THREAD_H_
#define _FRAME_THREAD_H_

#include "led_strip.h"

#define NUM_ROW     32
#define NUM_COL     8

#define RMT_TX_CHANNEL RMT_CHANNEL_0
#define CONFIG_EXAMPLE_STRIP_LED_NUMBER (256)

#define FRAME_TIME_MS                   (20)
#define HUE_TIME_MS                     (25)
#define PIX_TIME_MS                     (50)
#define RGB_OFFSET                      (30)

#define DIV_FACTOR                      (2)

typedef struct Frame{
    int num_row;
    int num_col;
    led_strip_t *strip;
    int** index_map;
    int*** frame_buff;
}Frame;

void generate_frame_buff(int num_row, int num_col, Frame* frame);
void print_index_map(Frame *led_panel);
int coord_to_index(int x, int y, int **index_map);
void frame_buff_task(void *pvParameters);

#endif