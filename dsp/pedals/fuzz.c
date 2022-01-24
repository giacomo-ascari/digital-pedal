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
float fuzz(float in, fuzz_pedal_t *p_pd) {
    static u_int32_t i = 0;
    float out = gain(in, p_pd->gain_intensity.value);
    out = hard_clip(out, p_pd->clip_threshold.value);
    if (out == p_pd->clip_threshold.value) {
        out = out + wave_gen('s', i++, p_pd->height.value, p_pd->speed.value) - p_pd->height.value;
    } else if (out == -p_pd->clip_threshold.value-1) {
        out = out - wave_gen('s', i++, p_pd->height.value, p_pd->speed.value) + p_pd->height.value;
    }
    out = mix(out, in, p_pd->balance.value);
    return out;
}