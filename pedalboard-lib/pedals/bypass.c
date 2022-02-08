#include "../pedalboard.h"

//$ PEDAL BYPASS bypass_pedal_init bypass_process

//$ HEADER

void bypass_pedal_init(pedal_config_t *conf);

float bypass_process(float in, pedal_config_t *conf);

//$ HEADER

//$ SOURCE

void bypass_pedal_init(pedal_config_t *conf) {
    return;
}

float bypass_process(float in, pedal_config_t *conf) {
    return in;
}

//$ SOURCE