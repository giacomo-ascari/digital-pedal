#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dsp.h"

#define MAX_SAMPLES_COUNT 48000
#define SAMPLES_RATE 48000

int main(int argc, char** argv) {

    int16_t raw_samples[MAX_SAMPLES_COUNT];
    int16_t out_samples[MAX_SAMPLES_COUNT];
    int32_t samples_index = 0;

    FILE *raw_file = fopen(argv[2], "r");
    FILE *out_file = fopen(argv[3], "w");

    while(!feof(raw_file)) {

        // getting traslated index
        int32_t i = samples_index % MAX_SAMPLES_COUNT;
        
        // acquire raw data from stdin
        fscanf(raw_file, "%hd", &raw_samples[i]);

        // digital signal processing
        if (strcmp(argv[1], "dst") == 0) {
            out_samples[i] = distortion(raw_samples[i], 4, 32768-1, 25000, 5, 1);
        } else {
            out_samples[i] = rand() % 64000 - 32000;
        }

        // send processed data to stdout
        fprintf(out_file, "%hd\n", out_samples[i]);

        // increase index
        samples_index++;

    }

    fclose(raw_file);
    fclose(out_file);

    return 0;
}