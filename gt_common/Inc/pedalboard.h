/*
 * pedalboard.h
 *
 *  Created on: 15 mag 2022
 *      Author: giaco
 */

#ifndef PEDALBOARD_PEDALBOARD_H_
#define PEDALBOARD_PEDALBOARD_H_


/*	/	-------------------------------------	\
	|			VALUE CONSTRAINTS				|
	\	_____________________________________	/	*/

// max and min for 24 bit samples
#define MAX_VAL 8388608.F
#define MIN_VAL -8388607.F
#define ILLEGAL_VAL 8388609.F


/*	/	-------------------------------------	\
	|			MUSICAL NOTES					|
	\	_____________________________________	/	*/

#define SEMITONES 12
#define OCTAVES 7

extern const float notes[SEMITONES*OCTAVES];
extern const char note_manifest[SEMITONES*OCTAVES][7];


/*	/	-------------------------------------	\
	|			PARAMS (effects config)			|
	\	_____________________________________	/	*/

#define EFFECT_SLOTS_COUNT 6

#define EFFECT_TYPES 10

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
	WAV
};

#define INT_PARAMS_COUNT 3
#define FLOAT_PARAMS_COUNT 4

enum int_param_raw {
    IP0 = 0,
	IP1 = 1,
	IP2 = 2
};

enum float_param_raw {
    FP0 = 0,
	FP1 = 1,
	FP2 = 2,
	FP3 = 3
};

#define TOTAL_PARAMS_COUNT (INT_PARAMS_COUNT + FLOAT_PARAMS_COUNT)

#define RAW_EFFECT_SIZE (1 + 4 * TOTAL_PARAMS_COUNT)

typedef struct _effect_config_t {
	int32_t int_params[INT_PARAMS_COUNT];
	float float_params[FLOAT_PARAMS_COUNT];
} effect_config_t;

typedef struct _effect_t {
	uint8_t type;
	effect_config_t config;
} effect_t;


/*	/	-------------------------------------	\
	|			EFFECTS/PARAMS (manifest)		|
	\	_____________________________________	/	*/

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
	FREQUENCY = 0,	// always in Hz			int and float
	PERCENTAGE,		// should be [0,1]		float
	VALUE,			// whatever m8			int and float
	TIME,			// always in ms			int
	DB,				// expressed in db		float
	KEY				// musical note			int
};

typedef struct _effect_manifest_t {
    char short_name[8];
    char long_name[24];
    int_params_manifest_t int_params_manifest[INT_PARAMS_COUNT];
	float_params_manifest_t float_params_manifest[FLOAT_PARAMS_COUNT];
	void (*effect_process)(float *value, effect_config_t *p_config, uint8_t *slot_index);
} effect_manifest_t;

extern const effect_manifest_t Effects_Manifest[EFFECT_TYPES];


/*	/	-------------------------------------	\
	|			SLOT DEPENDENT CACHE			|
	\	_____________________________________	/	*/

#define INT_CACHE_COUNT 4
#define FLOAT_CACHE_COUNT 6

enum int_cache_raw {
    IC0 = 0,
	IC1 = 1,
	IC2 = 2,
	IC3 = 3
};
enum float_cache_raw {
    FC0 = 0,
	FC1 = 1,
	FC2 = 2,
	FC3 = 3,
	FC4 = 4,
	FC5 = 5
};

typedef struct _cache_t {
	int32_t int_params[INT_CACHE_COUNT];
	float float_params[FLOAT_CACHE_COUNT];
} cache_t;

typedef struct _caches_t {
	cache_t slots[EFFECT_SLOTS_COUNT];
} caches_t;


/*	/	-------------------------------------	\
	|			PEDALBOARD HANDLER				|
	\	_____________________________________	/	*/

#define PEDALBOARD_HANDLER_SIZE (RAW_EFFECT_SIZE * EFFECT_SLOTS_COUNT + 1 + 1 + 1)

typedef struct _Pedalboard_Handler {
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

#endif /* PEDALBOARD_PEDALBOARD_H_ */
