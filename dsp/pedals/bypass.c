#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../dsp.h"

// BYPASS

bypass_pedal_t *bypass_pedal_init() {
    bypass_pedal_t *p_pd = malloc(sizeof(bypass_pedal_t));
    return p_pd;
}
void bypass_pedal_destroy(bypass_pedal_t *p_pd) {
    free(p_pd);
}
float bypass(float in, bypass_pedal_t *p_pd) {
    return in;
}