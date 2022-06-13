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
void overdrive_pedal_process(float *value, pedal_config_t *conf);

void bitcrusher_rs_pedal_init(pedal_config_t *conf);
void bitcrusher_rs_pedal_process(float *value, pedal_config_t *conf);

void tremolo_pedal_init(pedal_config_t *conf);
void tremolo_pedal_process(float *value, pedal_config_t *conf);

void overdrive_sqrt_pedal_init(pedal_config_t *conf);
void overdrive_sqrt_pedal_process(float *value, pedal_config_t *conf);

void amplifier_pedal_init(pedal_config_t *conf);
void amplifier_pedal_process(float *value, pedal_config_t *conf);

void low_pass_filter_pedal_init(pedal_config_t *conf);
void low_pass_filter_pedal_process(float *value, pedal_config_t *conf);

void fuzz_pedal_init(pedal_config_t *conf);
void fuzz_pedal_process(float *value, pedal_config_t *conf);

void noise_gate_pedal_init(pedal_config_t *conf);
void noise_gate_pedal_process(float *value, pedal_config_t *conf);

void bypass_pedal_init(pedal_config_t *conf);
void bypass_pedal_process(float *value, pedal_config_t *conf);

void mix(float *raw_in, float *raw_out, float *proc_out, pedal_config_t *conf);
void hard_clip(float *value, pedal_config_t *conf);
void soft_clip(float *value, pedal_config_t *conf);
void square_root(float *value);
void wave_gen(float *out, char t, uint32_t i, float tone);

// PEDALBOARD

void Pedalboard_Init(Pedalboard_Handler *p_pb) {
	p_pb->active_pedals = 0;
}

void Pedalboard_Append(Pedalboard_Handler *p_pb, enum pedal_types type) {
    uint8_t i = p_pb->active_pedals;

    if (i < MAX_PEDALS_COUNT) {
        p_pb->pedals[i].type = type;

        if (type == AMPLIFIER)
        {
            amplifier_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = amplifier_pedal_process;
        }
        else if (type == BITCRUSHER_RS)
        {
            bitcrusher_rs_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = bitcrusher_rs_pedal_process;
        }
        else if (type == FUZZ)
        {
            fuzz_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = fuzz_pedal_process;
        }
        else if (type == LPF)
        {
            low_pass_filter_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = low_pass_filter_pedal_process;
        }
        else if (type == OVERDRIVE)
        {
            overdrive_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = overdrive_pedal_process;
        }
        else if (type == OVERDRIVE_SQRT)
        {
            overdrive_sqrt_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = overdrive_sqrt_pedal_process;
        }
        else if (type == TREMOLO)
        {
            tremolo_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = tremolo_pedal_process;
        }
        else if (type == NOISE_GATE)
		{
			noise_gate_pedal_init(&(p_pb->pedals[i].config));
			p_pb->pedals[i].pedal_process = noise_gate_pedal_process;
		}
        else
        {
            // BYPASS AS DEFAULT
            bypass_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = bypass_pedal_process;
        }
        p_pb->active_pedals++;
    }
}

void Pedalboard_Process(Pedalboard_Handler *p_pb, float *value) {
    float pre;
	for (u_int8_t i = 0; i < p_pb->active_pedals; i++) {
    	pre = *value;
        p_pb->pedals[i].pedal_process(value, &(p_pb->pedals[i].config));
        mix(&pre, value, value, &(p_pb->pedals[i].config));
    }
}

// OVERDRIVE

void overdrive_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){100.F, 1.F, 10.F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F};
    conf->float_params[THRESHOLD_LOW] = (float_parameter_t){28000.F, 0.F, 32767.F};
    conf->float_params[SOFTENER] = (float_parameter_t){4.F, 1.F, 10.F};
    conf->float_params[BALANCE_IN] = (float_parameter_t){0.F, 0.F, 1.F};
    conf->float_params[BALANCE_OUT] = (float_parameter_t){1.F, 0.F, 1.F};
}

void overdrive_pedal_process(float *value, pedal_config_t *conf) {
    *value *=  conf->float_params[INTENSITY].value;
    soft_clip(value, conf);
    hard_clip(value, conf);
}

// BITCRUSHER resolution

void bitcrusher_rs_pedal_init(pedal_config_t *conf) {
    conf->int_params[REDUCT_INTENSITY] = (int_parameter_t){12, 1, 16};
    conf->float_params[BALANCE_IN] = (float_parameter_t){0.5F, 0.F, 1.F};
    conf->float_params[BALANCE_OUT] = (float_parameter_t){0.5F, 0.F, 1.F};
}

void bitcrusher_rs_pedal_process(float *value, pedal_config_t *conf) {
    int16_t shift = conf->int_params[REDUCT_INTENSITY].value;
    int16_t _out = (int16_t)*value;
    _out = (_out >> shift);
    _out = (_out << shift);
    _out += (2 << (shift -2));
    *value = _out;
}

// TREMOLO

void tremolo_pedal_init(pedal_config_t *conf) {
    conf->int_params[COUNTER] = (int_parameter_t){0, 1, 0};
    conf->float_params[SPEED] = (float_parameter_t){2.F, 0.1F, 10.F};
    conf->float_params[BALANCE_IN] = (float_parameter_t){1.0F, 0.F, 1.F};
    conf->float_params[BALANCE_OUT] = (float_parameter_t){0.0F, 0.F, 1.F};
}

