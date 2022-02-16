#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "pedalboard_min.h"

// AMPLIFIER

void amplifier_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){0.5F, 0.1F, 10.F, 0.1F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F, 1.F};
}

float amplifier_process(float in, pedal_config_t *conf) {
    float out = in * conf->float_params[INTENSITY].value;
    out = hard_clip(out, conf->float_params[THRESHOLD_HIGH].value);
    return out;
}

// BYPASS

void bypass_pedal_init(pedal_config_t *conf) {
    return;
}

float bypass_process(float in, pedal_config_t *conf) {
    return in;
}

// TREMOLO

void tremolo_pedal_init(pedal_config_t *conf) {
    conf->int_params[COUNTER] = (int_parameter_t){0, 1, 0, 0};
    conf->float_params[SPEED] = (float_parameter_t){32.F, 2.F, 64.F, 2.F};
    conf->float_params[BALANCE_1] = (float_parameter_t){0.1F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.9F, 0.F, 1.F, 0.1F};
}

float tremolo_process(float in, pedal_config_t *conf) {
    float out = in * wave_gen('s', conf->int_params[COUNTER].value, 1.F, conf->float_params[SPEED].value);
    conf->int_params[COUNTER].value++;
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}

// BITCRUSHER resolution

void bitcrusher_rs_pedal_init(pedal_config_t *conf) {
    conf->int_params[REDUCT_INTENSITY] = (int_parameter_t){12, 1, 16, 1};
    conf->float_params[BALANCE_1] = (float_parameter_t){0.5F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.5F, 0.F, 1.F, 0.1F};
}

float bitcrusher_rs_process(float in, pedal_config_t *conf) {
    int16_t shift = conf->int_params[REDUCT_INTENSITY].value;
    int16_t _out = (int16_t)in;
    _out = _out >> shift;
    _out = _out << shift;
    _out += (2 << (shift -2));
    float out = (float)_out;
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}

// LPF

void low_pass_filter_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){0.9F, 0.0F, 1.0F, 0.01F};
    conf->float_params[BALANCE_1] = (float_parameter_t){1.F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.F, 0.F, 1.F, 0.1F};
    conf->float_params[PAST] = (float_parameter_t){0.F, 0.F, 0.F, 0.F};
}

float low_pass_filter_process(float in, pedal_config_t *conf) {
    float alpha = conf->float_params[INTENSITY].value;
    float out = conf->float_params[PAST].value * alpha + (1.F - alpha) * in;
    conf->float_params[PAST].value = out;
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}

// DYN_AMPLIFIER

void dyn_amplifier_pedal_init(pedal_config_t *conf) {
    //conf->float_params[GAIN_INTENSITY] = (float_parameter_t){1000, 0.1, 10, 0.1};
    //conf->float_params[CLIP_THRESHOLD] = (float_parameter_t){32767, 0, 32767, 1};
    //conf->float_params[SOFT_THRESHOLD] = (float_parameter_t){28000, 0, 32767, 1};
    //conf->float_params[BALANCE] = (float_parameter_t){0.5, 0, 1, 0.1};
}

float dyn_amplifier_process(float in, pedal_config_t *conf) {
    //static float dyn = 1.0f, max = 0;;
    //static u_int16_t i = 0;
    //float out = gain(in, dyn);
    //if (out > max) max = out;
    //i++;
    //if (!(i%4800)) {
    //    if (max > conf->float_params[SOFT_THRESHOLD].value) dyn -= 0.01;
    //    else if (max < conf->float_params[SOFT_THRESHOLD].value) dyn += 0.01;
    //    max = 0;
    //}
    //out = hard_clip(out, conf->float_params[CLIP_THRESHOLD].value);
    //return out;
    return in;
}

// OVERDRIVE_SQRT

void overdrive_sqrt_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){500.F, 0.F, 2000.F, 100.F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F, 1.F};
    conf->float_params[BALANCE_1] = (float_parameter_t){0.25F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.75F, 0.F, 1.F, 0.1F};
}

