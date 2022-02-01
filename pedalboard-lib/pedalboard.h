#ifndef _PEDALBOARD_H
#define _PEDALBOARD_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_PEDALS_COUNT 16

#define PEDAL_TYPES 8
#define U_INT_PARAM_TYPES 1
#define FLOAT_PARAM_TYPES 8

// ENUMERATION

enum pedal_types {
    AMPLIFIER = 0,      // amp
    BITCRUSHER_RS = 1,  // brs
    BYPASS = 2,         // bps
    DYN_AMPLIFIER = 3,  // damp
    FUZZ = 4,           // fzz
    LPF = 5,            // lpf
    OVERDRIVE = 6,      // ovr
    OVERDRIVE_LOG = 7,  // ovrl
    TREMOLO = 8,        // trm
};

enum u_int_param_type {
    WIDTH = 0,          // width
};

enum float_param_type {
    GAIN_INTENSITY = 0,     // gain intensity
    CLIP_THRESHOLD = 1,     // clip threshold
    SOFT_THRESHOLD = 2,     // soft threshold
    REDUCT_INTENSITY = 3,   // reduction intensity
    SOFTENER = 4,           // softener
    BALANCE = 5,            // balance
    HEIGHT = 6,             // height
    SPEED = 7,              // speed
};

// PROCESSING

float gain(float in, float gain_intensity);
float mix(float in_1, float in_2, float balance);
float hard_clip(float in, float clip_threshold);
float soft_clip(float in, float soft_threshold, float softener);
float reduce_resolution(float in, float reduction_intensity);
float wave_gen(char t, u_int32_t i, float height, float speed);

// PARAMETERS structs _ DO NOT TOUCH

typedef struct _u_int_parameter_t {
    u_int32_t value, min, max, step;
} u_int_parameter_t;

typedef struct _float_parameter_t {
    float value, min, max, step;
} float_parameter_t;

// PEDALS structs _ DO NOT TOUCH

typedef struct _pedal_config_t {
    u_int_parameter_t u_int_params[U_INT_PARAM_TYPES];
    float_parameter_t float_params[FLOAT_PARAM_TYPES];
} pedal_config_t;

typedef struct _pedal_t {
    pedal_config_t config;
    enum pedal_types type;
    float (*pedal_process)(float in, pedal_config_t *p_config);
} pedal_t;

enum pedal_types pedal_type_parse(char *type_str);

// PEDALBOARD _ DO NOT TOUCH

typedef struct _pedalboard_t {
    u_int8_t active_pedals;
    pedal_t pedals[MAX_PEDALS_COUNT];
} pedalboard_t;

void pedalboard_append(pedalboard_t *p_pb, enum pedal_types type);
float pedalboard_process(pedalboard_t *p_pb, float in);

#endif