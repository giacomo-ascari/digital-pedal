#include "overdrive.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

// OVERDRIVE

void overdrive_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){4.F, 1.F, 10.F, 0.5F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F, 1.F};
    conf->float_params[THRESHOLD_LOW] = (float_parameter_t){28000.F, 0.F, 32767.F, 1.F};
    conf->float_params[SOFTENER] = (float_parameter_t){4.F, 1.F, 10.F, 0.5F};
    conf->float_params[BALANCE_1] = (float_parameter_t){1.F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.F, 0.F, 1.F, 0.1F};
}

float overdrive_process(float in, pedal_config_t *conf) {
    float out = in *  conf->float_params[INTENSITY].value;
    out = soft_clip(out, conf->float_params[THRESHOLD_LOW].value, conf->float_params[SOFTENER].value);
    out = hard_clip(out, conf->float_params[THRESHOLD_HIGH].value);
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}