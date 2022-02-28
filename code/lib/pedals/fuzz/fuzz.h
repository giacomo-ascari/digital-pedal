#ifndef _FUZZ_H
#define _FUZZ_H

#include "../../pedalboard.h"

void fuzz_pedal_init(pedal_config_t *conf);

float fuzz_process(float in, pedal_config_t *conf);

#endif