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

#define MIX(A, B, VolumeA, VolumeB) ((A)*(VolumeA)+(B)*(VolumeB))

// DECLARATIONS

void amp_process(float *value, effect_config_t *conf);
#define AMP_GAIN FP0

void brs_process(float *value, effect_config_t *conf);
#define BRS_REDUCT IP0
#define BRS_LEVEL_DRY FP0
#define BRS_LEVEL_WET FP1

void brt_process(float *value, effect_config_t *conf);
#define BRT_DIVIDER IP0
#define BRT_COUNTER IP1
#define BRT_LEVEL_DRY FP0
#define BRT_LEVEL_WET FP1
#define BRT_PAST_DRY FP2

void byp_process(float *value, effect_config_t *conf);
// nope

void cmp_process(float *value, effect_config_t *conf);
// wip

void fzz_process(float *value, effect_config_t *conf);
#define FZZ_LEVEL_DRY FP0
#define FZZ_LEVEL_WET FP1

void hpf_process(float *value, effect_config_t *conf);
void lpf_process(float *value, effect_config_t *conf);
void ngt_process(float *value, effect_config_t *conf);
void ovr_process(float *value, effect_config_t *conf);
void trm_process(float *value, effect_config_t *conf);
void wav_process(float *value, effect_config_t *conf);

void square_root(float *value);
void wave_gen(float *out, char t, uint32_t i, float tone);

// MANIFESTs
// ----------------------------------------------------------------------------------------------------- active, name, def, min, max, qual, micro s., macro s.

