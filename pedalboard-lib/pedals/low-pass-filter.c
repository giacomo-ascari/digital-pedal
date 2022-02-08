#include "../pedalboard.h"

//$ PEDAL LPF low_pass_filter_pedal_init low_pass_filter_process

//$ HEADER

void low_pass_filter_pedal_init(pedal_config_t *conf);

float low_pass_filter_process(float in, pedal_config_t *conf);

//$ HEADER

//$ SOURCE

void low_pass_filter_pedal_init(pedal_config_t *conf) {
    conf->u_int_params[WIDTH] = (u_int_parameter_t){4, 2, 32, 2};
    conf->float_params[BALANCE] = (float_parameter_t){1, 0, 1, 0.1};
}

float low_pass_filter_process(float in, pedal_config_t *conf) {
    static float history[441];
    static u_int16_t i = 0;
    u_int32_t width = conf->u_int_params[WIDTH].value;
    float out = 0;
    if (width > 441) width = 441;
    history[i % width] = in;
    for (u_int16_t j = i-width; j < i; j++) {
        out += history[j % width];
    }
    i++;
    out /= width;
    out = mix(out, in, conf->float_params[BALANCE].value);
    return out;
}

//$ SOURCE