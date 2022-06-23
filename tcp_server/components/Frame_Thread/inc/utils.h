#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>

int decay_pos(int pos_set, int decay_rate_ms, int decay_coef);
int proportional_pos(int prev_pos,float proportional_coef);
int integral_pos(int prev_pos,float integral_coef);
int derivative_pos(int wr_pos, float derivative_coef);
void led_strip_hsv2rgb(uint32_t h, uint32_t s, uint32_t v, uint32_t *r, uint32_t *g, uint32_t *b);

#endif