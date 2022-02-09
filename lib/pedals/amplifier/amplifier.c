<<<<<<< HEAD:pedalboard-lib/pedals/amplifier/amplifier.c
#include "amplifier.h"
#include "../../pedalboard.h"
#include "../../dsp/dsp.h"
=======
#include "../pedalboard.h"
>>>>>>> bf62d7bae70927fca8cb880393defa669ad491d1:pedalboard-lib/pedals/amplifier.c

//$ PEDAL AMPLIFIER amplifier_pedal_init amplifier_process

//$ HEADER

void amplifier_pedal_init(pedal_config_t *conf);

float amplifier_process(float in, pedal_config_t *conf);

//$ HEADER

//$ SOURCE

void amplifier_pedal_init(pedal_config_t *conf) {
    conf->float_params[GAIN_INTENSITY] = (float_parameter_t){0.5, 0.1, 10, 0.1};
    conf->float_params[CLIP_THRESHOLD] = (float_parameter_t){32767, 0, 32767, 1};
}

float amplifier_process(float in, pedal_config_t *conf) {
    float out = gain(in, conf->float_params[GAIN_INTENSITY].value);
    out = hard_clip(out, conf->float_params[CLIP_THRESHOLD].value);
    return out;
}

//$ SOURCE