float overdrive_sqrt_process(float in, pedal_config_t *conf) {
    float out;
    if (in > 0.0F) {
        out = square_root(in);
    } else {
        out = -square_root(-in);
    }
    out = out * conf->float_params[INTENSITY].value;
    out = hard_clip(out, conf->float_params[THRESHOLD_HIGH].value);
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}

// OVERDRIVE

void overdrive_pedal_init(pedal_config_t *conf) {
    conf->float_params[INTENSITY] = (float_parameter_t){4.F, 1.F, 10.F, 0.5F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F, 1.F};
    conf->float_params[THRESHOLD_LOW] = (float_parameter_t){28000.F, 0.F, 32767.F, 1.F};
    conf->float_params[SOFTENER] = (float_parameter_t){4.F, 1.F, 10.F, 0.5F};
    conf->float_params[BALANCE_1] = (float_parameter_t){1.F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.F, 0.F, 1.F, 0.1F};
}

float overdrive_process(float in, pedal_config_t *conf) {
    float out = in *  conf->float_params[INTENSITY].value;
    out = soft_clip(out, conf->float_params[THRESHOLD_LOW].value, conf->float_params[SOFTENER].value);
    out = hard_clip(out, conf->float_params[THRESHOLD_HIGH].value);
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}

// FUZZ

void fuzz_pedal_init(pedal_config_t *conf) {
    conf->int_params[COUNTER] = (int_parameter_t){0, 1, 0, 0};
    conf->float_params[INTENSITY] = (float_parameter_t){4.F, 1.F, 10.F, 0.5F};
    conf->float_params[THRESHOLD_HIGH] = (float_parameter_t){32767.F, 0.F, 32767.F, 1.F};
    conf->float_params[HEIGHT] = (float_parameter_t){2048.F, 0.F, 8192.F, 32.F};
    conf->float_params[SPEED] = (float_parameter_t){20.F, 2.F, 64.F, 2.F};
    conf->float_params[BALANCE_1] = (float_parameter_t){1.F, 0.F, 1.F, 0.1F};
    conf->float_params[BALANCE_2] = (float_parameter_t){0.F, 0.F, 1.F, 0.1F};
}

float fuzz_process(float in, pedal_config_t *conf) {
    float out = in * conf->float_params[INTENSITY].value;
    float threshold_high = conf->float_params[THRESHOLD_HIGH].value;
    float height = conf->float_params[HEIGHT].value;
    float speed = conf->float_params[SPEED].value;

    out = hard_clip(out, threshold_high);
    if (out == threshold_high) {
        out = out + wave_gen('s', conf->int_params[COUNTER].value, height, speed) - height;
    } else if (out == -threshold_high - 1.F) {
        out = out - wave_gen('s', conf->int_params[COUNTER].value, height, speed) + height;
    }
    conf->int_params[COUNTER].value++;
    out = mix(out, in, conf->float_params[BALANCE_1].value, conf->float_params[BALANCE_2].value);
    return out;
}


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
    for (u_int8_t i = 0; i < 8; i++) out = 0.5F * (out + (in / out));
    return out;
}

