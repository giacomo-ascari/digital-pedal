#include "bitcrusher-rs.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

// BITCRUSHER resolution

void bitcrusher_rs_pedal_init(pedal_config_t *conf) {
    conf->u_int_params[REDUCT_INTENSITY] = (u_int_parameter_t){8, 1, 10, 1};
    conf->float_params[BALANCE_1] = (float_parameter_t){0.5, 0, 1, 0.1};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.5, 0, 1, 0.1};
}

float bitcrusher_rs_process(float in, pedal_config_t *conf) {
    int16_t divider = pow(2, conf->u_int_params[REDUCT_INTENSITY].value);
    int16_t _out = (int16_t)in;
    _out /= divider;
    _out *= divider;
    float out = (float)_out;
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}