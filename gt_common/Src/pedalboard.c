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

#ifdef F407
#include "main.h" // to get arm_math
#endif //F407

// DSP FUNCTIONS/MACROS

#define CACHE_SIZE 5
float cached_log10f(float x, uint8_t slot);
float cached_exp10f(float x, uint8_t slot);

#define MIX(A, B, VolumeA, VolumeB) ((A)*(VolumeA)+(B)*(VolumeB))

// NOTES

// https://en.wikipedia.org/wiki/Piano_key_frequencies
// from c0 to b5
const float notes[SEMITONES*OCTAVES] = {
		// C		C# Db		D			D# Eb		E			F			F# Gb		G			G# Ab		A			A# Bb		B
		16.3516,	17.3239,	18.3541,	19.4454,	20.6017,	21.8268,	23.1247,	24.4997,	25.9565,	27.5000,	29.1352,	30.8677,
		32.7032,	34.6478,	36.7081,	38.8909,	41.2034,	43.6535,	46.2493,	48.9994,	51.9131,	55.0000,	58.2705,	61.7354,
		65.4064,	69.2957,	73.4162,	77.7818,	82.4069,	87.3071,	92.4986,	97.9989,	103.8262,	110.0000,	116.5409,	123.4708,
		130.8128,	138.5913,	146.8324,	155.5635,	164.8138,	174.6141,	184.9972,	195.9977,	207.6523,	220.0000,	233.0819,	246.9417,
		261.6256,	277.1826,	293.6648,	311.1270,	329.6276,	349.2282,	369.9944,	391.9954,	415.3047, 	440.0000,	466.1638,	493.8833,
		523.2511,	554.3653,	587.3295,	622.2540,	659.2551,	698.4565,	739.9888,	783.9909,	830.6094,	880.0000,	932.3275,	987.7666,
		1046.5020,	1108.7310,	1174.6590,	1244.5080,	1318.5100,	1396.9130,	1479.9780,	1567.9820,	1661.2190,	1760.0000,	1864.6550,	1975.5330
};

const char note_manifest[SEMITONES*OCTAVES][7] = {
		// C	C# Db		D		D# Eb		E		F		F# Gb		G		G# Ab		A		A# Bb		B
		"C0",	"C#/D,0",	"D0",	"D#/E,0",	"E0",	"F0",	"F#/G,0",	"G0",	"G#/A,0",	"A0",	"A#/B,0",	"B0",
		"C1",	"C#/D,1",	"D1",	"D#/E,1",	"E1",	"F1",	"F#/G,1",	"G1",	"G#/A,1",	"A1",	"A#/B,1",	"B1",
		"C2",	"C#/D,2",	"D2",	"D#/E,2",	"E2",	"F2",	"F#/G,2",	"G2",	"G#/A,2",	"A2",	"A#/B,2",	"B2",
		"C3",	"C#/D,3",	"D3",	"D#/E,3",	"E3",	"F3",	"F#/G,3",	"G3",	"G#/A,3",	"A3",	"A#/B,3",	"B3",
		"C4",	"C#/D,4",	"D4",	"D#/E,4",	"E4",	"F4",	"F#/G,4",	"G4",	"G#/A,4",	"A4",	"A#/B,4",	"B4",
		"C5",	"C#/D,5",	"D5",	"D#/E,5",	"E5",	"F5",	"F#/G,5",	"G5",	"G#/A,5",	"A5",	"A#/B,5",	"B5",
		"C6",	"C#/D,6",	"D6",	"D#/E,6",	"E6",	"F6",	"F#/G,6",	"G6",	"G#/A,6",	"A6",	"A#/B,6",	"B6"
};

// AMPLIFIER

#define AMP_GAIN FP0

void amp_process(float *value, effect_config_t *conf) {
#ifdef F407
    *value *= cached_exp10f(conf->float_params[AMP_GAIN] * 0.1F, 0); // pow cache slot 0
#endif
}

// BITCRUSHER resolution

#define BRS_REDUCT IP0
#define BRS_LEVEL_DRY FP0
#define BRS_LEVEL_WET FP1

void brs_process(float *value, effect_config_t *conf) {
#ifdef F407
	float dry = *value;
    int32_t shift = conf->int_params[BRS_REDUCT];
    int32_t _out = (int32_t)*value;
    _out = (_out >> shift);
    _out = (_out << shift);
    _out += (2 << (shift -2));
    *value = _out;
    *value = MIX(dry, *value, conf->float_params[BRS_LEVEL_DRY], conf->float_params[BRS_LEVEL_WET]);
#endif
}

// BITCRUSHER rate

#define BRT_DIVIDER IP0
#define BRT_LEVEL_DRY FP0
#define BRT_LEVEL_WET FP1
//util
#define BRT_COUNTER IP1
#define BRT_PAST_DRY FP2

