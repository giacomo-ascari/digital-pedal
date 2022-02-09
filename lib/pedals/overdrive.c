<<<<<<< HEAD:pedalboard-lib/pedals/overdrive/overdrive.c
#include "overdrive.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"
=======
#include "../pedalboard.h"
>>>>>>> bf62d7bae70927fca8cb880393defa669ad491d1:pedalboard-lib/pedals/overdrive.c

//$ PEDAL OVERDRIVE overdrive_pedal_init overdrive_process

//$ HEADER

void overdrive_pedal_init(pedal_config_t *conf);

float overdrive_process(float in, pedal_config_t *conf);

//$ HEADER

//$ SOURCE

void overdrive_pedal_init(pedal_config_t *conf) {
    conf->float_params[GAIN_INTENSITY] = (float_parameter_t){4, 1, 10, 0.5};
    conf->float_params[CLIP_THRESHOLD] = (float_parameter_t){32767, 0, 32767, 1};
    conf->float_params[SOFT_THRESHOLD] = (float_parameter_t){28000, 0, 32767, 1};
    conf->float_params[SOFTENER] = (float_parameter_t){4, 1, 10, 0.5};
    conf->float_params[BALANCE_1] = (float_parameter_t){1, 0, 1, 0.1};
    conf->float_params[BALANCE_2] = (float_parameter_t){0, 0, 1, 0.1};
}

float overdrive_process(float in, pedal_config_t *conf) {
    float out = gain(in, conf->float_params[GAIN_INTENSITY].value);
    out = soft_clip(out, conf->float_params[SOFT_THRESHOLD].value, conf->float_params[SOFTENER].value);
    out = hard_clip(out, conf->float_params[CLIP_THRESHOLD].value);
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}

//$ SOURCE