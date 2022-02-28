#include "overdrive-sqrt.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

// OVERDRIVE_SQRT

void overdrive_sqrt_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){500.F, 0.F, 2000.F, 100.F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F, 1.F};
    conf->float_params[BALANCE_1] = (float_parameter_t){0.25F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.75F, 0.F, 1.F, 0.1F};
}

float overdrive_sqrt_process(float in, pedal_config_t *conf) {
    float out;
    if (in > 0.0F) {
        out = square_root(in);
    } else {
        out = -square_root(-in);
    }
    out = out * conf->float_params[INTENSITY].value;
    out = hard_clip(out, conf->float_params[THRESHOLD_HIGH].value);
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}