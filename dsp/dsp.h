#ifndef _DSP_H
#define _DSP_H

#include <stdio.h>
#include <stdlib.h>

#define MAX_PEDALS_COUNT 16

enum pedal_types {
    BYPASS,         // bps
    AMPLIFIER,      // amp
    OVERDRIVE,      // ovr
    FUZZ,           // fzz
    BITCRUSHER_RS,  // brs
    TREMOLO,        // trm
    LPF             // lpf
};

// PARAMETERS structs

typedef struct _u_int_parameter_t {
    u_int32_t value, min, max, step;
} u_int_parameter_t;

typedef struct _float_parameter_t {
    float value, min, max, step;
} float_parameter_t;

// PEDALS structs

// BYPASS

typedef struct _bypass_pedal_t {
    float_parameter_t gain_intensity;
    float_parameter_t clip_threshold;
    float_parameter_t soft_threshold;
    float_parameter_t softener;
    float_parameter_t balance;
} bypass_pedal_t;
bypass_pedal_t *bypass_pedal_init();
void bypass_pedal_destroy(bypass_pedal_t *p_pd);
float bypass(float in, bypass_pedal_t *p_pd);

// AMPLIFIER

typedef struct _amplifier_pedal_t {
    float_parameter_t gain_intensity;
    float_parameter_t clip_threshold;
} amplifier_pedal_t;
amplifier_pedal_t *amplifier_pedal_init();
void amplifier_pedal_destroy(amplifier_pedal_t *p_pd);
float amplifier(float in, amplifier_pedal_t *p_pd);

// OVERDRIVE

typedef struct _overdrive_pedal_t {
    float_parameter_t gain_intensity;
    float_parameter_t clip_threshold;
    float_parameter_t soft_threshold;
    float_parameter_t softener;
    float_parameter_t balance;
} overdrive_pedal_t;
overdrive_pedal_t *overdrive_pedal_init();
void overdrive_pedal_destroy(overdrive_pedal_t *p_pd);
float overdrive(float in, overdrive_pedal_t *p_pd);

// FUZZ

typedef struct _fuzz_pedal_t {
    float_parameter_t gain_intensity;
    float_parameter_t clip_threshold;
    float_parameter_t height;
    float_parameter_t speed;
    float_parameter_t balance;
} fuzz_pedal_t;
fuzz_pedal_t *fuzz_pedal_init();
void fuzz_pedal_destroy(fuzz_pedal_t *p_pd);
float fuzz(float in, fuzz_pedal_t *p_pd);

// BITCRUSHER resolution

typedef struct _bitcrusher_rs_pedal_t {
    float_parameter_t reduction_intensity;
    float_parameter_t balance;
} bitcrusher_rs_pedal_t;
bitcrusher_rs_pedal_t *bitcrusher_rs_pedal_init();
void bitcrusher_rs_pedal_destroy(bitcrusher_rs_pedal_t *p_pd);
float bitcrusher_rs(float in, bitcrusher_rs_pedal_t *p_pd);

// TREMOLO

typedef struct _tremolo_pedal_t {
    float_parameter_t speed;
    float_parameter_t balance;
} tremolo_pedal_t;
tremolo_pedal_t *tremolo_pedal_init();
void tremolo_pedal_destroy(tremolo_pedal_t *p_pd);
float tremolo(float in, tremolo_pedal_t *p_pd);

// LOW PASS FILTER

typedef struct _low_pass_filter_pedal_t {
    u_int_parameter_t width;
    float_parameter_t balance;
} low_pass_filter_pedal_t;
low_pass_filter_pedal_t *low_pass_filter_pedal_init();
void low_pass_filter_pedal_destroy(low_pass_filter_pedal_t *p_pd);
float low_pass_filter(float in, low_pass_filter_pedal_t *p_pd);

// ABSTRACT PEDAL

typedef struct _abstract_pedal_t {
    enum pedal_types type;
    char uid[16];
    void *pedal;
} abstract_pedal_t;
abstract_pedal_t *abstract_pedal_init(enum pedal_types type);
void abstract_pedal_destroy(abstract_pedal_t *p_pd);
float abstract_pedal_process(abstract_pedal_t *p_pd, float in);
enum pedal_types pedal_type_parse(char *type_str);

// PEDALBOARD

typedef struct _pedalboard_t {
    u_int8_t active_pedals;
    abstract_pedal_t *pedals[MAX_PEDALS_COUNT];
} pedalboard_t;
pedalboard_t *pedalboard_init();
void pedalboard_destroy(pedalboard_t *p_pb);
void pedalboard_add(pedalboard_t *p_pb, enum pedal_types type);
float pedalboard_process(pedalboard_t *p_pb, float in);

// PROCESSING

float gain(float in, float gain_intensity);
float mix(float in_1, float in_2, float balance);
float hard_clip(float in, float clip_threshold);
float soft_clip(float in, float soft_threshold, float softener);
float reduce_resolution(float in, float reduction_intensity);
float wave_gen(char t, u_int32_t i, float height, float speed);

#endif