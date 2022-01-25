#include "bitcrusher-rs.h"
#include "../../pedalboard.h"

// BITCRUSHER resolution

void bitcrusher_rs_pedal_init(pedal_config_t *conf) {
    conf->float_params[REDUCT_INTENSITY] = (float_parameter_t){2000, 10, 2000, 10};
    conf->float_params[BALANCE] = (float_parameter_t){0.5, 0, 1, 0.1};
}

float bitcrusher_rs_process(float in, pedal_config_t *conf) {
    int16_t _out = (float)in / conf->float_params[REDUCT_INTENSITY].value;
    float out = (float)_out * conf->float_params[REDUCT_INTENSITY].value;
    out = mix(out, in, conf->float_params[BALANCE].value);
    return out;
}