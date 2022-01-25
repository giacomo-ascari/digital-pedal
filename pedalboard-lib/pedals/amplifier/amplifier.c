#include "amplifier.h"
#include "../../pedalboard.h"

// AMPLIFIER

void amplifier_pedal_init(pedal_config_t *conf) {
    conf->float_params[GAIN_INTENSITY] = (float_parameter_t){0.5, 0.1, 10, 0.1};
    conf->float_params[CLIP_THRESHOLD] = (float_parameter_t){32767, 0, 32767, 1};
}

float amplifier_process(float in, pedal_config_t *conf) {
    float out = gain(in, conf->float_params[GAIN_INTENSITY].value);
    out = hard_clip(out, conf->float_params[CLIP_THRESHOLD].value);
    return out;
}