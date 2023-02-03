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

void amplifier_process(float *value, effect_config_t *conf);

void bitcrusher_rs_process(float *value, effect_config_t *conf);

//void bypass_process(float *value, effect_config_t *conf);

void fuzz_process(float *value, effect_config_t *conf);

void high_pass_filter_process(float *value, effect_config_t *conf);

void low_pass_filter_process(float *value, effect_config_t *conf);

void noise_gate_process(float *value, effect_config_t *conf);

void overdrive_process(float *value, effect_config_t *conf);

void overdrive_sqrt_process(float *value, effect_config_t *conf);

void tremolo_process(float *value, effect_config_t *conf);

void mix(float *raw_in, float *raw_out, float *proc_out, effect_config_t *conf);
void hard_clip(float *value, effect_config_t *conf);
void soft_clip(float *value, effect_config_t *conf);
void square_root(float *value);
void wave_gen(float *out, char t, uint32_t i, float tone);

// MANIFESTs
// ----------------------------------------------------------------------------------------------------- active, name, def, min, max, qual

const effect_manifest_t Effects_Manifest[EFFECT_TYPES] = {
		[AMPLIFIER] = (effect_manifest_t){"amp","amplifier"},
		[AMPLIFIER].params_manifest.float_params_manifest[INTENSITY] = (float_params_manifest_t)		{ 1, "Intensity", 1.0, 0.0, 10.0, VALUE },
		[AMPLIFIER].params_manifest.float_params_manifest[THRESHOLD_HIGH] = (float_params_manifest_t)	{ 1, "Max threshold", 32767, 0, 32767, PERCENTAGE },
		[AMPLIFIER].params_manifest.float_params_manifest[BALANCE_IN] = (float_params_manifest_t)		{ 1, "Balance IN", 0, 0, 1.0, PERCENTAGE },
		[AMPLIFIER].params_manifest.float_params_manifest[BALANCE_OUT] = (float_params_manifest_t)		{ 1, "Balance OUT", 1, 0, 1.0, PERCENTAGE },
		[AMPLIFIER].effect_process = amplifier_process,

		[BITCRUSHER_RS] = (effect_manifest_t){"brs","bitcrusher rs"},
		[BITCRUSHER_RS].params_manifest.int_params_manifest[REDUCTION] = (int_params_manifest_t)		{ 1, "Reduction intens.", 4, 1, 16, VALUE },
		[BITCRUSHER_RS].params_manifest.float_params_manifest[BALANCE_IN] = (float_params_manifest_t)	{ 1, "Balance IN", 0.9, 0, 1.F, PERCENTAGE },
		[BITCRUSHER_RS].params_manifest.float_params_manifest[BALANCE_OUT] = (float_params_manifest_t)	{ 1, "Balance OUT", 0.1, 0, 1.F, PERCENTAGE },
		[BITCRUSHER_RS].effect_process = bitcrusher_rs_process,

		[BITCRUSHER_RT] = (effect_manifest_t){"brt","bitcrusher rt"},
		[BITCRUSHER_RT].params_manifest.int_params_manifest[REDUCTION] = (int_params_manifest_t)		{ 1, "Reduction intens.", 12, 1, 16, VALUE },
		[BITCRUSHER_RT].params_manifest.float_params_manifest[BALANCE_IN] = (float_params_manifest_t)	{ 1, "Balance IN", 0.5, 0, 1.F, PERCENTAGE },
		[BITCRUSHER_RT].params_manifest.float_params_manifest[BALANCE_OUT] = (float_params_manifest_t)	{ 1, "Balance OUT", 0.5, 0, 1.F, PERCENTAGE },
		[BITCRUSHER_RT].effect_process = bitcrusher_rs_process,

		[BYPASS] = (effect_manifest_t){"","bypass"},

		[FUZZ] = (effect_manifest_t){"fzz","fuzz"},
		[FUZZ].params_manifest.float_params_manifest[INTENSITY] = (float_params_manifest_t)				{ 1, "Intensity", 4, 0, 10, VALUE },
		[FUZZ].params_manifest.float_params_manifest[THRESHOLD_HIGH] = (float_params_manifest_t)		{ 1, "Max threshold", 32767, 0, 32767, PERCENTAGE },
		[FUZZ].params_manifest.float_params_manifest[HEIGHT] = (float_params_manifest_t)				{ 1, "Height", 2048, 0, 32767, PERCENTAGE },
		[FUZZ].params_manifest.float_params_manifest[SPEED] = (float_params_manifest_t)					{ 1, "Speed", 1, 0, 10, VALUE },
		[FUZZ].params_manifest.float_params_manifest[BALANCE_IN] = (float_params_manifest_t)			{ 1, "Balance IN", 0.F, 0.F, 1.F, PERCENTAGE },
		[FUZZ].params_manifest.float_params_manifest[BALANCE_OUT] = (float_params_manifest_t)			{ 1, "Balance OUT", 1.F, 0.F, 1.F, PERCENTAGE },
		[FUZZ].effect_process = fuzz_process,

		[HPF] = (effect_manifest_t){"hpf","high pass filter"},
		[HPF].params_manifest.float_params_manifest[SOFTENER] = (float_params_manifest_t)				{ 1, "Softener", 0.1, 0, 1, VALUE },
		[HPF].params_manifest.float_params_manifest[BALANCE_IN] = (float_params_manifest_t)				{ 1, "Balance IN", 0.F, 0.F, 1.F, PERCENTAGE },
		[HPF].params_manifest.float_params_manifest[BALANCE_OUT] = (float_params_manifest_t)			{ 1, "Balance OUT", 1.F, 0.F, 1.F, PERCENTAGE },
		[HPF].effect_process = high_pass_filter_process,

		[LPF] = (effect_manifest_t){"lpf","low pass filter"},
		[LPF].params_manifest.float_params_manifest[SOFTENER] = (float_params_manifest_t)				{ 1, "Softener", 0.1, 0, 1, VALUE },
		[LPF].params_manifest.float_params_manifest[BALANCE_IN] = (float_params_manifest_t)				{ 1, "Balance IN", 0.F, 0.F, 1.F, PERCENTAGE },
		[LPF].params_manifest.float_params_manifest[BALANCE_OUT] = (float_params_manifest_t)			{ 1, "Balance OUT", 1.F, 0.F, 1.F, PERCENTAGE },
		[LPF].effect_process = low_pass_filter_process,

		[NOISE_GATE] = (effect_manifest_t){"ngt","noise gate"},
		[NOISE_GATE].params_manifest.float_params_manifest[THRESHOLD_HIGH] = (float_params_manifest_t)	{ 1, "Min threshold", 0, 0, 1024, PERCENTAGE },
		[NOISE_GATE].params_manifest.float_params_manifest[BALANCE_IN] = (float_params_manifest_t)		{ 1, "Balance IN", 0.F, 0.F, 1.F, PERCENTAGE },
		[NOISE_GATE].params_manifest.float_params_manifest[BALANCE_OUT] = (float_params_manifest_t)		{ 1, "Balance OUT", 1.F, 0.F, 1.F, PERCENTAGE },
		[NOISE_GATE].effect_process = noise_gate_process,

		[OVERDRIVE] = (effect_manifest_t){"ovr","overdrive"},
		[OVERDRIVE].params_manifest.float_params_manifest[INTENSITY] = (float_params_manifest_t)		{ 1, "Intensity", 10, 0, 100, VALUE },
		[OVERDRIVE].params_manifest.float_params_manifest[THRESHOLD_HIGH] = (float_params_manifest_t)	{ 1, "Max threshold", 32767.F, 0.F, 32767.F, PERCENTAGE },
		[OVERDRIVE].params_manifest.float_params_manifest[THRESHOLD_LOW] = (float_params_manifest_t)	{ 1, "Level threshold", 28000.F, 0, 32767.F, PERCENTAGE },
		[OVERDRIVE].params_manifest.float_params_manifest[SOFTENER] = (float_params_manifest_t)			{ 1, "Softener", 1, 0, 10, VALUE },
		[OVERDRIVE].params_manifest.float_params_manifest[BALANCE_IN] = (float_params_manifest_t)		{ 1, "Balance IN", 0.F, 0.F, 1.F, PERCENTAGE },
		[OVERDRIVE].params_manifest.float_params_manifest[BALANCE_OUT] = (float_params_manifest_t)		{ 1, "Balance OUT", 1.F, 0.F, 1.F, PERCENTAGE },
		[OVERDRIVE].effect_process = overdrive_process,

		[OVERDRIVE_SQRT] = (effect_manifest_t){"ovrs","overdrive s"},
		[OVERDRIVE_SQRT].params_manifest.float_params_manifest[INTENSITY] = (float_params_manifest_t)		{ 1, "Intensity", 100, 0, 1000, VALUE },
		[OVERDRIVE_SQRT].params_manifest.float_params_manifest[THRESHOLD_HIGH] = (float_params_manifest_t)	{ 1, "Max threshold", 32767.F, 0.F, 32767.F, PERCENTAGE },
		[OVERDRIVE_SQRT].params_manifest.float_params_manifest[BALANCE_IN] = (float_params_manifest_t)		{ 1, "Balance IN", 0.F, 0.F, 1.F, PERCENTAGE },
		[OVERDRIVE_SQRT].params_manifest.float_params_manifest[BALANCE_OUT] = (float_params_manifest_t)		{ 1, "Balance OUT", 1.F, 0.F, 1.F, PERCENTAGE },
		[OVERDRIVE_SQRT].effect_process = overdrive_sqrt_process,

		[TREMOLO] = (effect_manifest_t){"trm","tremolo"},
		[TREMOLO].params_manifest.float_params_manifest[SPEED] = (float_params_manifest_t)				{ 1, "Speed", 2, 0, 10, VALUE },
		[TREMOLO].params_manifest.float_params_manifest[BALANCE_IN] = (float_params_manifest_t)			{ 1, "Balance IN", 0.F, 0.F, 1.F, PERCENTAGE },
		[TREMOLO].params_manifest.float_params_manifest[BALANCE_OUT] = (float_params_manifest_t)		{ 1, "Balance OUT", 1.F, 0.F, 1.F, PERCENTAGE },
		[TREMOLO].effect_process = tremolo_process
};

