#include "low-pass-filter.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

// LPF

void low_pass_filter_pedal_init(pedal_config_t *conf) {
    conf->u_int_params[WIDTH] = (u_int_parameter_t){40, 2, 32, 2};
    conf->u_int_params[COUNTER] = (u_int_parameter_t){0, 1, 0, 0};
    conf->float_params[BALANCE_1] = (float_parameter_t){1, 0, 1, 0.1};
    conf->float_params[BALANCE_2] = (float_parameter_t){0, 0, 1, 0.1};
}

float low_pass_filter_process(float in, pedal_config_t *conf) {
    static float history[441];
    u_int32_t width = conf->u_int_params[WIDTH].value;
    u_int32_t i = conf->u_int_params[COUNTER].value;
    float out = 0;
    if (width > 441) width = 441;
    history[i % width] = in;
    for (u_int16_t j = i-width; j < i; j++) {
        out += history[j % width];
    }
    conf->u_int_params[COUNTER].value++;
    out /= width;
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}