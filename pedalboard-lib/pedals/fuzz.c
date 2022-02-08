#include "../pedalboard.h"

//$ PEDAL FUZZ fuzz_pedal_init fuzz_process

//$ HEADER

void fuzz_pedal_init(pedal_config_t *conf);

float fuzz_process(float in, pedal_config_t *conf);

//$ HEADER

//$ SOURCE

void fuzz_pedal_init(pedal_config_t *conf) {
    conf->float_params[GAIN_INTENSITY] = (float_parameter_t){4, 1, 10, 0.5};
    conf->float_params[CLIP_THRESHOLD] = (float_parameter_t){32767, 0, 32767, 1};
    conf->float_params[HEIGHT] = (float_parameter_t){2048, 0, 8192, 32};
    conf->float_params[SPEED] = (float_parameter_t){20, 2, 64, 2};
    conf->float_params[BALANCE] = (float_parameter_t){1, 0, 1, 0.1};
}

float fuzz_process(float in, pedal_config_t *conf) {
    static u_int32_t i = 0;
    float out = gain(in, conf->float_params[GAIN_INTENSITY].value);
    out = hard_clip(out, conf->float_params[CLIP_THRESHOLD].value);
    if (out == conf->float_params[CLIP_THRESHOLD].value) {
        out = out + wave_gen('s', i++, conf->float_params[HEIGHT].value, conf->float_params[SPEED].value) - conf->float_params[HEIGHT].value;
    } else if (out == -(conf->float_params[CLIP_THRESHOLD].value)-1) {
        out = out - wave_gen('s', i++, conf->float_params[HEIGHT].value, conf->float_params[SPEED].value) + conf->float_params[HEIGHT].value;
    }
    out = mix(out, in, conf->float_params[BALANCE].value);
    return out;
}

//$ SOURCE