#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../dsp.h"

// BITCRUSHER resolution

bitcrusher_rs_pedal_t *bitcrusher_rs_pedal_init() {
    bitcrusher_rs_pedal_t *p_pd = malloc(sizeof(bitcrusher_rs_pedal_t));
    p_pd->reduction_intensity = (float_parameter_t){2000, 10, 2000, 10};
    p_pd->balance = (float_parameter_t){0.5, 0, 1, 0.1};
    return p_pd;
}
void bitcrusher_rs_pedal_destroy(bitcrusher_rs_pedal_t *p_pd) {
    free(p_pd);
}
float bitcrusher_rs(float in, bitcrusher_rs_pedal_t *p_pd) {
    int16_t _out = (float)in / p_pd->reduction_intensity.value;
    float out = (float)_out * p_pd->reduction_intensity.value;
    out = mix(out, in, p_pd->balance.value);
    return out;
}