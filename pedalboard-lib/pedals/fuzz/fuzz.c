#include "fuzz.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"

// FUZZ

void fuzz_pedal_init(pedal_config_t *conf) {
    conf->u_int_params[COUNTER] = (u_int_parameter_t){0, 1, 0, 0};
    conf->float_params[GAIN_INTENSITY] = (float_parameter_t){4, 1, 10, 0.5};
    conf->float_params[CLIP_THRESHOLD] = (float_parameter_t){32767, 0, 32767, 1};
    conf->float_params[HEIGHT] = (float_parameter_t){2048, 0, 8192, 32};
    conf->float_params[SPEED] = (float_parameter_t){20, 2, 64, 2};
    conf->float_params[BALANCE_1] = (float_parameter_t){1, 0, 1, 0.1};
    conf->float_params[BALANCE_2] = (float_parameter_t){0, 0, 1, 0.1};
}

float fuzz_process(float in, pedal_config_t *conf) {
    float out = gain(in, conf->float_params[GAIN_INTENSITY].value);
    out = hard_clip(out, conf->float_params[CLIP_THRESHOLD].value);
    if (out == conf->float_params[CLIP_THRESHOLD].value) {
        out = out + wave_gen('s', conf->u_int_params[COUNTER].value, conf->float_params[HEIGHT].value, conf->float_params[SPEED].value) - conf->float_params[HEIGHT].value;
    } else if (out == -(conf->float_params[CLIP_THRESHOLD].value)-1) {
        out = out - wave_gen('s', conf->u_int_params[COUNTER].value++, conf->float_params[HEIGHT].value, conf->float_params[SPEED].value) + conf->float_params[HEIGHT].value;
    }
    conf->u_int_params[COUNTER].value++;
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}