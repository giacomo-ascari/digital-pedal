/*
 * pedalboard.c
 *
 *  Created on: 15 mag 2022
 *      Author: giaco
 */


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "pedalboard.h"

// DECLARATIONS

void overdrive_pedal_init(pedal_config_t *conf);
void overdrive_process(float *value, pedal_config_t *conf);

void bitcrusher_rs_pedal_init(pedal_config_t *conf);
void bitcrusher_rs_process(float *value, pedal_config_t *conf);

void tremolo_pedal_init(pedal_config_t *conf);
void tremolo_process(float *value, pedal_config_t *conf);

void overdrive_sqrt_pedal_init(pedal_config_t *conf);
void overdrive_sqrt_process(float *value, pedal_config_t *conf);

void amplifier_pedal_init(pedal_config_t *conf);
void amplifier_process(float *value, pedal_config_t *conf);

void dyn_amplifier_pedal_init(pedal_config_t *conf);
void dyn_amplifier_process(float *value, pedal_config_t *conf);

void low_pass_filter_pedal_init(pedal_config_t *conf);
void low_pass_filter_process(float *value, pedal_config_t *conf);

void fuzz_pedal_init(pedal_config_t *conf);
void fuzz_process(float *value, pedal_config_t *conf);

void bypass_pedal_init(pedal_config_t *conf);
void bypass_process(float *value, pedal_config_t *conf);

void mix(float *out, float *value_1, float *value_2, float *balance_1, float *balance_2);
void hard_clip(float *value, float *clip_threshold);
void soft_clip(float *value, float *soft_threshold, float *softener);
void square_root(float *value);
void wave_gen(float *out, char t, uint32_t i, float tone);

// PEDALBOARD

void pedalboard_init(pedalboard_t *p_pb) {
	p_pb->active_pedals = 0;
}

void pedalboard_append(pedalboard_t *p_pb, enum pedal_types type) {
    u_int8_t i = p_pb->active_pedals;

    if (p_pb->active_pedals < MAX_PEDALS_COUNT) {
        u_int8_t i = p_pb->active_pedals;
        p_pb->pedals[i].type = type;

        if (type == AMPLIFIER)
        {
            amplifier_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = amplifier_process;
        }
        else if (type == BITCRUSHER_RS)
        {
            bitcrusher_rs_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = bitcrusher_rs_process;
        }
        else if (type == DYN_AMPLIFIER)
        {
            dyn_amplifier_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = dyn_amplifier_process;
        }
        else if (type == FUZZ)
        {
            fuzz_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = fuzz_process;
        }
        else if (type == LPF)
        {
            low_pass_filter_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = low_pass_filter_process;
        }
        else if (type == OVERDRIVE)
        {
            overdrive_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = overdrive_process;
        }
        else if (type == OVERDRIVE_SQRT)
        {
            overdrive_sqrt_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = overdrive_sqrt_process;
        }
        else if (type == TREMOLO)
        {
            tremolo_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = tremolo_process;
        }
        else
        {
            // BYPASS AS DEFAULT
            bypass_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = bypass_process;
        }
        p_pb->active_pedals++;
    }
}

void pedalboard_process(pedalboard_t *p_pb, float *value) {
    for (u_int8_t i = 0; i < p_pb->active_pedals; i++) {
        p_pb->pedals[i].pedal_process(value, &(p_pb->pedals[i].config));
    }
}

// OVERDRIVE

void overdrive_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){4.F, 1.F, 10.F, 0.5F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F, 1.F};
    conf->float_params[THRESHOLD_LOW] = (float_parameter_t){28000.F, 0.F, 32767.F, 1.F};
    conf->float_params[SOFTENER] = (float_parameter_t){4.F, 1.F, 10.F, 0.5F};
    conf->float_params[BALANCE_1] = (float_parameter_t){1.F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.F, 0.F, 1.F, 0.1F};
}

void overdrive_process(float *value, pedal_config_t *conf) {
    *value *=  conf->float_params[INTENSITY].value;
    soft_clip(value, &(conf->float_params[THRESHOLD_LOW].value), &(conf->float_params[SOFTENER].value));
    hard_clip(value, &(conf->float_params[THRESHOLD_HIGH].value));
}

// BITCRUSHER resolution

void bitcrusher_rs_pedal_init(pedal_config_t *conf) {
    conf->int_params[REDUCT_INTENSITY] = (int_parameter_t){12, 1, 16, 1};
    conf->float_params[BALANCE_1] = (float_parameter_t){0.5F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.5F, 0.F, 1.F, 0.1F};
}