void tremolo_pedal_process(float *value, pedal_config_t *conf) {
    float tone = 440.0F;
    wave_gen(value, 's', conf->int_params[COUNTER].value, tone * conf->float_params[SPEED].value);
    conf->int_params[COUNTER].value++;
}

// OVERDRIVE_SQRT

void overdrive_sqrt_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){500.F, 0.F, 2000.F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F};
    conf->float_params[BALANCE_IN] = (float_parameter_t){0.75F, 0.F, 1.F};
    conf->float_params[BALANCE_OUT] = (float_parameter_t){0.25F, 0.F, 1.F};
}

void overdrive_sqrt_pedal_process(float *value, pedal_config_t *conf) {
    square_root(value);
    *value *= conf->float_params[INTENSITY].value;
    hard_clip(value, conf);
}

// AMPLIFIER

void amplifier_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){1.F, 0.1F, 10.F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F};
    conf->float_params[BALANCE_IN] = (float_parameter_t){0.F, 0.F, 1.F};
	conf->float_params[BALANCE_OUT] = (float_parameter_t){1.F, 0.F, 1.F};
}

void amplifier_pedal_process(float *value, pedal_config_t *conf) {
    *value *= conf->float_params[INTENSITY].value;
    hard_clip(value, conf);
}

// LPF

void low_pass_filter_pedal_init(pedal_config_t *conf) {
    conf->float_params[SOFTENER] = (float_parameter_t){0.9F, 0.0F, 1.0F};
    conf->float_params[BALANCE_IN] = (float_parameter_t){0.F, 0.F, 1.F};
    conf->float_params[BALANCE_OUT] = (float_parameter_t){1.F, 0.F, 1.F};
    conf->float_params[PAST] = (float_parameter_t){0.F, 0.F, 0.F};
}

void low_pass_filter_pedal_process(float *value, pedal_config_t *conf) {
    float alpha = conf->float_params[SOFTENER].value;
    *value = conf->float_params[PAST].value * alpha + (1.F - alpha) * *value;
    conf->float_params[PAST].value = *value;
}

// FUZZ

void fuzz_pedal_init(pedal_config_t *conf) {
    conf->int_params[COUNTER] = (int_parameter_t){0, 1, 0};
    conf->float_params[INTENSITY] = (float_parameter_t){4.F, 1.F, 10.F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F};
    conf->float_params[HEIGHT] = (float_parameter_t){2048.F, 0.F, 8192.F};
    conf->float_params[SPEED] = (float_parameter_t){1.F, 0.1F, 10.F,};
    conf->float_params[BALANCE_IN] = (float_parameter_t){0.F, 0.F, 1.F};
    conf->float_params[BALANCE_OUT] = (float_parameter_t){1.F, 0.F, 1.F};
}

void fuzz_pedal_process(float *value, pedal_config_t *conf) {
	float tone = 440.0F;
	float wave;
	*value *= conf->float_params[INTENSITY].value;
	hard_clip(value, conf);
	if (*value == conf->float_params[THRESHOLD_HIGH].value) {
		wave_gen(&wave, 's', conf->int_params[COUNTER].value, tone * conf->float_params[SPEED].value);
		*value += (wave - 1) * conf->float_params[HEIGHT].value;
	} else if (*value == - conf->float_params[THRESHOLD_HIGH].value - 1.F) {
		wave_gen(&wave, 's', conf->int_params[COUNTER].value, tone * conf->float_params[SPEED].value);
		*value += (wave + 1) * conf->float_params[HEIGHT].value;
	}
}

// NOISE GATE

void noise_gate_pedal_init(pedal_config_t *conf) {
	conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){50.F, 0.F, 32767.F};
	conf->float_params[BALANCE_IN] = (float_parameter_t){0.F, 0.F, 1.F};
	conf->float_params[BALANCE_OUT] = (float_parameter_t){1.F, 0.F, 1.F};
}

void noise_gate_pedal_process(float *value, pedal_config_t *conf) {
    if (*value < conf->float_params[THRESHOLD_HIGH].value) {
    	*value = 0;
    }
}

// BYPASS

void bypass_pedal_init(pedal_config_t *conf) {
    return;
}

void bypass_pedal_process(float *value, pedal_config_t *conf) {
    return;
}

// DSP

void mix(float *raw_in, float *raw_out, float *proc_out, pedal_config_t *conf) {
	*proc_out = *raw_in * conf->float_params[BALANCE_IN].value + *raw_out * conf->float_params[BALANCE_OUT].value;
}

void hard_clip(float *value, pedal_config_t *conf) {
	float thr = conf->float_params[THRESHOLD_HIGH].value;
    if (*value > thr) {
    	*value = thr;
    } else if (*value < -thr) {
    	*value = -thr - 1.;
    }
}

void soft_clip(float *value, pedal_config_t *conf) {
	float thr = conf->float_params[THRESHOLD_LOW].value;
	float soft = conf->float_params[SOFTENER].value;
    if (*value > thr) {
    	*value = thr + (*value - thr) / soft;
    } else if (*value < -thr) {
    	*value = -thr + (*value + thr) / soft;
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
    uint32_t period_i = period_f;
    uint32_t table_index = (i * 256 / period_i) % 256;

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
