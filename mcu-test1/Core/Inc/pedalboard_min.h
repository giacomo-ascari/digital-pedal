#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define _DSP_H
#define _DYN_AMPLIFIER_H
#define FLOAT_PARAM_TYPES 9
#define _BYPASS_H
#define _BITCRUSHER_RS_H
#define _PEDALBOARD_H
#define _OVERDRIVE_H
#define _TREMOLO_H
#define _FUZZ_H
#define _OVERDRIVE_SQRT_H
#define MAX_PEDALS_COUNT 8
#define INT_PARAM_TYPES 3
#define _AMPLIFIER_H
#define _LOW_PASS_FILTER_H




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



float mix(float in_1, float in_2, float balance_1, float balance_2);

float hard_clip(float in, float clip_threshold);

float soft_clip(float in, float soft_threshold, float softener);

float square_root(float in);

float wave_gen(char t, u_int32_t i, float tone);



void overdrive_pedal_init(pedal_config_t *conf);

float overdrive_process(float in, pedal_config_t *conf);



void bitcrusher_rs_pedal_init(pedal_config_t *conf);

float bitcrusher_rs_process(float in, pedal_config_t *conf);



void tremolo_pedal_init(pedal_config_t *conf);

float tremolo_process(float in, pedal_config_t *conf);



void overdrive_sqrt_pedal_init(pedal_config_t *conf);

float overdrive_sqrt_process(float in, pedal_config_t *conf);



void amplifier_pedal_init(pedal_config_t *conf);

float amplifier_process(float in, pedal_config_t *conf);



void dyn_amplifier_pedal_init(pedal_config_t *conf);

float dyn_amplifier_process(float in, pedal_config_t *conf);



void low_pass_filter_pedal_init(pedal_config_t *conf);

float low_pass_filter_process(float in, pedal_config_t *conf);



void fuzz_pedal_init(pedal_config_t *conf);

float fuzz_process(float in, pedal_config_t *conf);



void bypass_pedal_init(pedal_config_t *conf);

float bypass_process(float in, pedal_config_t *conf);

