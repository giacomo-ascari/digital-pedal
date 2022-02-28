#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "dsp.h"

float mix(float in_1, float in_2, float balance_1, float balance_2) {
    return in_1 * balance_1 + in_2 * balance_2;
}

float hard_clip(float in, float clip_threshold) {
    if (in > clip_threshold) {
        return clip_threshold;
    } else if (in < -clip_threshold) {
        return -clip_threshold - 1;
    }
    return in;
}

float soft_clip(float in, float soft_threshold, float softener) {
    float out;
    if (in > soft_threshold) {
        out = soft_threshold + (in - soft_threshold) / softener;
    } else if (in < -soft_threshold) {
        out = -soft_threshold + (in + soft_threshold) / softener;
    } else {
        out = in;
    }
    return out;
}

float square_root(float in) {
    float out;  
    out = in * 0.5F;
    for (u_int8_t i = 0; i < 8; i++) {
        out = 0.5F * (out + (in / out));
    }
    return out;
}

float wave_gen(char t, u_int32_t i, u_int32_t tone) {
    
    float sin_table[128] = { 0.000000F, 0.024541F, 0.049068F, 0.073565F, 0.098017F, 0.122411F, 0.146730F, 0.170962F, 0.195090F, 0.219101F, 0.242980F, 0.266713F, 0.290284F, 0.313681F, 0.336890F, 0.359895F, 0.382683F, 0.405241F, 0.427555F, 0.449611F, 0.471396F, 0.492898F, 0.514102F, 0.534997F, 0.555570F, 0.575808F, 0.595699F, 0.615231F, 0.634393F, 0.653172F, 0.671559F, 0.689540F, 0.707106F, 0.724247F, 0.740951F, 0.757208F, 0.773010F, 0.788346F, 0.803207F, 0.817584F, 0.831469F, 0.844853F, 0.857728F, 0.870087F, 0.881921F, 0.893224F, 0.903989F, 0.914209F, 0.923879F, 0.932992F, 0.941544F, 0.949528F, 0.956940F, 0.963776F, 0.970031F, 0.975702F, 0.980785F, 0.985277F, 0.989176F, 0.992479F, 0.995185F, 0.997290F, 0.998795F, 0.999699F, 1.000000F, 0.999699F, 0.998796F, 0.997291F, 0.995185F, 0.992480F, 0.989177F, 0.985278F, 0.980786F, 0.975702F, 0.970032F, 0.963776F, 0.956941F, 0.949529F, 0.941545F, 0.932993F, 0.923880F, 0.914210F, 0.903990F, 0.893225F, 0.881922F, 0.870088F, 0.857729F, 0.844855F, 0.831471F, 0.817586F, 0.803209F, 0.788348F, 0.773012F, 0.757210F, 0.740952F, 0.724248F, 0.707108F, 0.689542F, 0.671560F, 0.653174F, 0.634395F, 0.615233F, 0.595701F, 0.575810F, 0.555572F, 0.534999F, 0.514105F, 0.492900F, 0.471399F, 0.449613F, 0.427557F, 0.405243F, 0.382685F, 0.359897F, 0.336892F, 0.313684F, 0.290287F, 0.266715F, 0.242982F, 0.219104F, 0.195093F, 0.170964F, 0.146733F, 0.122413F, 0.098019F, 0.073567F, 0.049070F, 0.024544F };
    u_int32_t period = 48000 / tone;
    u_int32_t table_index = (i * 256 * period) % 256;

    if (t == 's') {
        if (table_index <= 127) {
            return sin_table[table_index];
        } else {
            return sin_table[table_index - 128] * (-1);
        }
    } else if (t == 'q') {
        if (table_index <= 127) {
            return -1.F;
        } else {
            return 1.F;
        }
    } else {
        return 0;
    }
}