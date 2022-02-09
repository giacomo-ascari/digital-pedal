#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#define _LOW_PASS_FILTER_H
#define _OVERDRIVE_H
#define U_INT_PARAM_TYPES 2
#define _OVERDRIVE_SQRT_H
#define _BYPASS_H
#define FLOAT_PARAM_TYPES 9
#define _AMPLIFIER_H
#define _DYN_AMPLIFIER_H
#define _DSP_H
#define _BITCRUSHER_RS_H
#define _FUZZ_H
#define MAX_PEDALS_COUNT 16
#define _PEDALBOARD_H
#define _TREMOLO_H



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



float gain(float in, float gain_intensity);

float mix(float in_1, float in_2, float balance_1, float balance_2);

float hard_clip(float in, float clip_threshold);

float soft_clip(float in, float soft_threshold, float softener);

float reduce_resolution(float in, float reduction_intensity);

float square_root(float in);

float wave_gen(char t, u_int32_t i, float height, float speed);



void amplifier_pedal_init(pedal_config_t *conf);

float amplifier_process(float in, pedal_config_t *conf);



void bypass_pedal_init(pedal_config_t *conf);

float bypass_process(float in, pedal_config_t *conf);



void tremolo_pedal_init(pedal_config_t *conf);

float tremolo_process(float in, pedal_config_t *conf);



void bitcrusher_rs_pedal_init(pedal_config_t *conf);

float bitcrusher_rs_process(float in, pedal_config_t *conf);



void low_pass_filter_pedal_init(pedal_config_t *conf);

float low_pass_filter_process(float in, pedal_config_t *conf);



void dyn_amplifier_pedal_init(pedal_config_t *conf);

float dyn_amplifier_process(float in, pedal_config_t *conf);



void overdrive_sqrt_pedal_init(pedal_config_t *conf);

float overdrive_sqrt_process(float in, pedal_config_t *conf);



void overdrive_pedal_init(pedal_config_t *conf);

float overdrive_process(float in, pedal_config_t *conf);



void fuzz_pedal_init(pedal_config_t *conf);

float fuzz_process(float in, pedal_config_t *conf);

