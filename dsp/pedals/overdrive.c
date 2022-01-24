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
float overdrive(float in, overdrive_pedal_t *p_pd) {
    float out = gain(in, p_pd->gain_intensity.value);
    out = soft_clip(out, p_pd->soft_threshold.value, p_pd->softener.value);
    out = hard_clip(out, p_pd->clip_threshold.value);
    out = mix(out, in, p_pd->balance.value);
    return out;
}