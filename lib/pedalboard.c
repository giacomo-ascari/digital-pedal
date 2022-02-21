#include <stdlib.h>
#include <stdio.h>

#include "pedalboard.h"

#include "pedals/amplifier/amplifier.h"
#include "pedals/bitcrusher-rs/bitcrusher-rs.h"
#include "pedals/bypass/bypass.h"
#include "pedals/dyn-amplifier/dyn-amplifier.h"
#include "pedals/fuzz/fuzz.h"
#include "pedals/low-pass-filter/low-pass-filter.h"
#include "pedals/overdrive/overdrive.h"
#include "pedals/overdrive-sqrt/overdrive-sqrt.h"
#include "pedals/tremolo/tremolo.h"

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

int16_t pedalboard_process(pedalboard_t *p_pb, int16_t in) {
    float out = (float)in;
    for (u_int8_t i = 0; i < p_pb->active_pedals; i++) {
        out = p_pb->pedals[i].pedal_process(out, &(p_pb->pedals[i].config));
    }
    return (int16_t)out;
}