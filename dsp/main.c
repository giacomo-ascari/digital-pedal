#include <stdlib.h>
#include <stdio.h>

#define MAX_SAMPLES_COUNT 48000
#define SAMPLES_RATE 48000

int16_t raw_samples[MAX_SAMPLES_COUNT];
int16_t out_samples[MAX_SAMPLES_COUNT];
int16_t samples_count = 0;

float gain(float in, float gain_intensity);
float mix(float in_1, float in_2, float balance);
float hard_clip(float in, float clip_threshold);
float soft_clip(float in, float clip_threshold, float soft_threshold, float softener);
float overlap_last_frequency(float in, float height, float speed);
float reduce_resolution(float in, float reduction_intensity);
float reduce_rate(float in, float reduction_intensity);
float multiply_by_lfo(float in, float speed);
float low_pass_filter(int16_t *in, int i, int width);

int main(int argc, char** argv) {

    // INPUT
    scanf("%d", &samples_count);
    for (int16_t i = 0; i < samples_count; i++) {
        scanf("%d", &raw_samples[i]);
    }

    // DSP
    for (int16_t i = 0; i < samples_count; i++) {

        float x = raw_samples[i];


        out_samples[i] = x;

    }

    // OUTPUT
    for (int16_t i = 0; i < samples_count; i++) {
        printf("%d ", out_samples[i]);
    }

    return 0;
}