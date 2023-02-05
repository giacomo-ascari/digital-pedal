/*
 * pedalboard.h
 *
 *  Created on: 15 mag 2022
 *      Author: giaco
 */

#ifndef PEDALBOARD_PEDALBOARD_H_
#define PEDALBOARD_PEDALBOARD_H_

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
    AMPLIFIER,  // AMP
    BITCRUSHER_RS,  // BITRS
	BITCRUSHER_RT,  // BITRT
    BYPASS,         //  -
    FUZZ,           // FZZ
	HPF,			// HPF
    LPF,            // LPF
	NOISE_GATE,		// NGT
    OVERDRIVE,      // OVR
    OVERDRIVE_SQRT, // OVRS
    TREMOLO,        // TRM
	WAVE_GEN,		// WAV
};

//|	NAME			| name	| custom	| util		| custom	| custom		| custom	| custom	| custom	| custom	| util		| util		|
//|-----------------|-------| INT-0 	| INT-1 	| FLT-0 	| FLT-1 		| FLT-2 	| FLT-3 	| FLT-4 	| FLT-5 	| FLT-6 	| FLT-7		|
//| AMPLIFIER		| AMP	|			| counter	| gain		| high thr. 	|			|			| bal. in	| bal. out	| past in	| past out  |
//| BITCRUSHER_RS	| BITRS	| reduction	| counter	|			|				|			|			| bal. in	| bal. out	| past in	| past out  |
//| BITCRUSHER_RT	| BITRT	| reduction	| counter	|			|				|			|			| bal. in	| bal. out	| past in	| past out  |
//| BYPASS			|		|			| counter	|			|				|			|			|			|			| past in	| past out  |
//| FUZZ			| FZZ	|			| counter	| gain		| high thr.		| height	| speed		| bal. in	| bal. out	| past in	| past out  |
//| HPF				| HPF	|			| counter	|			|				|			| softener	| bal. in	| bal. out	| past in	| past out  |
//| LPF				| LPF	|			| counter	|			|				|			| softener	| bal. in	| bal. out	| past in	| past out  |
//| NOISE_GATE		| NGT	|			| counter	|			|				| threshold	| release	| bal. in	| bal. out	| past in	| past out  |
//| OVERDRIVE		| OVR	|			| counter	| gain		| high thr. 	| low thr.	| softener  | bal. in	| bal. out	| past in	| past out  |
//| OVERDRIVE_SQRT	| OVRS	|			| counter	| gain		| high thr. 	|			| softener  | bal. in	| bal. out	| past in	| past out  |
//| TREMOLO			| TRM	|			| counter	|			|				| height	| speed		| bal. in	| bal. out	| past in	| past out  |
//| WAGE GE			| WAV	|			| counter	|			|				| height	| speed		| bal. in	| bal. out	| past in	| past out  |

#define INT_PARAM_TYPES 2

enum int_param_type {
    COUNTER,            // multipurpose counter
    REDUCTION,   		// reduction intensity
};

#define FLOAT_PARAM_TYPES 9

enum float_param_type {
    INTENSITY,          // gain intensity
    THRESHOLD_HIGH,     // high (e.g. clip) threshold
    THRESHOLD_LOW,      // low (e.g. soft) threshold
    SOFTENER,           // softener
    BALANCE_IN,         // gain on input channel
    BALANCE_OUT,        // gain on output channel
    HEIGHT,             // height
    SPEED,              // speed
    PAST,               // past
};

#define TOTAL_PARAM_TYPES (INT_PARAM_TYPES + FLOAT_PARAM_TYPES)

// PARAMETERS structs

typedef struct _int_params_manifest_t {
    uint8_t active;
    char name[24];
    int32_t def;
    int32_t min;
    int32_t max;
    uint8_t qual;
} int_params_manifest_t;

typedef struct _float_params_manifest_t {
    uint8_t active;
    char name[24];
    float def;
    float min;
    float max;
    uint8_t qual;
} float_params_manifest_t;

enum param_qualifier {
	FREQUENCY,
	PERCENTAGE,
	VALUE
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
