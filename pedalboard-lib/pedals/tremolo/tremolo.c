#include "tremolo.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

// TREMOLO

void tremolo_pedal_init(pedal_config_t *conf) {
    conf->u_int_params[COUNTER] = (u_int_parameter_t){0, 1, 0, 0};
    conf->float_params[SPEED] = (float_parameter_t){32, 2, 64, 2};
    conf->float_params[BALANCE_1] = (float_parameter_t){0.1, 0, 1, 0.1};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.9, 0, 1, 0.1};
}

float tremolo_process(float in, pedal_config_t *conf) {
    static u_int32_t i = 0;
    float out = in * wave_gen('s', conf->u_int_params[COUNTER].value, 1, conf->float_params[SPEED].value);
    conf->u_int_params[COUNTER].value++;
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}