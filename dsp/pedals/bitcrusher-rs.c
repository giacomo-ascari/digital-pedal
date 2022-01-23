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
float bitcrusher_rs(float in, float reduction_intensity, float balance) {
    int16_t _out = (float)in / reduction_intensity;
    float out = (float)_out * reduction_intensity;
    out = mix(out, in, balance);
    return out;
}