void brt_process(float *value, effect_config_t *conf) {
#ifdef F407
	float dry = *value;
	// keep the same value until n of ticks
	if (conf->int_params[BRT_COUNTER] % conf->int_params[BRT_DIVIDER] == 0) {
		conf->float_params[BRT_PAST_DRY] = *value;
	} else {
		*value = conf->float_params[BRT_PAST_DRY];
	}
	conf->int_params[BRT_COUNTER]++;
    *value = MIX(dry, *value, conf->float_params[BRT_LEVEL_DRY], conf->float_params[BRT_LEVEL_WET]);
#endif
}

// BYPASS

void byp_process(float *value, effect_config_t *conf) {}

// COMPRESSOR

void cmp_process(float *value, effect_config_t *conf) {
#ifdef F407

#endif
}

// FUZZ

void fzz_process(float *value, effect_config_t *conf) {
#ifdef F407
	//float dry = *value;
	// distort and clip the hell out of it
	//*value = MIX(dry, *value, conf->float_params[FZZ_LEVEL_DRY], conf->float_params[FZZ_LEVEL_WET]);
#endif
}

// HPF

#define HPF_ATTACK FP0
//util
#define HPF_PAST_DRY FP1
#define HPF_PAST_WET FP2

void hpf_process(float *value, effect_config_t *conf) {
#ifdef F407
	float dry = *value;
	// alpha is attack
	*value = (1.0 - conf->float_params[HPF_ATTACK]) * (conf->float_params[HPF_PAST_WET] + *value - conf->float_params[HPF_PAST_DRY]);
	conf->float_params[HPF_PAST_DRY] = dry;
    conf->float_params[HPF_PAST_WET] = *value;
#endif
}

// LPF

#define LPF_ATTACK FP0
//util
#define LPF_PAST_WET FP1

void lpf_process(float *value, effect_config_t *conf) {
#ifdef F407
	// alpha is attack
	*value = conf->float_params[LPF_PAST_WET] + (1.0 - conf->float_params[LPF_ATTACK]) * (*value - conf->float_params[LPF_PAST_WET]);
    conf->float_params[LPF_PAST_WET] = *value;
#endif
}

// NOISE GATE

#define NGT_INTENSITY FP0
#define NGT_THRESHOLD FP1
#define NGT_HOLD IP0
//util
#define NGT_COUNTER IP1

void ngt_process(float *value, effect_config_t *conf) {
#ifdef F407
	if (fabsf(*value) < MAX_VAL * conf->float_params[NGT_THRESHOLD]) {
    	conf->int_params[NGT_COUNTER]++;
    } else {
    	conf->int_params[NGT_COUNTER] = 0;
    }
	// x / 48 = y
	// (x / a) * (a / 48) = y
	// x * (a/48) / a = y
	// se a pot. di due allora bitshift e non si fanno divisioni
	// 4096/48 = 85.333
	// 4096 = 2^12
	if ((conf->int_params[NGT_COUNTER] * 85 >> 12) > conf->int_params[NGT_HOLD]) {
		*value *=  (1.0 - conf->float_params[NGT_INTENSITY]);
	}
#endif
}

// OVERDRIVE

void ovr_process(float *value, effect_config_t *conf) {
#ifdef F407
	//float dry = *value;
	// distort and clip the hell out of it
	//mix(&dry, value, value, conf);
#endif
}

// TREMOLO

void trm_process(float *value, effect_config_t *conf) {
#ifdef F407
	//float tone = 440.0F;
    //wave_gen(value, 's', conf->int_params[COUNTER], tone * conf->float_params[SPEED]);
    //conf->int_params[COUNTER]++;
#endif
}

// WAVE GEN

#define WAV_LEVEL FP0
#define WAV_LEVEL_DRY FP1
#define WAV_LEVEL_WET FP2
#define WAV_KEY IP0
//util
#define WAV_COUNTER IP1
#define TWO_PI 6.28318530717958F
#ifdef F407
void wave_gen(float *out, char t, uint32_t i, float tone);
#endif

void wav_process(float *value, effect_config_t *conf) {
#ifdef F407
	float wave = 0.F;
	// 0.000130899694F = 2 * PI / 48000
	// w=freq*2*pi, t=counter/48000
	// y=sin(wt)
	wave = arm_sin_f32(notes[conf->int_params[WAV_KEY]] * conf->int_params[WAV_COUNTER] * 0.000130899694F); // fmodf is stupid, glicthes on 2:25
	wave *= (conf->float_params[WAV_LEVEL] * MAX_VAL);
	conf->int_params[WAV_COUNTER]++;
	*value = MIX(*value, wave, conf->float_params[WAV_LEVEL_DRY], conf->float_params[WAV_LEVEL_WET]);
#endif
}