float wave_gen(char t, u_int32_t i, float height, float speed) {
    float sin_table[512] = { 0, 0.00307396, 0.00614789, 0.00922176, 0.0122955, 0.0153692, 0.0184427, 0.0215161, 0.0245892, 0.0276621, 0.0307348, 0.0338071, 0.0368792, 0.0399509, 0.0430222, 0.0460931, 0.0491636, 0.0522336, 0.0553031, 0.0583721, 0.0614405, 0.0645084, 0.0675756, 0.0706423, 0.0737082, 0.0767734, 0.079838, 0.0829017, 0.0859647, 0.0890269, 0.0920882, 0.0951487, 0.0982083, 0.101267, 0.104325, 0.107381, 0.110437, 0.113492, 0.116545, 0.119598, 0.122649, 0.125699, 0.128748, 0.131796, 0.134842, 0.137888, 0.140932, 0.143974, 0.147015, 0.150055, 0.153094, 0.156131, 0.159166, 0.1622, 0.165233, 0.168264, 0.171293, 0.174321, 0.177347, 0.180371, 0.183394, 0.186415, 0.189434, 0.192451, 0.195467, 0.198481, 0.201493, 0.204503, 0.207511, 0.210517, 0.213521, 0.216523, 0.219523, 0.222521, 0.225517, 0.22851, 0.231502, 0.234491, 0.237478, 0.240463, 0.243446, 0.246426, 0.249404, 0.25238, 0.255353, 0.258324, 0.261292, 0.264258, 0.267222, 0.270183, 0.273141, 0.276097, 0.27905, 0.282, 0.284948, 0.287893, 0.290836, 0.293776, 0.296713, 0.299647, 0.302578, 0.305506, 0.308432, 0.311355, 0.314274, 0.317191, 0.320105, 0.323015, 0.325923, 0.328828, 0.331729, 0.334627, 0.337523, 0.340415, 0.343303, 0.346189, 0.349071, 0.35195, 0.354826, 0.357698, 0.360567, 0.363432, 0.366294, 0.369153, 0.372008, 0.37486, 0.377708, 0.380552, 0.383393, 0.38623, 0.389064, 0.391894, 0.39472, 0.397542, 0.400361, 0.403176, 0.405987, 0.408795, 0.411598, 0.414398, 0.417193, 0.419985, 0.422773, 0.425556, 0.428336, 0.431112, 0.433883, 0.436651, 0.439414, 0.442173, 0.444929, 0.447679, 0.450426, 0.453168, 0.455906, 0.45864, 0.46137, 0.464095, 0.466815, 0.469532, 0.472243, 0.474951, 0.477654, 0.480352, 0.483046, 0.485735, 0.48842, 0.4911, 0.493775, 0.496446, 0.499112, 0.501773, 0.50443, 0.507082, 0.509729, 0.512371, 0.515008, 0.517641, 0.520269, 0.522891, 0.525509, 0.528122, 0.53073, 0.533332, 0.53593, 0.538523, 0.541111, 0.543693, 0.54627, 0.548843, 0.55141, 0.553971, 0.556528, 0.559079, 0.561625, 0.564166, 0.566701, 0.569231, 0.571756, 0.574275, 0.576789, 0.579297, 0.5818, 0.584298, 0.58679, 0.589276, 0.591757, 0.594232, 0.596701, 0.599165, 0.601624, 0.604076, 0.606523, 0.608964, 0.6114, 0.613829, 0.616253, 0.618671, 0.621083, 0.623489, 0.62589, 0.628284, 0.630673, 0.633055, 0.635432, 0.637802, 0.640167, 0.642525, 0.644878, 0.647224, 0.649564, 0.651899, 0.654226, 0.656548, 0.658864, 0.661173, 0.663476, 0.665773, 0.668063, 0.670348, 0.672625, 0.674897, 0.677162, 0.679421, 0.681673, 0.683919, 0.686158, 0.688391, 0.690618, 0.692838, 0.695051, 0.697258, 0.699458, 0.701651, 0.703838, 0.706019, 0.708192, 0.710359, 0.712519, 0.714673, 0.71682, 0.71896, 0.721093, 0.723219, 0.725339, 0.727451, 0.729557, 0.731656, 0.733748, 0.735833, 0.737911, 0.739982, 0.742046, 0.744103, 0.746154, 0.748197, 0.750233, 0.752261, 0.754283, 0.756298, 0.758305, 0.760306, 0.762299, 0.764285, 0.766263, 0.768235, 0.770199, 0.772156, 0.774106, 0.776048, 0.777983, 0.779911, 0.781831, 0.783744, 0.785649, 0.787547, 0.789438, 0.791321, 0.793197, 0.795065, 0.796926, 0.798779, 0.800625, 0.802463, 0.804293, 0.806116, 0.807931, 0.809739, 0.811539, 0.813331, 0.815115, 0.816892, 0.818662, 0.820423, 0.822177, 0.823923, 0.825661, 0.827391, 0.829114, 0.830828, 0.832535, 0.834234, 0.835925, 0.837608, 0.839283, 0.840951, 0.84261, 0.844262, 0.845905, 0.84754, 0.849168, 0.850787, 0.852399, 0.854002, 0.855597, 0.857184, 0.858764, 0.860334, 0.861897, 0.863452, 0.864999, 0.866537, 0.868067, 0.869589, 0.871103, 0.872608, 0.874105, 0.875594, 0.877075, 0.878548, 0.880012, 0.881468, 0.882915, 0.884354, 0.885785, 0.887207, 0.888622, 0.890027, 0.891424, 0.892813, 0.894194, 0.895565, 0.896929, 0.898284, 0.89963, 0.900968, 0.902298, 0.903619, 0.904931, 0.906235, 0.90753, 0.908817, 0.910095, 0.911365, 0.912626, 0.913878, 0.915122, 0.916357, 0.917583, 0.918801, 0.92001, 0.92121, 0.922402, 0.923585, 0.924759, 0.925924, 0.927081, 0.928229, 0.929368, 0.930499, 0.93162, 0.932733, 0.933837, 0.934932, 0.936018, 0.937096, 0.938164, 0.939224, 0.940275, 0.941317, 0.94235, 0.943374, 0.944389, 0.945396, 0.946393, 0.947382, 0.948361, 0.949332, 0.950293, 0.951246, 0.95219, 0.953124, 0.95405, 0.954966, 0.955874, 0.956773, 0.957662, 0.958542, 0.959414, 0.960276, 0.961129, 0.961974, 0.962809, 0.963635, 0.964452, 0.965259, 0.966058, 0.966848, 0.967628, 0.968399, 0.969161, 0.969914, 0.970658, 0.971393, 0.972118, 0.972834, 0.973541, 0.974239, 0.974928, 0.975607, 0.976277, 0.976938, 0.97759, 0.978232, 0.978866, 0.97949, 0.980105, 0.98071, 0.981306, 0.981893, 0.982471, 0.983039, 0.983598, 0.984148, 0.984689, 0.98522, 0.985742, 0.986254, 0.986758, 0.987252, 0.987736, 0.988211, 0.988677, 0.989134, 0.989581, 0.990019, 0.990448, 0.990867, 0.991277, 0.991677, 0.992068, 0.99245, 0.992822, 0.993185, 0.993539, 0.993883, 0.994218, 0.994543, 0.994859, 0.995166, 0.995463, 0.995751, 0.996029, 0.996298, 0.996558, 0.996808, 0.997048, 0.99728, 0.997502, 0.997714, 0.997917, 0.998111, 0.998295, 0.99847, 0.998635, 0.998791, 0.998937, 0.999074, 0.999202, 0.99932, 0.999428, 0.999528, 0.999617, 0.999698, 0.999768, 0.99983, 0.999882, 0.999924, 0.999957, 0.999981, 0.999995, 1 };
    if (t == 's') {
        i = (float)i * speed;
        u_int32_t x = i % 512;
        u_int32_t quadrant = (i % 2048) / 512;
        if (quadrant == 0) return sin_table[x] * height;
        else if (quadrant == 1) return sin_table[511 - x] * height;
        else if (quadrant == 2) return -sin_table[x] * height;
        else /*if (quadrant == 3)*/ return -sin_table[511 - x] * height;
    }
    return 0;
}

// PEDALBOARD

void pedalboard_append(pedalboard_t *p_pb, enum pedal_types type) {

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
