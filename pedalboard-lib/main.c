#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "pedalboard.h"

#define MAX_SAMPLES_COUNT 48000
#define SAMPLES_RATE 48000

int main(int argc, char** argv)
{

    int16_t raw_samples[MAX_SAMPLES_COUNT];
    int16_t out_samples[MAX_SAMPLES_COUNT];
    int32_t samples_index = 0;

    FILE *raw_file = fopen(argv[1], "r");
    FILE *out_file = fopen(argv[2], "w");

    pedalboard_t pedalboard;
    for (u_int32_t i = 3; i < argc; i++) {
        pedalboard_append(&pedalboard, pedal_type_parse(argv[i]));
    }

    while(!feof(raw_file))
    {

        // getting traslated index
        int32_t i = samples_index % MAX_SAMPLES_COUNT;
        
        // acquire raw data from stdin
        fscanf(raw_file, "%hd", &raw_samples[i]);
        
        // digital signal processing
        out_samples[i] = pedalboard_process(&pedalboard, raw_samples[i]);

        // send processed data to stdout
        fprintf(out_file, "%hd\n", out_samples[i]);

        // increase index
        samples_index++;
    }

    fclose(raw_file);
    fclose(out_file);

    return 0;
}