// CACHED MATH.H

float cached_log10f(float x, uint8_t slot) {
	static uint8_t first_time = 1;
	static float cache_x[CACHE_SIZE];
	static float cache_y[CACHE_SIZE];
	if (first_time) {
		first_time = 0;
		for(uint8_t i = 0; i < CACHE_SIZE; i++) {
			cache_x[i] = ILLEGAL_VAL;
		}
	}
	if (slot < CACHE_SIZE && x == cache_x[slot]) {
		return cache_y[slot];
	} else {
		cache_y[slot] = log10f(x);
		cache_x[slot] = x;
		return cache_y[slot];
	}
}

float cached_exp10f(float x, uint8_t slot) {
	static uint8_t first_time = 1;
	static float cache_x[CACHE_SIZE];
	static float cache_y[CACHE_SIZE];
	if (first_time) {
		first_time = 0;
		for(uint8_t i = 0; i < CACHE_SIZE; i++) {
			cache_x[i] = ILLEGAL_VAL;
		}
	}
	if (slot < CACHE_SIZE && x == cache_x[slot]) {
		return cache_y[slot];
	} else {
		cache_y[slot] = expf(x*2.30258509F);
		cache_x[slot] = x;
		return cache_y[slot];
	}
}


// MANIFESTs
// ----------------------------------------------------------------------------------------------------- active, name, def, min, max, qual, micro s., macro s.

