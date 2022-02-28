#ifndef _DSP_H
#define _DSP_H

#include <stdlib.h>

float mix(float in_1, float in_2, float balance_1, float balance_2);

float hard_clip(float in, float clip_threshold);

float soft_clip(float in, float soft_threshold, float softener);

float square_root(float in);

float wave_gen(char t, u_int32_t i, u_int32_t tone);

#endif