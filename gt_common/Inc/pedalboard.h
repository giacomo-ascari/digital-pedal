/*
 * pedalboard.h
 *
 *  Created on: 15 mag 2022
 *      Author: giaco
 */

#ifndef PEDALBOARD_PEDALBOARD_H_
#define PEDALBOARD_PEDALBOARD_H_

// max and min for 24 bit samples
#define MAX_VAL 8388608.0
#define MIN_VAL -8388607.0

#define MAX_EFFECTS_COUNT 6

// ENUMERATION

#define MODE_TYPES 4

enum mode_type {
	TS = 0, // right
	RS = 1, // left
	TRS_B = 2,
	TRS_UB = 3
};

extern const char mode_manifest[MODE_TYPES][10];

#define EFFECT_TYPES 12

enum effect_type {
    AMPLIFIER,		// AMP
    BITCRUSHER_RS,  // BITRS
	BITCRUSHER_RT,  // BITRT
    BYPASS,         //  -
	COMPRESSOR,		// CMP
    FUZZ,           // FZZ
	HPF,			// HPF
    LPF,            // LPF
	NOISE_GATE,		// NGT
    OVERDRIVE,      // OVR
    TREMOLO,        // TRM
	WAVE_GEN,		// WAV
};

#define INT_PARAM_TYPES 4

enum int_param_type {
    REDUCTION,   		// reduction
	DIVIDER,			// divider
	HOLD,				// hold
	// utils
	COUNTER,            // counter
};

#define FLOAT_PARAM_TYPES 11

enum float_param_type {
	GAIN,				// gain, pre-gain, drive
	SPEED,			    // speed
    ATTACK,             // attack
	THRESHOLD,          // threshold
	INTENSITY,			// intensity
	RELEASE,            // release
	LEVEL,				// level, volume
	// mixing
    LEVEL_DRY,          // / mixing input (dry) and... output (wet) channel
	LEVEL_WET,          // \ ...output (wet) channel
    // utils
    PAST_DRY,               // past dry
	PAST_WET,               // past wet
};

#define TOTAL_PARAM_TYPES (INT_PARAM_TYPES + FLOAT_PARAM_TYPES)

// PARAMETERS structs

typedef struct _int_params_manifest_t {
    uint8_t active;
    char name[17]; //16+null
    int32_t def;
    int32_t min;
    int32_t max;
    uint8_t qual;
    int32_t micro_step;
    int32_t macro_step;
} int_params_manifest_t;

typedef struct _float_params_manifest_t {
    uint8_t active;
    char name[17]; //16+null
    float def;
    float min;
    float max;
    uint8_t qual;
    float micro_step;
    float macro_step;
} float_params_manifest_t;

enum param_qualifier {
	FREQUENCY,		// always in Hz
	PERCENTAGE,		// should be [0,1]
	VALUE,			// whatever m8
	TIME,			// always in ms
	DB				// expressed in db
};

typedef struct _params_manifest_t {
	int_params_manifest_t int_params_manifest[INT_PARAM_TYPES];
	float_params_manifest_t float_params_manifest[FLOAT_PARAM_TYPES];
} params_manifest_t;

// EFFECT structs

#define RAW_EFFECT_SIZE (1 + 4 * (TOTAL_PARAM_TYPES))

typedef struct _effect_config_t {
	int32_t int_params[INT_PARAM_TYPES];
	float float_params[FLOAT_PARAM_TYPES];
} effect_config_t;

typedef struct _effect_t {
	uint8_t type;
	effect_config_t config;
} effect_t;

typedef union _effect_union_t {
	effect_t effect_formatted;
	uint8_t effect_raw[RAW_EFFECT_SIZE];
} effect_union_t;

typedef struct _effect_manifest_t {
    char short_name[8];
    char long_name[24];
    params_manifest_t params_manifest;
    void (*effect_process)(float *value, effect_config_t *p_config);
} effect_manifest_t;

extern const effect_manifest_t Effects_Manifest[EFFECT_TYPES];

// PEDALBOARD

#define PEDALBOARD_HANDLER_SIZE (RAW_EFFECT_SIZE * MAX_EFFECTS_COUNT + 1 + 1 + 1)

typedef struct _Pedalboard_Handler {
	uint8_t input_mode;
	uint8_t output_mode;
	uint8_t active;
    effect_union_t effects[MAX_EFFECTS_COUNT];
} Pedalboard_Handler;

void Pedalboard_Init(Pedalboard_Handler *p_pb);
void Pedalboard_SetEffect(Pedalboard_Handler *p_pb, uint8_t type, uint8_t i);
void Pedalboard_DeleteEffect(Pedalboard_Handler *p_pb, uint8_t i);
void Pedalboard_Process(Pedalboard_Handler *p_pb, float *value);
uint8_t Pedalboard_CountActiveParams(Pedalboard_Handler *p_pb, uint8_t i);
uint8_t Pedalboard_CountActiveParamsByType(uint8_t type);
void Pedalboard_GetActiveParamsByType(uint8_t active_index, uint8_t type, uint8_t *_int, uint8_t *i);

void wave_gen(float *out, char t, uint32_t i, float tone);

#endif /* PEDALBOARD_PEDALBOARD_H_ */
