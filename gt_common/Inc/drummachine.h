/*
 * drummachine.h
 *
 *  Created on: 26 apr 2023
 *      Author: Giacomo
 */

#ifndef INC_DRUMMACHINE_H_
#define INC_DRUMMACHINE_H_

#include <stdio.h>
#include <stdlib.h>


/*	/	-------------------------------------	\
	|			SOUNDS (samples)				|
	\	_____________________________________	/	*/

#define SOUNDS_COUNT 10

enum drum_sound {
	PAUSE = 0,
    KICK,		// cassa
	SNARE,		// rullante
	HIHAT_O,	// charleston aperto
	HIHAT_C,	// charleston chiuso
	RIDE,		// ride
	CRASH,		// crash
	COWBELL,	// campanaccio
	TOM1,		// tom (basso)
	TOM2,		// tom (acuto)
};

#ifdef F407

typedef struct _drum_sound_manifest_t {
    uint32_t length;
    const int16_t *wave;
} drum_sound_manifest_t;

extern const drum_sound_manifest_t drum_sounds_manifest[SOUNDS_COUNT];

#endif


/*	/	-------------------------------------	\
	|			VALUE CONSTRAINTS				|
	\	_____________________________________	/	*/

// max and min for 24 bit samples
#define MAX_VAL 8388608.F
#define MIN_VAL -8388607.F
#define ILLEGAL_VAL 8388609.F


/*	/	-------------------------------------	\
	|			NOTES AND PRESETS				|
	\	_____________________________________	/	*/

#define PRESETS_COUNT 10
#define MAX_NOTES_PER_VOICE 48
#define VOICES_COUNT 4

#define WHOLE			1.F
#define HALF			0.5F
#define THIRD 			0.33333F
#define QUARTER			0.25F
#define FIFTH			0.2F
#define SIXTH			0.16667F
#define EIGHTH			0.125F
#define SIXTEENTH		0.0625F
#define THIRTYSECOND	0.03125F

typedef struct _note_t {
	uint8_t sound;
	float duration;
} note_t;

typedef struct _voice_t {
	uint8_t notes_count;
	uint8_t bars_count;
	note_t notes[MAX_NOTES_PER_VOICE];
} voice_t;

typedef struct _drum_preset_manifest_t {
    char name[25]; //24+null
    uint8_t beats_per_bar;
    float default_bpm;
#ifdef F407
    voice_t voices[VOICES_COUNT];
#endif
} drum_preset_manifest_t;

extern const drum_preset_manifest_t drum_presets_manifest[PRESETS_COUNT];


/*	/	-------------------------------------	\
	|			DRUM CONFIG						|
	\	_____________________________________	/	*/

#define DRUM_INT_PARAMS_COUNT 1
#define DRUM_FLOAT_PARAMS_COUNT 2

#define DRUMS_PARAMS_COUNT (DRUM_INT_PARAMS_COUNT+DRUM_FLOAT_PARAMS_COUNT)

#define DRUMS_CONF_SIZE DRUMS_PARAMS_COUNT * 4 + 1 + 4

typedef struct _drum_conf_t {
	uint8_t active;
	int32_t int_params[DRUM_INT_PARAMS_COUNT];
	float float_params[DRUM_FLOAT_PARAMS_COUNT];
	uint32_t counter;
} drum_conf_t;

void Drum_Init(drum_conf_t *conf);

void Drum_Playback(float *value, drum_conf_t *conf);

void Drums_GetParamsByType(uint8_t active_index, uint8_t *_int, uint8_t *i);

/*	/	-------------------------------------	\
	|			PARAMS (manifest)				|
	\	_____________________________________	/	*/

#define DIP0 0
#define DFP0 0
#define DFP1 1

typedef struct _drum_int_params_manifest_t {
    char name[9]; //8+null
    int32_t def;
    int32_t min;
    int32_t max;
    uint8_t qual;
    int32_t micro_step;
    int32_t macro_step;
} drum_int_params_manifest_t;

typedef struct _drum_float_params_manifest_t {
    char name[9]; //8+null
    float def;
    float min;
    float max;
    uint8_t qual;
    float micro_step;
    float macro_step;
} drum_float_params_manifest_t;

enum drum_param_qualifier {
	DRUM_VALUE,			// whatever m8			int and float
	DRUM_DB,			// expressed in db		float
	DRUM_PS
};

typedef struct _drum_params_manifest_t {
    drum_int_params_manifest_t int_params_manifest[DRUM_INT_PARAMS_COUNT];
	drum_float_params_manifest_t float_params_manifest[DRUM_FLOAT_PARAMS_COUNT];
} drum_params_manifest_t;

extern const drum_params_manifest_t drum_params_manifest;

#endif /* INC_DRUMMACHINE_H_ */
