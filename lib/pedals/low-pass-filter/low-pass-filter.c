#include "low-pass-filter.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

// LPF

void low_pass_filter_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){0.9F, 0.0F, 1.0F, 0.01F};
    conf->float_params[BALANCE_1] = (float_parameter_t){1.F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.F, 0.F, 1.F, 0.1F};
    conf->float_params[PAST] = (float_parameter_t){0.F, 0.F, 0.F, 0.F};
}

float low_pass_filter_process(float in, pedal_config_t *conf) {
    float alpha = conf->float_params[INTENSITY].value;
    float out = conf->float_params[PAST].value * alpha + (1.F - alpha) * in;
    conf->float_params[PAST].value = out;
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}