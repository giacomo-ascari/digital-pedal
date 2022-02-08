#include "dyn-amplifier.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

// DYN_AMPLIFIER

void dyn_amplifier_pedal_init(pedal_config_t *conf) {
    //conf->float_params[GAIN_INTENSITY] = (float_parameter_t){1000, 0.1, 10, 0.1};
    //conf->float_params[CLIP_THRESHOLD] = (float_parameter_t){32767, 0, 32767, 1};
    //conf->float_params[SOFT_THRESHOLD] = (float_parameter_t){28000, 0, 32767, 1};
    //conf->float_params[BALANCE] = (float_parameter_t){0.5, 0, 1, 0.1};
}

float dyn_amplifier_process(float in, pedal_config_t *conf) {
    //static float dyn = 1.0f, max = 0;;
    //static u_int16_t i = 0;
    //float out = gain(in, dyn);
    //if (out > max) max = out;
    //i++;
    //if (!(i%4800)) {
    //    if (max > conf->float_params[SOFT_THRESHOLD].value) dyn -= 0.01;
    //    else if (max < conf->float_params[SOFT_THRESHOLD].value) dyn += 0.01;
    //    max = 0;
    //}
    //out = hard_clip(out, conf->float_params[CLIP_THRESHOLD].value);
    //return out;
    return in;
}