#include "bitcrusher-rs.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

// BITCRUSHER resolution

void bitcrusher_rs_pedal_init(pedal_config_t *conf) {
    conf->int_params[REDUCT_INTENSITY] = (int_parameter_t){12, 1, 16, 1};
    conf->float_params[BALANCE_1] = (float_parameter_t){0.5F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.5F, 0.F, 1.F, 0.1F};
}

float bitcrusher_rs_process(float in, pedal_config_t *conf) {
    int16_t shift = conf->int_params[REDUCT_INTENSITY].value;
    int16_t _out = (int16_t)in;
    _out = _out >> shift;
    _out = _out << shift;
    _out += (2 << shift -2);
    float out = (float)_out;
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}