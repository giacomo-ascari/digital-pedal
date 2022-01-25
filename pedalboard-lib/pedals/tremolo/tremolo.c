#include "tremolo.h"
#include "../../pedalboard.h"

// TREMOLO

void tremolo_pedal_init(pedal_config_t *conf) {
    conf->float_params[SPEED] = (float_parameter_t){32, 2, 64, 2};
    conf->float_params[BALANCE] = (float_parameter_t){0.25, 0, 1, 0.1};
}

float tremolo_process(float in, pedal_config_t *conf) {
    static u_int32_t i = 0;
    float out = in * wave_gen('s', i++, 1, conf->float_params[SPEED].value);
    out = mix(out, in, conf->float_params[BALANCE].value);
    return out;
}