void bitcrusher_rs_process(float *value, pedal_config_t *conf) {
    int16_t shift = conf->int_params[REDUCT_INTENSITY].value;
    int16_t _out = (int16_t)*value;
    _out = (_out >> shift);
    _out = (_out << shift);
    _out += (2 << (shift -2));
    *value = _out;
}

// TREMOLO

void tremolo_pedal_init(pedal_config_t *conf) {
    conf->int_params[COUNTER] = (int_parameter_t){0, 1, 0, 0};
    conf->float_params[SPEED] = (float_parameter_t){2.F, 0.1F, 10.F, 0.1F};
    conf->float_params[BALANCE_1] = (float_parameter_t){0.2F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.8F, 0.F, 1.F, 0.1F};
}

void tremolo_process(float *value, pedal_config_t *conf) {
    //float tone = 440.0F;
    //float wave_value;
    //float out = in * wave_gen('s', conf->int_params[COUNTER].value, tone * conf->float_params[SPEED].value);
    //conf->int_params[COUNTER].value++;
    //out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    //return out;
}

// OVERDRIVE_SQRT

void overdrive_sqrt_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){500.F, 0.F, 2000.F, 100.F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F, 1.F};
    conf->float_params[BALANCE_1] = (float_parameter_t){0.25F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.75F, 0.F, 1.F, 0.1F};
}

void overdrive_sqrt_process(float *value, pedal_config_t *conf) {
    square_root(value);
    *value *= conf->float_params[INTENSITY].value;
    hard_clip(value, &(conf->float_params[THRESHOLD_HIGH].value));
}

// AMPLIFIER

void amplifier_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){0.5F, 0.1F, 10.F, 0.1F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F, 1.F};
}

void amplifier_process(float *value, pedal_config_t *conf) {
    *value *= conf->float_params[INTENSITY].value;
    hard_clip(value, &(conf->float_params[THRESHOLD_HIGH].value));
}

// DYN_AMPLIFIER

void dyn_amplifier_pedal_init(pedal_config_t *conf) {
    //conf->float_params[GAIN_INTENSITY] = (float_parameter_t){1000, 0.1, 10, 0.1};
    //conf->float_params[CLIP_THRESHOLD] = (float_parameter_t){32767, 0, 32767, 1};
    //conf->float_params[SOFT_THRESHOLD] = (float_parameter_t){28000, 0, 32767, 1};
    //conf->float_params[BALANCE] = (float_parameter_t){0.5, 0, 1, 0.1};
}

void dyn_amplifier_process(float *value, pedal_config_t *conf) {
	//pass
}

// LPF

void low_pass_filter_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){0.9F, 0.0F, 1.0F, 0.01F};
    conf->float_params[BALANCE_1] = (float_parameter_t){1.F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.F, 0.F, 1.F, 0.1F};
    conf->float_params[PAST] = (float_parameter_t){0.F, 0.F, 0.F, 0.F};
}

void low_pass_filter_process(float *value, pedal_config_t *conf) {
    float alpha = conf->float_params[INTENSITY].value;
    *value = conf->float_params[PAST].value * alpha + (1.F - alpha) * *value;
    conf->float_params[PAST].value = *value;
}

// FUZZ

void fuzz_pedal_init(pedal_config_t *conf) {
    conf->int_params[COUNTER] = (int_parameter_t){0, 1, 0, 0};
    conf->float_params[INTENSITY] = (float_parameter_t){4.F, 1.F, 10.F, 0.5F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F, 1.F};
    conf->float_params[HEIGHT] = (float_parameter_t){2048.F, 0.F, 8192.F, 32.F};
    conf->float_params[SPEED] = (float_parameter_t){1.F, 0.1F, 10.F, 0.1F};
    conf->float_params[BALANCE_1] = (float_parameter_t){1.F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.F, 0.F, 1.F, 0.1F};
}

void fuzz_process(float *value, pedal_config_t *conf) {
    //float out = in * conf->float_params[INTENSITY].value;
    //float threshold_high = conf->float_params[THRESHOLD_HIGH].value;
    //float height = conf->float_params[HEIGHT].value;
    //float speed = conf->float_params[SPEED].value;
    //float tone = 440.0F;
    //out = hard_clip(out, threshold_high);
    //if (out == threshold_high) {
    //    out += (wave_gen('s', conf->int_params[COUNTER].value, tone * speed) - 1) * height;
    //} else if (out == - threshold_high - 1.F) {
    //    out += (wave_gen('s', conf->int_params[COUNTER].value, tone * speed) + 1) * height;
    //}
    //conf->int_params[COUNTER].value++;
    //out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    //return out;
}

