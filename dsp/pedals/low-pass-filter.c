#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../dsp.h"

// LOW PASS FILTER

low_pass_filter_pedal_t *low_pass_filter_pedal_init() {
    low_pass_filter_pedal_t *p_pd = malloc(sizeof(low_pass_filter_pedal_t));
    p_pd->width = (u_int_parameter_t){4, 2, 32, 2};
    p_pd->balance = (float_parameter_t){1, 0, 1, 0.1};
    return p_pd;
}
void low_pass_filter_pedal_destroy(low_pass_filter_pedal_t *p_pd) {
    free(p_pd);
}
float low_pass_filter(float in, u_int32_t width, float balance) {
    static float history[441];
    static u_int16_t i = 0;
    float out = 0;
    if (width > 441) width = 441;
    history[i % width] = in;
    for (u_int16_t j = i-width; j < i; j++) {
        out += history[j % width];
    }
    i++;
    out /= width;
    out = mix(out, in, balance);
    return out;
}