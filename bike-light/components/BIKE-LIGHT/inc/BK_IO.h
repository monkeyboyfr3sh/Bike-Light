#ifndef _BK_IO_H_
#define _BK_IO_H_

#include <stdint.h>
#include <stdbool.h>

#define LEFT_SIG_PIN    GPIO_NUM_22
#define RIGHT_SIG_PIN   GPIO_NUM_23
#define HORN_SIG_PIN    GPIO_NUM_21

typedef enum {
    left_turn_t,
    none_turn_t,
    right_turn_t
} turn_signal_t;

void init_bk_io(void);
turn_signal_t turn_signal_state(void);
bool horn_state(void);

#endif