#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../dsp.h"

// FUZZ

fuzz_pedal_t *fuzz_pedal_init() {
    fuzz_pedal_t *p_pd = malloc(sizeof(fuzz_pedal_t));
    p_pd->gain_intensity = (float_parameter_t){4, 1, 10, 0.5};
    p_pd->clip_threshold = (float_parameter_t){32767, 0, 32767, 1};
    p_pd->height = (float_parameter_t){2048, 0, 8192, 32};
    p_pd->speed = (float_parameter_t){20, 2, 64, 2};
    p_pd->balance = (float_parameter_t){1, 0, 1, 0.1};
    return p_pd;
}
void fuzz_pedal_destroy(fuzz_pedal_t *p_pd) {
    free(p_pd);
}
float fuzz(float in, float gain_intensity, float clip_threshold, float height, float speed, float balance) {
    static u_int32_t i = 0;
    float out = gain(in, gain_intensity);
    out = hard_clip(out, clip_threshold);
    if (out == clip_threshold) {
        out = out + wave_gen('s', i++, height, speed) - height;
    } else if (out == -clip_threshold-1) {
        out = out - wave_gen('s', i++, height, speed) + height;
    }
    out = mix(out, in, balance);
    return out;
}