const char mode_manifest[MODE_TYPES][10] = {
		[TS] = {"ts"},
		[RS] = {"rs"},
		[TRS_B] = {"trs bal."},
		[TRS_UB] = {"trs unb."}
};

// PEDALBOARD

void Pedalboard_Init(Pedalboard_Handler *p_pb) {
	p_pb->active = 1;
	p_pb->input_mode = TS;
	p_pb->output_mode = TRS_B;
	for (u_int8_t i = 0; i < MAX_EFFECTS_COUNT; i++) {
		p_pb->effects[i].effect_formatted.type = BYPASS;
	}
}

void Pedalboard_SetEffect(Pedalboard_Handler *p_pb, uint8_t type, uint8_t i) {
    if (i < MAX_EFFECTS_COUNT) {
    	p_pb->effects[i].effect_formatted.type = BYPASS;
    	for (uint8_t j = 0; j < INT_PARAM_TYPES; j++) {
    		if (Effects_Manifest[type].params_manifest.int_params_manifest[j].active) {
    			p_pb->effects[i].effect_formatted.config.int_params[j] = Effects_Manifest[type].params_manifest.int_params_manifest[j].def;
    		} else {
    			p_pb->effects[i].effect_formatted.config.int_params[j] = 0;
    		}
    	}
    	for (uint8_t j = 0; j < FLOAT_PARAM_TYPES; j++) {
    		if (Effects_Manifest[type].params_manifest.float_params_manifest[j].active) {
				p_pb->effects[i].effect_formatted.config.float_params[j] = Effects_Manifest[type].params_manifest.float_params_manifest[j].def;
			} else {
				p_pb->effects[i].effect_formatted.config.float_params[j] = 0.0;
			}
    	}
        p_pb->effects[i].effect_formatted.type = type;
    }
}

