#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../dsp.h"

// TREMOLO

tremolo_pedal_t *tremolo_pedal_init() {
    tremolo_pedal_t *p_pd = malloc(sizeof(tremolo_pedal_t));
    p_pd->speed = (float_parameter_t){50, 2, 64, 2};
    p_pd->balance = (float_parameter_t){0.5, 0, 1, 0.1};
    return p_pd;
}
void tremolo_pedal_destroy(tremolo_pedal_t *p_pd) {
    free(p_pd);
}
float tremolo(float in, tremolo_pedal_t *p_pd) {
    static u_int32_t i = 0;
    float out = in * wave_gen('s', i++, 1, p_pd->speed.value);
    out = mix(out, in, p_pd->balance.value);
    return out;
}