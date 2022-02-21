#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../lib/pedalboard.min.h"

#define MAX_SAMPLES_COUNT 48000
#define SAMPLES_RATE 48000

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

int main(int argc, char** argv)
{

    int16_t raw_samples[MAX_SAMPLES_COUNT];
    int16_t out_samples[MAX_SAMPLES_COUNT];
    int32_t index = 0, circular_index;

    FILE *raw_file = fopen(argv[1], "r");
    FILE *out_file = fopen(argv[2], "w");

    pedalboard_t pedalboard;
    for (u_int32_t i = 3; i < argc; i++) {
        pedalboard_append(&pedalboard, pedal_type_parse(argv[i]));
    }

    while(!feof(raw_file))
    {

        // getting traslated index
        circular_index = index % MAX_SAMPLES_COUNT;
        
        // acquire raw data from stdin
        fscanf(raw_file, "%hd", &raw_samples[circular_index]);
        
        // digital signal processing
        out_samples[circular_index] = pedalboard_process(&pedalboard, raw_samples[circular_index]);

        // send processed data to stdout
        fprintf(out_file, "%hd\n", out_samples[circular_index]);

        // increase index
        index++;
    }

    fclose(raw_file);
    fclose(out_file);

    return 0;
}