const effect_manifest_t Effects_Manifest[EFFECT_TYPES] = {
		[AMP] = (effect_manifest_t){"amp","amplifier"},
		[AMP].float_params_manifest[AMP_GAIN] = (float_params_manifest_t)			{ 1, "Gain", 0.0, 0.0, 5.0, VALUE, 0.05, 0.5 },
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
		[HPF].float_params_manifest[ATTACK] = (float_params_manifest_t)			{ 1, "Attack", 0.1, 0.0, 1.0, VALUE },
		[HPF].float_params_manifest[PAST_DRY] = (float_params_manifest_t)		{ 0, "", 0, 0, 0, VALUE, 0, 0 },
		[HPF].float_params_manifest[PAST_WET] = (float_params_manifest_t)		{ 0, "", 0, 0, 0, VALUE, 0, 0 },
		[HPF].effect_process = hpf_process,

		[LPF] = (effect_manifest_t){"lpf","low pass filter"},
		[LPF].float_params_manifest[ATTACK] = (float_params_manifest_t)			{ 1, "Attack", 0.1, 0.0, 1.0, VALUE },
		[LPF].float_params_manifest[PAST_DRY] = (float_params_manifest_t)		{ 0, "", 0.0, 0.0, 0.0, VALUE, 0, 0 },
		[LPF].float_params_manifest[PAST_WET] = (float_params_manifest_t)		{ 0, "", 0.0, 0.0, 0.0, VALUE, 0, 0 },
		[LPF].effect_process = lpf_process,

		[NGT] = (effect_manifest_t){"ngt","noise gate"},
		[NGT].float_params_manifest[THRESHOLD] = (float_params_manifest_t)		{ 1, "Threshold", 0.1, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[NGT].float_params_manifest[INTENSITY] = (float_params_manifest_t)		{ 1, "Intensity", 0.5, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[NGT].int_params_manifest[HOLD] = (int_params_manifest_t)				{ 1, "Hold", 500, 0, 5000, TIME, 10, 100 },
		[NGT].int_params_manifest[COUNTER] = (int_params_manifest_t) 			{ 0, "", 0, 0, 0, VALUE, 0, 0 },
		[NGT].effect_process = ngt_process,

		[OVR] = (effect_manifest_t){"ovr","overdrive"},
		[OVR].float_params_manifest[GAIN] = (float_params_manifest_t)			{ 1, "Gain", 4.0, 1.0, 10.0, VALUE },
		[OVR].float_params_manifest[LEVEL] = (float_params_manifest_t)			{ 1, "Level", 0.3, 0.0, 1.0, PERCENTAGE },
		[OVR].float_params_manifest[LEVEL_DRY] = (float_params_manifest_t)		{ 1, "Level dry", 0.F, 0.F, 1.F, PERCENTAGE, 0.01, 0.1 },
		[OVR].float_params_manifest[LEVEL_WET] = (float_params_manifest_t)		{ 1, "Level wet", 1.F, 0.F, 1.F, PERCENTAGE, 0.01, 0.1 },
		[OVR].effect_process = ovr_process,

		[TRM] = (effect_manifest_t){"trm","tremolo"},
		[TRM].float_params_manifest[SPEED] = (float_params_manifest_t)			{ 1, "Speed", 1, 0, 12, VALUE },
		[TRM].effect_process = trm_process,

		[WAV] = (effect_manifest_t){"wav","wave gen."},
		[WAV].float_params_manifest[LEVEL] = (float_params_manifest_t)			{ 1, "Amplitude", 0.1, 0.0, 0.5, PERCENTAGE },
		[WAV].float_params_manifest[SPEED] = (float_params_manifest_t)			{ 1, "Speed", 440, 110, 880, VALUE },
		[WAV].float_params_manifest[LEVEL_DRY] = (float_params_manifest_t)		{ 1, "Level dry", 0.5, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[WAV].float_params_manifest[LEVEL_WET] = (float_params_manifest_t)		{ 1, "Level wet", 0.5, 0.0, 1.0, PERCENTAGE, 0.01, 0.1 },
		[WAV].int_params_manifest[COUNTER] = (int_params_manifest_t) 			{ 0, "", 0, 0, 0, VALUE, 0, 0 },
		[WAV].effect_process = wav_process
};

// AMPLIFIER

void amp_process(float *value, effect_config_t *conf) {
#ifdef F407
	//update with decibels
    *value *= conf->float_params[AMP_GAIN];
#endif
}

// BITCRUSHER resolution

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

void brt_process(float *value, effect_config_t *conf) {
#ifdef F407
	float dry = *value;
	// keep the same value until n of ticks
	if (!conf->int_params[BRT_COUNTER] % conf->int_params[BRT_DIVIDER]) {
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
	float dry = *value;
	// distort and clip the hell out of it
	*value = MIX(dry, *value, conf->float_params[FZZ_LEVEL_DRY], conf->float_params[FZZ_LEVEL_WET]);
#endif
}

// HPF

void hpf_process(float *value, effect_config_t *conf) {
#ifdef F407
	float dry = *value;
	// alpha is attack
	*value = (1.0 - conf->float_params[ATTACK]) * (conf->float_params[PAST_WET] + *value - conf->float_params[PAST_DRY]);
	conf->float_params[PAST_DRY] = dry;
    conf->float_params[PAST_WET] = *value;
#endif
}

// LPF

void lpf_process(float *value, effect_config_t *conf) {
#ifdef F407
	//float dry = *value;
	// alpha is attack
	*value = conf->float_params[PAST_WET] + (1.0 - conf->float_params[ATTACK]) * (*value - conf->float_params[PAST_WET]);
	//conf->float_params[PAST_DRY] = dry;
    conf->float_params[PAST_WET] = *value;
#endif
}

// NOISE GATE

void ngt_process(float *value, effect_config_t *conf) {
#ifdef F407
	if (fabsf(*value) < MAX_VAL * conf->float_params[THRESHOLD]) {
    	conf->int_params[COUNTER]++;
    } else {
    	conf->int_params[COUNTER] = 0;
    }
	// x / 48 = y
	// (x / a) * (a / 48) = y
	// x * (a/48) / a = y
	// se a pot. di due allora bitshift e non si fanno divisioni
	// 4096/48 = 85.333
	// 4096 = 2^12
	if ((conf->int_params[COUNTER] * 85 >> 12) > conf->int_params[HOLD]) {
		*value *=  (1.0 - conf->float_params[INTENSITY]);
	}
#endif
}

// OVERDRIVE

void ovr_process(float *value, effect_config_t *conf) {
#ifdef F407
	float dry = *value;
	// distort and clip the hell out of it
	mix(&dry, value, value, conf);
#endif
}

// TREMOLO

void trm_process(float *value, effect_config_t *conf) {
#ifdef F407
	float tone = 440.0F;
    wave_gen(value, 's', conf->int_params[COUNTER], tone * conf->float_params[SPEED]);
    conf->int_params[COUNTER]++;
#endif
}

// WAVE GEN

void wav_process(float *value, effect_config_t *conf) {
#ifdef F407
	//arm_sin_f32(10.0 / (2.0 * 3.14159) * 48000.0);
	float wave = 0.0;
	wave_gen(&wave, 's', conf->int_params[COUNTER], conf->float_params[SPEED]);
	wave *= (conf->float_params[LEVEL] * MAX_VAL);
	conf->int_params[COUNTER]++;
	mix(&wave, value, value, conf);
#endif
}

// DSP

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
    if (i < EFFECT_SLOTS_COUNT) {
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