// BYPASS

void bypass_pedal_init(pedal_config_t *conf) {
    return;
}

void bypass_process(float *value, pedal_config_t *conf) {
    // pass
}

// DSP

void mix(float *out, float *value_1, float *value_2, float *balance_1, float *balance_2) {
	*out = *value_1 * *balance_1 + *value_2 * *balance_2;
}

void hard_clip(float *value, float *clip_threshold) {
    if (*value > *clip_threshold) {
    	*value = *clip_threshold;
    } else if (*value < -(*clip_threshold)) {
    	*value = -(*clip_threshold) - 1.;
    }
}

void soft_clip(float *value, float *soft_threshold, float *softener) {
    if (*value > *soft_threshold) {
    	*value = *soft_threshold + (*value - *soft_threshold) / *softener;
    } else if (*value < -*soft_threshold) {
    	*value = -*soft_threshold + (*value + *soft_threshold) / *softener;
    }
}

void square_root(float *value) {
	float multiplier = (*value < 0.F) ? -1.F : 1.F;
	*value *= multiplier;
	float original = *value;
    *value /= 2;
    for (u_int8_t i = 0; i < 8; i++) {
    	*value = 0.5F * (*value + (original / *value));
    }
    *value *= multiplier;
}

float sin_table[128] = { 0.000000F, 0.024541F, 0.049068F, 0.073565F, 0.098017F, 0.122411F, 0.146730F, 0.170962F, 0.195090F, 0.219101F, 0.242980F, 0.266713F, 0.290284F, 0.313681F, 0.336890F, 0.359895F, 0.382683F, 0.405241F, 0.427555F, 0.449611F, 0.471396F, 0.492898F, 0.514102F, 0.534997F, 0.555570F, 0.575808F, 0.595699F, 0.615231F, 0.634393F, 0.653172F, 0.671559F, 0.689540F, 0.707106F, 0.724247F, 0.740951F, 0.757208F, 0.773010F, 0.788346F, 0.803207F, 0.817584F, 0.831469F, 0.844853F, 0.857728F, 0.870087F, 0.881921F, 0.893224F, 0.903989F, 0.914209F, 0.923879F, 0.932992F, 0.941544F, 0.949528F, 0.956940F, 0.963776F, 0.970031F, 0.975702F, 0.980785F, 0.985277F, 0.989176F, 0.992479F, 0.995185F, 0.997290F, 0.998795F, 0.999699F, 1.000000F, 0.999699F, 0.998796F, 0.997291F, 0.995185F, 0.992480F, 0.989177F, 0.985278F, 0.980786F, 0.975702F, 0.970032F, 0.963776F, 0.956941F, 0.949529F, 0.941545F, 0.932993F, 0.923880F, 0.914210F, 0.903990F, 0.893225F, 0.881922F, 0.870088F, 0.857729F, 0.844855F, 0.831471F, 0.817586F, 0.803209F, 0.788348F, 0.773012F, 0.757210F, 0.740952F, 0.724248F, 0.707108F, 0.689542F, 0.671560F, 0.653174F, 0.634395F, 0.615233F, 0.595701F, 0.575810F, 0.555572F, 0.534999F, 0.514105F, 0.492900F, 0.471399F, 0.449613F, 0.427557F, 0.405243F, 0.382685F, 0.359897F, 0.336892F, 0.313684F, 0.290287F, 0.266715F, 0.242982F, 0.219104F, 0.195093F, 0.170964F, 0.146733F, 0.122413F, 0.098019F, 0.073567F, 0.049070F, 0.024544F };

void wave_gen(float *out, char t, uint32_t i, float tone) {
    float period_f = 48000.F / tone;
    u_int32_t period_i = period_f;
    u_int32_t table_index = (i * 256 / period_i) % 256;

    if (t == 's') {
        if (table_index <= 127) {
        	*out = sin_table[table_index];
        } else {
            *out = -sin_table[table_index - 128];
        }
    } else if (t == 'q') {
        if (table_index <= 127) {
        	*out = -1.F;
        } else {
        	*out = 1.F;
        }
    }
}