const effect_manifest_t Effects_Manifest[EFFECT_TYPES] = {
		[AMP] = (effect_manifest_t){"amp","amplifier"},
		[AMP].float_params_manifest[AMP_GAIN] = (float_params_manifest_t)			{ 1, "Gain", 0.0, -7.0, 7.0, DB, 0.1, 1 },
		[AMP].effect_process = amp_process,

		[BRS] = (effect_manifest_t){"brs","bitcrusher rs"},
		[BRS].int_params_manifest[BRS_REDUCT] = (int_params_manifest_t)				{ 1, "Bit reduction", 4, 1, 16, VALUE, 1, 5 },
		[BRS].float_params_manifest[BRS_LEVEL_DRY] = (float_params_manifest_t)		{ 1, "Level dry", 0.5, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[BRS].float_params_manifest[BRS_LEVEL_WET] = (float_params_manifest_t)		{ 1, "Level wet", 0.5, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[BRS].effect_process = brs_process,

		[BRT] = (effect_manifest_t){"brt","bitcrusher rt"},
		[BRT].int_params_manifest[BRT_DIVIDER] = (int_params_manifest_t)			{ 1, "Rate divider", 4, 1, 24, VALUE, 1, 5 },
		[BRT].float_params_manifest[BRT_LEVEL_DRY] = (float_params_manifest_t)		{ 1, "Level dry", 0.5, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[BRT].float_params_manifest[BRT_LEVEL_WET] = (float_params_manifest_t)		{ 1, "Level wet", 0.5, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[BRT].effect_process = brt_process,

		[BYP] = (effect_manifest_t){"byp","bypass"},
		[BYP].effect_process = byp_process,

		[CMP] = (effect_manifest_t){"cmp","compressor"},
		[CMP].effect_process = cmp_process,

		[FZZ] = (effect_manifest_t){"fzz","fuzz"},
		[FZZ].effect_process = fzz_process,

		[HPF] = (effect_manifest_t){"hpf","high pass filter"},
		[HPF].float_params_manifest[HPF_ATTACK] = (float_params_manifest_t)			{ 1, "Attack", 0.1, 0.0, 1.0, VALUE, 0.01, 0.1 },
		[HPF].effect_process = hpf_process,

		[LPF] = (effect_manifest_t){"lpf","low pass filter"},
		[LPF].float_params_manifest[LPF_ATTACK] = (float_params_manifest_t)			{ 1, "Attack", 0.1, 0.0, 1.0, VALUE, 0.01, 0.1 },
		[LPF].effect_process = lpf_process,

		[NGT] = (effect_manifest_t){"ngt","noise gate"},
		[NGT].float_params_manifest[NGT_THRESHOLD] = (float_params_manifest_t)		{ 1, "Threshold", 0.1, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[NGT].float_params_manifest[NGT_INTENSITY] = (float_params_manifest_t)		{ 1, "Intensity", 0.5, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[NGT].int_params_manifest[NGT_HOLD] = (int_params_manifest_t)				{ 1, "Hold", 500, 0, 5000, TIME, 10, 100 },
		[NGT].effect_process = ngt_process,

		[OVR] = (effect_manifest_t){"ovr","overdrive"},
		[OVR].effect_process = ovr_process,

		[TRM] = (effect_manifest_t){"trm","tremolo"},
		[TRM].effect_process = trm_process,

		[WAV] = (effect_manifest_t){"wav","wave gen."},
		[WAV].float_params_manifest[WAV_LEVEL] = (float_params_manifest_t)			{ 1, "Amplitude", 0.1, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[WAV].float_params_manifest[WAV_LEVEL_DRY] = (float_params_manifest_t)		{ 1, "Level dry", 0.5, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[WAV].float_params_manifest[WAV_LEVEL_WET] = (float_params_manifest_t)		{ 1, "Level wet", 0.5, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[WAV].int_params_manifest[WAV_KEY] = (int_params_manifest_t)				{ 1, "Key", 57, 0, SEMITONES*OCTAVES-1, KEY, 1, 12 },
		[WAV].effect_process = wav_process
};

// OTHER

const char mode_manifest[OUTPUT_MODE_TYPES][MODE_STRING_SIZE] = {
		[TS] = {"ts"},
		[RS] = {"rs"},
		[TRS_B] = {"trs bal."},
		[TRS_UB] = {"trs unb."}
};

// PEDALBOARD

void Pedalboard_Init(Pedalboard_Handler *p_pb) {
	p_pb->active = 1;
	p_pb->input_mode = TS;
	p_pb->output_mode = TS;
	for (u_int8_t i = 0; i < EFFECT_SLOTS_COUNT; i++) {
		p_pb->effects[i].type = BYP;
	}
}

void Pedalboard_SetEffect(Pedalboard_Handler *p_pb, uint8_t type, uint8_t i) {
    if (i < EFFECT_SLOTS_COUNT && type < EFFECT_TYPES) {
    	p_pb->effects[i].type = BYP;
    	for (uint8_t j = 0; j < INT_PARAMS_COUNT; j++) {
    		if (Effects_Manifest[type].int_params_manifest[j].active) {
    			p_pb->effects[i].config.int_params[j] = Effects_Manifest[type].int_params_manifest[j].def;
    		} else {
    			p_pb->effects[i].config.int_params[j] = 0;
    		}
    	}
    	for (uint8_t j = 0; j < FLOAT_PARAMS_COUNT; j++) {
    		if (Effects_Manifest[type].float_params_manifest[j].active) {
				p_pb->effects[i].config.float_params[j] = Effects_Manifest[type].float_params_manifest[j].def;
			} else {
				p_pb->effects[i].config.float_params[j] = 0.0;
			}
    	}
        p_pb->effects[i].type = type;
    }
}

void Pedalboard_DeleteEffect(Pedalboard_Handler *p_pb, uint8_t i) {
	if (i < EFFECT_SLOTS_COUNT) {
		p_pb->effects[i].type = BYP;
	}
}

void Pedalboard_Process(Pedalboard_Handler *p_pb, float *value) {
#ifdef F407
    if (p_pb->active) {
    	for (uint8_t i = 0; i < EFFECT_SLOTS_COUNT; i++) {
    		uint8_t type = p_pb->effects[i].type;
    		if (type != BYP) {
    			Effects_Manifest[type].effect_process(value, &(p_pb->effects[i].config));
    		}
        }
    	if (*value > MAX_VAL) {
			*value = MAX_VAL;
		} else if (*value < MIN_VAL) {
			*value = MIN_VAL;
		}
    }
#endif
}

uint8_t Pedalboard_CountActiveParams(Pedalboard_Handler *p_pb, uint8_t i) {
	uint8_t active_params = 0;
	if (i < EFFECT_SLOTS_COUNT) {
		active_params = Pedalboard_CountActiveParamsByType(p_pb->effects[i].type);
	}
	return active_params;
}

uint8_t Pedalboard_CountActiveParamsByType(uint8_t type) {
	uint8_t active_params = 0;
	for (uint8_t j = 0; j < INT_PARAMS_COUNT; j++) {
		if (Effects_Manifest[type].int_params_manifest[j].active) {
			active_params++;
		}
	}
	for (uint8_t j = 0; j < FLOAT_PARAMS_COUNT; j++) {
		if (Effects_Manifest[type].float_params_manifest[j].active) {
			active_params++;
		}
	}
	return active_params;
}

void Pedalboard_GetActiveParamsByType(uint8_t active_index, uint8_t type, uint8_t *_int, uint8_t *i) {
	uint8_t active_params = 0;
	for (uint8_t j = 0; j < INT_PARAMS_COUNT; j++) {
		if (Effects_Manifest[type].int_params_manifest[j].active) {
			if (active_params == active_index) {
				*_int = 1;
				*i = j;
				return;
			}
			active_params++;
		}
	}
	for (uint8_t j = 0; j < FLOAT_PARAMS_COUNT; j++) {
		if (Effects_Manifest[type].float_params_manifest[j].active) {
			if (active_params == active_index) {
				*_int = 0;
				*i = j;
				return;
			}
			active_params++;
		}
	}
}