void Pedalboard_DeleteEffect(Pedalboard_Handler *p_pb, uint8_t i) {
	if (i < MAX_EFFECTS_COUNT) {
		p_pb->effects[i].effect_formatted.type = BYPASS;
	}
}

void Pedalboard_Process(Pedalboard_Handler *p_pb, float *value) {
    float pre;
    if (p_pb->active) {
    	for (uint8_t i = 0; i < MAX_EFFECTS_COUNT; i++) {
    		uint8_t type = p_pb->effects[i].effect_formatted.type;
    		if (type != BYPASS) {
    			pre = *value;
    			Effects_Manifest[type].effect_process(value, &(p_pb->effects[i].effect_formatted.config));
    			mix(&pre, value, value, &(p_pb->effects[i].effect_formatted.config));
    		}
        }
    }
}

uint8_t Pedalboard_CountActiveParams(Pedalboard_Handler *p_pb, uint8_t i) {
	uint8_t active_params = 0;
	if (i < MAX_EFFECTS_COUNT) {
		active_params = Pedalboard_CountActiveParamsByType(p_pb->effects[i].effect_formatted.type);
	}
	return active_params;
}

uint8_t Pedalboard_CountActiveParamsByType(uint8_t type) {
	uint8_t active_params = 0;
	for (uint8_t j = 0; j < INT_PARAM_TYPES; j++) {
		if (Effects_Manifest[type].params_manifest.int_params_manifest[j].active) {
			active_params++;
		}
	}
	for (uint8_t j = 0; j < FLOAT_PARAM_TYPES; j++) {
		if (Effects_Manifest[type].params_manifest.float_params_manifest[j].active) {
			active_params++;
		}
	}
	return active_params;
}

