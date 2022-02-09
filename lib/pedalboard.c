#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pedalboard.h"

<<<<<<< HEAD
#include "pedals/amplifier/amplifier.h"
#include "pedals/bitcrusher-rs/bitcrusher-rs.h"
#include "pedals/bypass/bypass.h"
#include "pedals/dyn-amplifier/dyn-amplifier.h"
#include "pedals/fuzz/fuzz.h"
#include "pedals/low-pass-filter/low-pass-filter.h"
#include "pedals/overdrive/overdrive.h"
#include "pedals/overdrive-sqrt/overdrive-sqrt.h"
#include "pedals/tremolo/tremolo.h"

enum pedal_types pedal_type_parse(char *type_str) {
    enum pedal_types type;
    if (strcmp(type_str, "bps") == 0) type = BYPASS;
    else if (strcmp(type_str, "amp") == 0) type = AMPLIFIER;
    else if (strcmp(type_str, "damp") == 0) type = DYN_AMPLIFIER;
    else if (strcmp(type_str, "ovr") == 0) type = OVERDRIVE;
    else if (strcmp(type_str, "ovrs") == 0) type = OVERDRIVE_SQRT;
    else if (strcmp(type_str, "fzz") == 0) type = FUZZ;
    else if (strcmp(type_str, "brs") == 0) type = BITCRUSHER_RS;
    else if (strcmp(type_str, "trm") == 0) type = TREMOLO;
    else if (strcmp(type_str, "lpf") == 0) type = LPF;
    return type;
}

=======
>>>>>>> bf62d7bae70927fca8cb880393defa669ad491d1
// PEDALBOARD

void pedalboard_append(pedalboard_t *p_pb, enum pedal_types type) {
    u_int8_t i = p_pb->active_pedals;

    if (p_pb->active_pedals < MAX_PEDALS_COUNT) {
        u_int8_t i = p_pb->active_pedals;
        p_pb->pedals[i].type = type;

        if (type == AMPLIFIER)
        {
            amplifier_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = amplifier_process;
        }
        else if (type == BITCRUSHER_RS)
        {
            bitcrusher_rs_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = bitcrusher_rs_process;
        }
        else if (type == DYN_AMPLIFIER)
        {
            dyn_amplifier_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = dyn_amplifier_process;
        }
        else if (type == FUZZ)
        {
            fuzz_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = fuzz_process;
        }
        else if (type == LPF)
        {
            low_pass_filter_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = low_pass_filter_process;
        }
        else if (type == OVERDRIVE)
        {
            overdrive_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = overdrive_process;
        }
        else if (type == OVERDRIVE_SQRT)
        {
            overdrive_sqrt_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = overdrive_sqrt_process;
        }
        else if (type == TREMOLO)
        {
            tremolo_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = tremolo_process;
        }
        else
        {
            // BYPASS AS DEFAULT
            bypass_pedal_init(&(p_pb->pedals[i].config));
            p_pb->pedals[i].pedal_process = bypass_process;
        }
        p_pb->active_pedals++;
    }
}

float pedalboard_process(pedalboard_t *p_pb, float in) {
    float out = in;
    for (u_int8_t i = 0; i < p_pb->active_pedals; i++) {
        out = p_pb->pedals[i].pedal_process(out, &(p_pb->pedals[i].config));
    }
    return out;
}