/*
 * pedalboard.h
 *
 *  Created on: 15 mag 2022
 *      Author: giaco
 */

#ifndef PEDALBOARD_PEDALBOARD_H_
#define PEDALBOARD_PEDALBOARD_H_

// GENERIC STUFF

// max and min for 24 bit samples
#define MAX_VAL 8388608.0
#define MIN_VAL -8388607.0

#define EFFECT_SLOTS_COUNT 6

// INPUT AND OUTPUT MODES (LEFT, RIGHT, BALANCING ETC.)

#define INPUT_MODE_TYPES 2
#define OUTPUT_MODE_TYPES 4
#define MODE_STRING_SIZE 8+1

enum mode_type {
	TS = 0, //left
	RS, //right
	TRS_B,
	TRS_UB
};

extern const char mode_manifest[OUTPUT_MODE_TYPES][MODE_STRING_SIZE];

// EFFECTS TYPES AND MANIFEST

#define EFFECT_TYPES 12

enum effect_type {
    AMP,
    BRS,
	BRT,
    BYP,
	CMP,
    FZZ,
	HPF,
    LPF,
	NGT,
    OVR,
    TRM,
	WAV
};

#define INT_PARAMS_COUNT 2
#define FLOAT_PARAMS_COUNT 6

enum int_param_raw {
    IP0 = 0,
	IP1 = 1
};

enum float_param_raw {
    FP0 = 0,
	FP1 = 1,
	FP2 = 2,
	FP3 = 3,
	FP4 = 4,
	FP5 = 5,
};

#define TOTAL_PARAMS_COUNT (INT_PARAMS_COUNT + FLOAT_PARAMS_COUNT)

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

// EFFECT structs

#define RAW_EFFECT_SIZE (1 + 4 * TOTAL_PARAMS_COUNT)

typedef struct _effect_config_t {
	int32_t int_params[INT_PARAMS_COUNT];
	float float_params[FLOAT_PARAMS_COUNT];
} effect_config_t;

typedef struct _effect_t {
	uint8_t type;
	effect_config_t config;
} effect_t;

typedef struct _effect_manifest_t {
    char short_name[8];
    char long_name[24];
    int_params_manifest_t int_params_manifest[INT_PARAMS_COUNT];
	float_params_manifest_t float_params_manifest[FLOAT_PARAMS_COUNT];
    void (*effect_process)(float *value, effect_config_t *p_config);
} effect_manifest_t;

extern const effect_manifest_t Effects_Manifest[EFFECT_TYPES];

// PEDALBOARD

#define PEDALBOARD_HANDLER_SIZE (RAW_EFFECT_SIZE * EFFECT_SLOTS_COUNT + 1 + 1 + 1)

typedef struct _Pedalboard_Handler {
	uint8_t input_mode;
	uint8_t output_mode;
	uint8_t active;
	effect_t effects[EFFECT_SLOTS_COUNT];
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