void Pedalboard_GetActiveParamsByType(uint8_t active_index, uint8_t type, uint8_t *_int, uint8_t *i) {
	uint8_t active_params = 0;
	for (uint8_t j = 0; j < INT_PARAM_TYPES; j++) {
		if (Effects_Manifest[type].params_manifest.int_params_manifest[j].active) {
			if (active_params == active_index) {
				*_int = 1;
				*i = j;
				return;
			}
			active_params++;
		}
	}
	for (uint8_t j = 0; j < FLOAT_PARAM_TYPES; j++) {
		if (Effects_Manifest[type].params_manifest.float_params_manifest[j].active) {
			if (active_params == active_index) {
				*_int = 0;
				*i = j;
				return;
			}
			active_params++;
		}
	}
}

// AMPLIFIER

void amplifier_process(float *value, effect_config_t *conf) {
    *value *= conf->float_params[INTENSITY];
    hard_clip(value, conf);
}

// BITCRUSHER resolution

void bitcrusher_rs_process(float *value, effect_config_t *conf) {
    int16_t shift = conf->int_params[REDUCTION];
    int16_t _out = (int16_t)*value;
    _out = (_out >> shift);
    _out = (_out << shift);
    _out += (2 << (shift -2));
    *value = _out;
}

// BYPASS
/*
void bypass_process(float *value, effect_config_t *conf) {
    return;
}*/

// FUZZ

void fuzz_process(float *value, effect_config_t *conf) {
	float tone = 440.0F;
	float wave;
	*value *= conf->float_params[INTENSITY];
	hard_clip(value, conf);
	if (*value == conf->float_params[THRESHOLD_HIGH]) {
		wave_gen(&wave, 's', conf->int_params[COUNTER], tone * conf->float_params[SPEED]);
		*value += (wave - 1) * conf->float_params[HEIGHT];
	} else if (*value == - conf->float_params[THRESHOLD_HIGH] - 1.F) {
		wave_gen(&wave, 's', conf->int_params[COUNTER], tone * conf->float_params[SPEED]);
		*value += (wave + 1) * conf->float_params[HEIGHT];
	}
}

// HPF

void high_pass_filter_process(float *value, effect_config_t *conf) {
	//new_hpf = (prev_hpf + new_sample - prev_sample) / (1 + time_constant/sample_period)

    float alpha = conf->float_params[SOFTENER];
    *value = conf->float_params[PAST] * alpha + (1.F - alpha) * *value;
    conf->float_params[PAST] = *value;
}

// LPF

