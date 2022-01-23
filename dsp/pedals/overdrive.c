#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../dsp.h"

// OVERDRIVE

overdrive_pedal_t *overdrive_pedal_init() {
    overdrive_pedal_t *p_pd = malloc(sizeof(overdrive_pedal_t));
    p_pd->gain_intensity = (float_parameter_t){4, 1, 10, 0.5};
    p_pd->clip_threshold = (float_parameter_t){32767, 0, 32767, 1};
    p_pd->soft_threshold = (float_parameter_t){28000, 0, 32767, 1};
    p_pd->softener = (float_parameter_t){4, 1, 10, 0.5};
    p_pd->balance = (float_parameter_t){1, 0, 1, 0.1};
    return p_pd;
}
void overdrive_pedal_destroy(overdrive_pedal_t *p_pd) {
    free(p_pd);
}
float overdrive(float in, float gain_intensity, float clip_threshold, float soft_threshold, float softener, float balance) {
    float out = gain(in, gain_intensity);
    out = soft_clip(out, soft_threshold, softener);
    out = hard_clip(out, clip_threshold);
    out = mix(out, in, balance);
    return out;
}