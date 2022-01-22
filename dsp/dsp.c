#include <stdlib.h>
#include <stdio.h>

#include "dsp.h"

float gain(float in, float gain_intensity) {
    return in * gain_intensity;
}

float mix(float in_1, float in_2, float balance) {
    return in_1 * balance + in_2 * (1. - balance);
}

float hard_clip(float in, float clip_threshold) {
    if (in > clip_threshold) {
        return clip_threshold;
    } else if (in < -clip_threshold) {
        return -clip_threshold - 1;
    }
    return in;
}

float soft_clip(float in, float clip_threshold, float soft_threshold, float softener) {
    float out;
    if (in > soft_threshold) {
        out = soft_threshold + (in - soft_threshold) / softener;
    } else if (in < -soft_threshold) {
        out = -soft_threshold + (in + soft_threshold) / softener;
    } else {
        out = in;
    }
    return hard_clip(out, clip_threshold);
}