void low_pass_filter_process(float *value, effect_config_t *conf) {
    float alpha = conf->float_params[SOFTENER];
    *value = conf->float_params[PAST] * alpha + (1.F - alpha) * *value;
    conf->float_params[PAST] = *value;
}

// NOISE GATE

void noise_gate_process(float *value, effect_config_t *conf) {
    if (*value < conf->float_params[THRESHOLD_HIGH] && *value >= -conf->float_params[THRESHOLD_HIGH]) {
    	*value = 0;
    }
}

// OVERDRIVE

void overdrive_process(float *value, effect_config_t *conf) {
    *value *=  conf->float_params[INTENSITY];
    soft_clip(value, conf);
    hard_clip(value, conf);
}

// OVERDRIVE_SQRT

void overdrive_sqrt_process(float *value, effect_config_t *conf) {
    square_root(value);
    *value *= conf->float_params[INTENSITY];
    hard_clip(value, conf);
}

// TREMOLO

void tremolo_process(float *value, effect_config_t *conf) {
    float tone = 440.0F;
    wave_gen(value, 's', conf->int_params[COUNTER], tone * conf->float_params[SPEED]);
    conf->int_params[COUNTER]++;
}


// DSP

void mix(float *raw_in, float *raw_out, float *proc_out, effect_config_t *conf) {
	*proc_out = *raw_in * conf->float_params[BALANCE_IN] + *raw_out * conf->float_params[BALANCE_OUT];
}

void hard_clip(float *value, effect_config_t *conf) {
	float thr = conf->float_params[THRESHOLD_HIGH];
    if (*value > thr) {
    	*value = thr;
    } else if (*value < -thr) {
    	*value = -thr - 1.;
    }
}

void soft_clip(float *value, effect_config_t *conf) {
	float thr = conf->float_params[THRESHOLD_LOW];
	float soft = conf->float_params[SOFTENER];
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

const float sin_table[128] = { 0.000000F, 0.024541F, 0.049068F, 0.073565F, 0.098017F, 0.122411F, 0.146730F, 0.170962F, 0.195090F, 0.219101F, 0.242980F, 0.266713F, 0.290284F, 0.313681F, 0.336890F, 0.359895F, 0.382683F, 0.405241F, 0.427555F, 0.449611F, 0.471396F, 0.492898F, 0.514102F, 0.534997F, 0.555570F, 0.575808F, 0.595699F, 0.615231F, 0.634393F, 0.653172F, 0.671559F, 0.689540F, 0.707106F, 0.724247F, 0.740951F, 0.757208F, 0.773010F, 0.788346F, 0.803207F, 0.817584F, 0.831469F, 0.844853F, 0.857728F, 0.870087F, 0.881921F, 0.893224F, 0.903989F, 0.914209F, 0.923879F, 0.932992F, 0.941544F, 0.949528F, 0.956940F, 0.963776F, 0.970031F, 0.975702F, 0.980785F, 0.985277F, 0.989176F, 0.992479F, 0.995185F, 0.997290F, 0.998795F, 0.999699F, 1.000000F, 0.999699F, 0.998796F, 0.997291F, 0.995185F, 0.992480F, 0.989177F, 0.985278F, 0.980786F, 0.975702F, 0.970032F, 0.963776F, 0.956941F, 0.949529F, 0.941545F, 0.932993F, 0.923880F, 0.914210F, 0.903990F, 0.893225F, 0.881922F, 0.870088F, 0.857729F, 0.844855F, 0.831471F, 0.817586F, 0.803209F, 0.788348F, 0.773012F, 0.757210F, 0.740952F, 0.724248F, 0.707108F, 0.689542F, 0.671560F, 0.653174F, 0.634395F, 0.615233F, 0.595701F, 0.575810F, 0.555572F, 0.534999F, 0.514105F, 0.492900F, 0.471399F, 0.449613F, 0.427557F, 0.405243F, 0.382685F, 0.359897F, 0.336892F, 0.313684F, 0.290287F, 0.266715F, 0.242982F, 0.219104F, 0.195093F, 0.170964F, 0.146733F, 0.122413F, 0.098019F, 0.073567F, 0.049070F, 0.024544F };

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
