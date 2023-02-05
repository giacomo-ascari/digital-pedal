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

void wave_gen_process(float *value, effect_config_t *conf);

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
		[TREMOLO].effect_process = tremolo_process,

		[WAVE_GEN] = (effect_manifest_t){"wav","wave gen."},
		[WAVE_GEN].params_manifest.float_params_manifest[HEIGHT] = (float_params_manifest_t)			{ 1, "Height", 2048, 0, 32767, PERCENTAGE },
		[WAVE_GEN].params_manifest.float_params_manifest[SPEED] = (float_params_manifest_t)				{ 1, "Speed", 440, 110, 880, VALUE },
		[WAVE_GEN].params_manifest.float_params_manifest[BALANCE_IN] = (float_params_manifest_t)		{ 1, "Balance IN", 0.F, 0.F, 1.F, PERCENTAGE },
		[WAVE_GEN].params_manifest.float_params_manifest[BALANCE_OUT] = (float_params_manifest_t)		{ 1, "Balance OUT", 1.F, 0.F, 1.F, PERCENTAGE },
		[WAVE_GEN].effect_process = wave_gen_process
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

// WAVE GEN


void wave_gen_process(float *value, effect_config_t *conf) {
	wave_gen(value, 's', conf->int_params[COUNTER], conf->float_params[SPEED]);
	*value *= conf->float_params[HEIGHT];
	//duplicate = conf->float_params[HEIGHT] * sin (6.283185307179586476925286766559 * conf->float_params[SPEED] * conf->int_params[COUNTER] / 48000.F);
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

#ifdef F103

void wave_gen(float *out, char t, uint32_t i, float tone) {
	*out = 0;
}

#endif


#ifdef F407

// 512 samples of half period of a sine-wave
const float sin_table[512] = { 0.000000F, 0.006136F, 0.012272F, 0.018407F, 0.024541F, 0.030675F, 0.036807F, 0.042938F, 0.049068F, 0.055195F, 0.061321F, 0.067444F, 0.073565F, 0.079682F, 0.085797F, 0.091909F, 0.098017F, 0.104122F, 0.110222F, 0.116319F, 0.122411F, 0.128498F, 0.134581F, 0.140658F, 0.146730F, 0.152797F, 0.158858F, 0.164913F, 0.170962F, 0.177004F, 0.183040F, 0.189069F, 0.195090F, 0.201105F, 0.207111F, 0.213110F, 0.219101F, 0.225084F, 0.231058F, 0.237024F, 0.242980F, 0.248928F, 0.254866F, 0.260794F, 0.266713F, 0.272621F, 0.278520F, 0.284408F, 0.290285F, 0.296151F, 0.302006F, 0.307850F, 0.313682F, 0.319502F, 0.325310F, 0.331106F, 0.336890F, 0.342661F, 0.348419F, 0.354164F, 0.359895F, 0.365613F, 0.371317F, 0.377007F, 0.382683F, 0.388345F, 0.393992F, 0.399624F, 0.405241F, 0.410843F, 0.416430F, 0.422000F, 0.427555F, 0.433094F, 0.438616F, 0.444122F, 0.449611F, 0.455084F, 0.460539F, 0.465976F, 0.471397F, 0.476799F, 0.482184F, 0.487550F, 0.492898F, 0.498228F, 0.503538F, 0.508830F, 0.514103F, 0.519356F, 0.524590F, 0.529804F, 0.534998F, 0.540171F, 0.545325F, 0.550458F, 0.555570F, 0.560662F, 0.565732F, 0.570781F, 0.575808F, 0.580814F, 0.585798F, 0.590760F, 0.595699F, 0.600616F, 0.605511F, 0.610383F, 0.615232F, 0.620057F, 0.624859F, 0.629638F, 0.634393F, 0.639124F, 0.643832F, 0.648514F, 0.653173F, 0.657807F, 0.662416F, 0.667000F, 0.671559F, 0.676093F, 0.680601F, 0.685084F, 0.689541F, 0.693971F, 0.698376F, 0.702755F, 0.707107F, 0.711432F, 0.715731F, 0.720003F, 0.724247F, 0.728464F, 0.732654F, 0.736817F, 0.740951F, 0.745058F, 0.749136F, 0.753187F, 0.757209F, 0.761202F, 0.765167F, 0.769103F, 0.773010F, 0.776888F, 0.780737F, 0.784557F, 0.788346F, 0.792107F, 0.795837F, 0.799537F, 0.803208F, 0.806848F, 0.810457F, 0.814036F, 0.817585F, 0.821103F, 0.824589F, 0.828045F, 0.831470F, 0.834863F, 0.838225F, 0.841555F, 0.844854F, 0.848120F, 0.851355F, 0.854558F, 0.857729F, 0.860867F, 0.863973F, 0.867046F, 0.870087F, 0.873095F, 0.876070F, 0.879012F, 0.881921F, 0.884797F, 0.887640F, 0.890449F, 0.893224F, 0.895966F, 0.898674F, 0.901349F, 0.903989F, 0.906596F, 0.909168F, 0.911706F, 0.914210F, 0.916679F, 0.919114F, 0.921514F, 0.923880F, 0.926210F, 0.928506F, 0.930767F, 0.932993F, 0.935184F, 0.937339F, 0.939459F, 0.941544F, 0.943593F, 0.945607F, 0.947586F, 0.949528F, 0.951435F, 0.953306F, 0.955141F, 0.956940F, 0.958703F, 0.960431F, 0.962121F, 0.963776F, 0.965394F, 0.966976F, 0.968522F, 0.970031F, 0.971504F, 0.972940F, 0.974339F, 0.975702F, 0.977028F, 0.978317F, 0.979570F, 0.980785F, 0.981964F, 0.983105F, 0.984210F, 0.985278F, 0.986308F, 0.987301F, 0.988258F, 0.989177F, 0.990058F, 0.990903F, 0.991710F, 0.992480F, 0.993212F, 0.993907F, 0.994565F, 0.995185F, 0.995767F, 0.996313F, 0.996820F, 0.997290F, 0.997723F, 0.998118F, 0.998476F, 0.998795F, 0.999078F, 0.999322F, 0.999529F, 0.999699F, 0.999831F, 0.999925F, 0.999981F, 1.000000F, 0.999981F, 0.999925F, 0.999831F, 0.999699F, 0.999529F, 0.999322F, 0.999078F, 0.998795F, 0.998476F, 0.998118F, 0.997723F, 0.997290F, 0.996820F, 0.996313F, 0.995767F, 0.995185F, 0.994565F, 0.993907F, 0.993212F, 0.992480F, 0.991710F, 0.990903F, 0.990058F, 0.989177F, 0.988258F, 0.987301F, 0.986308F, 0.985278F, 0.984210F, 0.983105F, 0.981964F, 0.980785F, 0.979570F, 0.978317F, 0.977028F, 0.975702F, 0.974339F, 0.972940F, 0.971504F, 0.970031F, 0.968522F, 0.966976F, 0.965394F, 0.963776F, 0.962121F, 0.960431F, 0.958703F, 0.956940F, 0.955141F, 0.953306F, 0.951435F, 0.949528F, 0.947586F, 0.945607F, 0.943593F, 0.941544F, 0.939459F, 0.937339F, 0.935184F, 0.932993F, 0.930767F, 0.928506F, 0.926210F, 0.923880F, 0.921514F, 0.919114F, 0.916679F, 0.914210F, 0.911706F, 0.909168F, 0.906596F, 0.903989F, 0.901349F, 0.898674F, 0.895966F, 0.893224F, 0.890449F, 0.887640F, 0.884797F, 0.881921F, 0.879012F, 0.876070F, 0.873095F, 0.870087F, 0.867046F, 0.863973F, 0.860867F, 0.857729F, 0.854558F, 0.851355F, 0.848120F, 0.844854F, 0.841555F, 0.838225F, 0.834863F, 0.831470F, 0.828045F, 0.824589F, 0.821103F, 0.817585F, 0.814036F, 0.810457F, 0.806848F, 0.803208F, 0.799537F, 0.795837F, 0.792107F, 0.788346F, 0.784557F, 0.780737F, 0.776888F, 0.773010F, 0.769103F, 0.765167F, 0.761202F, 0.757209F, 0.753187F, 0.749136F, 0.745058F, 0.740951F, 0.736817F, 0.732654F, 0.728464F, 0.724247F, 0.720003F, 0.715731F, 0.711432F, 0.707107F, 0.702755F, 0.698376F, 0.693971F, 0.689541F, 0.685084F, 0.680601F, 0.676093F, 0.671559F, 0.667000F, 0.662416F, 0.657807F, 0.653173F, 0.648514F, 0.643832F, 0.639124F, 0.634393F, 0.629638F, 0.624859F, 0.620057F, 0.615232F, 0.610383F, 0.605511F, 0.600616F, 0.595699F, 0.590760F, 0.585798F, 0.580814F, 0.575808F, 0.570781F, 0.565732F, 0.560662F, 0.555570F, 0.550458F, 0.545325F, 0.540171F, 0.534998F, 0.529804F, 0.524590F, 0.519356F, 0.514103F, 0.508830F, 0.503538F, 0.498228F, 0.492898F, 0.487550F, 0.482184F, 0.476799F, 0.471397F, 0.465976F, 0.460539F, 0.455084F, 0.449611F, 0.444122F, 0.438616F, 0.433094F, 0.427555F, 0.422000F, 0.416430F, 0.410843F, 0.405241F, 0.399624F, 0.393992F, 0.388345F, 0.382683F, 0.377007F, 0.371317F, 0.365613F, 0.359895F, 0.354164F, 0.348419F, 0.342661F, 0.336890F, 0.331106F, 0.325310F, 0.319502F, 0.313682F, 0.307850F, 0.302006F, 0.296151F, 0.290285F, 0.284408F, 0.278520F, 0.272621F, 0.266713F, 0.260794F, 0.254866F, 0.248928F, 0.242980F, 0.237024F, 0.231058F, 0.225084F, 0.219101F, 0.213110F, 0.207111F, 0.201105F, 0.195090F, 0.189069F, 0.183040F, 0.177004F, 0.170962F, 0.164913F, 0.158858F, 0.152797F, 0.146730F, 0.140658F, 0.134581F, 0.128498F, 0.122411F, 0.116319F, 0.110222F, 0.104122F, 0.098017F, 0.091909F, 0.085797F, 0.079682F, 0.073565F, 0.067444F, 0.061321F, 0.055195F, 0.049068F, 0.042938F, 0.036807F, 0.030675F, 0.024541F, 0.018407F, 0.012272F, 0.006136F };

/* HOW TO GENERATE THE TABLE
#define M_TWOPI 6.283185307179586476925286766559
printf("const float sin_table[512] = { ");
for (int i = 0; i < 512; i++) printf("%fF, ", sin(M_TWOPI * i / 1024));
printf("}");
 */

void wave_gen(float *out, char t, uint32_t i, float tone) {

    //float period_f = 48000.F / tone;
    //uint32_t period_i = period_f;
    //uint32_t table_index = (i * 1024 / period_i) % 1024;

	float mid = ((float)i * 1024.F * tone / 48000.F);

	uint32_t table_index = (uint32_t)mid % 1024;

    if (t == 's') {
        if (table_index < 512) {
        	*out = sin_table[table_index];
        } else {
            *out = -sin_table[table_index - 512];
        }
    } else if (t == 'q') {
        if (table_index < 512) {
        	*out = -1.F;
        } else {
        	*out = 1.F;
        }
    }
}

#endif

