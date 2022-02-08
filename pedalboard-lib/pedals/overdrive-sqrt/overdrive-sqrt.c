#include "overdrive-sqrt.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

#include <math.h>

// OVERDRIVE_LOG

void overdrive_sqrt_pedal_init(pedal_config_t *conf) {
    conf->float_params[GAIN_INTENSITY] = (float_parameter_t){1000, 500, 2000, 100};
    conf->float_params[CLIP_THRESHOLD] = (float_parameter_t){32767, 0, 32767, 1};
    conf->float_params[BALANCE_1] = (float_parameter_t){0.2, 0, 1, 0.1};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.8, 0, 1, 0.1};
}

float overdrive_sqrt_process(float in, pedal_config_t *conf) {
    float out;
    if (in > 1) out = log2f(in);
    else if (in < -1) out = -log2f(-in);
    else out = 0;
    out = gain(out, conf->float_params[GAIN_INTENSITY].value);
    out = hard_clip(out, conf->float_params[CLIP_THRESHOLD].value);
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}