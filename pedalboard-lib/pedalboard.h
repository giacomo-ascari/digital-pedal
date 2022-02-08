#ifndef _PEDALBOARD_H
#define _PEDALBOARD_H

#include <stdlib.h>

#define MAX_PEDALS_COUNT 16
#define U_INT_PARAM_TYPES 2
#define FLOAT_PARAM_TYPES 9

// ENUMERATION

enum pedal_types {
    AMPLIFIER,      // amp
    BITCRUSHER_RS,  // brs
    BYPASS,         // bps
    DYN_AMPLIFIER,  // damp
    FUZZ,           // fzz
    LPF,            // lpf
    OVERDRIVE,      // ovr
    OVERDRIVE_SQRT, // ovrs
    TREMOLO,        // trm
};

enum u_int_param_type {
    WIDTH,          // width
    COUNTER,        // multipurpose counter
};

enum float_param_type {
    GAIN_INTENSITY,     // gain intensity
    CLIP_THRESHOLD,     // clip threshold
    SOFT_THRESHOLD,     // soft threshold
    REDUCT_INTENSITY,   // reduction intensity
    SOFTENER,           // softener
    BALANCE_1,          // gain on primary channel
    BALANCE_2,          // gain on secondary channel
    HEIGHT,             // height
    SPEED,              // speed
};

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