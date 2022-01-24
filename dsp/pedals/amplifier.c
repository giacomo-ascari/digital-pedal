#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../dsp.h"

// AMPLIFIER

amplifier_pedal_t *amplifier_pedal_init() {
    amplifier_pedal_t *p_pd = malloc(sizeof(amplifier_pedal_t));
    p_pd->gain_intensity = (float_parameter_t){1, 0.1, 10, 0.1};
    p_pd->clip_threshold = (float_parameter_t){32767, 0, 32767, 1};
    return p_pd;
}
void amplifier_pedal_destroy(amplifier_pedal_t *p_pd) {
    free(p_pd);
}
float amplifier(float in, amplifier_pedal_t *p_pd) {
    float out = gain(in, p_pd->gain_intensity.value);
    out = hard_clip(out, p_pd->clip_threshold.value);
    return out;
}