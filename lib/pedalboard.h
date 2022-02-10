#ifndef _PEDALBOARD_H
#define _PEDALBOARD_H

#include <stdlib.h>

#define MAX_PEDALS_COUNT 16

#define INT_PARAM_TYPES 3
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

enum int_param_type {
    WIDTH,              // width
    COUNTER,            // multipurpose counter
    REDUCT_INTENSITY,   // reduction intensity
};

enum float_param_type {
    INTENSITY,          // gain intensity
    THRESHOLD_HIGH,     // high (e.g. clip) threshold
    THRESHOLD_LOW,      // low (e.g. soft) threshold
    SOFTENER,           // softener
    BALANCE_1,          // gain on primary channel
    BALANCE_2,          // gain on secondary channel
    HEIGHT,             // height
    SPEED,              // speed
    PAST,              // past
};

// PARAMETERS structs _ DO NOT TOUCH

typedef struct _int_parameter_t {
    int32_t value, min, max, step;
} int_parameter_t;

typedef struct _float_parameter_t {
    float value, min, max, step;
} float_parameter_t;

// PEDALS structs

typedef struct _pedal_config_t {
    int_parameter_t int_params[INT_PARAM_TYPES];
    float_parameter_t float_params[FLOAT_PARAM_TYPES];
} pedal_config_t;

typedef struct _pedal_t {
    pedal_config_t config;
    enum pedal_types type;
    float (*pedal_process)(float in, pedal_config_t *p_config);
} pedal_t;

enum pedal_types pedal_type_parse(char *type_str);

// PEDALBOARD

typedef struct _pedalboard_t {
    u_int8_t active_pedals;
    pedal_t pedals[MAX_PEDALS_COUNT];
} pedalboard_t;

void pedalboard_append(pedalboard_t *p_pb, enum pedal_types type);
int16_t pedalboard_process(pedalboard_t *p_pb, int16_t in);

#endif