#include "amplifier.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

// AMPLIFIER

void amplifier_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){0.5F, 0.1F, 10.F, 0.1F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F, 1.F};
}

float amplifier_process(float in, pedal_config_t *conf) {
    float out = in * conf->float_params[INTENSITY].value;
    out = hard_clip(out, conf->float_params[THRESHOLD_HIGH].value);
    return out;
}