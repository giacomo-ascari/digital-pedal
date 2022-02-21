#include "tremolo.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

// TREMOLO

void tremolo_pedal_init(pedal_config_t *conf) {
    conf->int_params[COUNTER] = (int_parameter_t){0, 1, 0, 0};
    conf->float_params[SPEED] = (float_parameter_t){32.F, 2.F, 64.F, 2.F};
    conf->float_params[BALANCE_1] = (float_parameter_t){0.1F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.9F, 0.F, 1.F, 0.1F};
}

float tremolo_process(float in, pedal_config_t *conf) {
    float out = in * wave_gen('s', conf->int_params[COUNTER].value, 1.F, conf->float_params[SPEED].value);
    conf->int_params[COUNTER].value++;
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}