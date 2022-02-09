#ifndef _OVERDRIVE_SQRT_H
#define _OVERDRIVE_SQRT_H

#include "../../pedalboard.h"

void overdrive_sqrt_pedal_init(pedal_config_t *conf);

float overdrive_sqrt_process(float in, pedal_config_t *conf);

#endif