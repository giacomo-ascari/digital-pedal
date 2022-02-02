#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define _TREMOLO_H
#define _BITCRUSHER_RS_H
#define _LOW_PASS_FILTER_H
#define _FUZZ_H
#define MAX_PEDALS_COUNT 16
#define _BYPASS_H
#define _PEDALBOARD_H
#define FLOAT_PARAM_TYPES 8
#define PEDAL_TYPES 8
#define _OVERDRIVE_LOG_H
#define _DYN_AMPLIFIER_H
#define U_INT_PARAM_TYPES 1
#define _AMPLIFIER_H
#define _OVERDRIVE_H




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



void overdrive_log_pedal_init(pedal_config_t *conf);

float overdrive_log_process(float in, pedal_config_t *conf);



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



void overdrive_pedal_init(pedal_config_t *conf);

float overdrive_process(float in, pedal_config_t *conf);



void fuzz_pedal_init(pedal_config_t *conf);

float fuzz_process(float in, pedal_config_t *conf);

