#ifndef _DSP_H
#define _DSP_H

#include <stdio.h>
#include <stdlib.h>

float gain(float in, float gain_intensity);
float mix(float in_1, float in_2, float balance);
float hard_clip(float in, float clip_threshold);
float soft_clip(float in, float clip_threshold, float soft_threshold, float softener);
float overlap_last_frequency(float in, float height, float speed);
float reduce_resolution(float in, float reduction_intensity);
float reduce_rate(float in, float reduction_intensity);
float multiply_by_lfo(float in, float speed);
float low_pass_filter(int16_t *in, int i, int width);

#endif