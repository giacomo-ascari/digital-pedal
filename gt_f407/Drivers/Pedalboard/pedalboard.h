/*
 * pedalboard.h
 *
 *  Created on: 15 mag 2022
 *      Author: giaco
 */

#ifndef PEDALBOARD_PEDALBOARD_H_
#define PEDALBOARD_PEDALBOARD_H_

#define MAX_PEDALS_COUNT 8

// 8 pedals, with 9 float parameters and 3 integers parameters
// 8 * (9 * 32 + 3 * 32) = 3072 bits
// 384 bytes
// 1152 byte with value alongside max and min

// ENUMERATION

enum pedal_types {
    AMPLIFIER,      // AMP
    BITCRUSHER_RS,  // BIT
    BYPASS,         //  -
    FUZZ,           // FZZ
    LPF,            // LPF
    OVERDRIVE,      // OVR
    OVERDRIVE_SQRT, // OVRS
    TREMOLO,        // TRM
	NOISE_GATE		// NGT
};

#define INT_PARAM_TYPES 3

enum int_param_type {
    WIDTH,              // width
    COUNTER,            // multipurpose counter
    REDUCT_INTENSITY,   // reduction intensity
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

// PARAMETERS structs

typedef struct _int_parameter_t {
    int32_t value, min, max;
} int_parameter_t;

typedef struct _float_parameter_t {
    float value, min, max;
} float_parameter_t;

// PEDALS structs

typedef struct _pedal_config_t {
    int_parameter_t int_params[INT_PARAM_TYPES];
    float_parameter_t float_params[FLOAT_PARAM_TYPES];
} pedal_config_t;

typedef struct _pedal_t {
    pedal_config_t config;
    enum pedal_types type;
    void (*pedal_process)(float *value, pedal_config_t *p_config);
} pedal_t;

enum pedal_types pedal_type_parse(char *type_str);

// PEDALBOARD

typedef struct _Pedalboard_Handler {
    u_int8_t active_pedals;
    pedal_t pedals[MAX_PEDALS_COUNT];
} Pedalboard_Handler;

void Pedalboard_Init(Pedalboard_Handler *p_pb);
void Pedalboard_Append(Pedalboard_Handler *p_pb, enum pedal_types type);
void Pedalboard_Process(Pedalboard_Handler *p_pb, float *value);

#endif /* PEDALBOARD_PEDALBOARD_H_ */
