#include "fuzz.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

// FUZZ

void fuzz_pedal_init(pedal_config_t *conf) {
    conf->int_params[COUNTER] = (int_parameter_t){0, 1, 0, 0};
    conf->float_params[INTENSITY] = (float_parameter_t){4.F, 1.F, 10.F, 0.5F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F, 1.F};
    conf->float_params[HEIGHT] = (float_parameter_t){2048.F, 0.F, 8192.F, 32.F};
    conf->float_params[SPEED] = (float_parameter_t){20.F, 2.F, 64.F, 2.F};
    conf->float_params[BALANCE_1] = (float_parameter_t){1.F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.F, 0.F, 1.F, 0.1F};
}

float fuzz_process(float in, pedal_config_t *conf) {
    float out = in * conf->float_params[INTENSITY].value;
    float threshold_high = conf->float_params[THRESHOLD_HIGH].value;
    float height = conf->float_params[HEIGHT].value;
    float speed = conf->float_params[SPEED].value;

    out = hard_clip(out, threshold_high);
    if (out == threshold_high) {
        out = out + wave_gen('s', conf->int_params[COUNTER].value, height, speed) - height;
    } else if (out == -threshold_high - 1.F) {
        out = out - wave_gen('s', conf->int_params[COUNTER].value, height, speed) + height;
    }
    conf->int_params[COUNTER].value++;
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}