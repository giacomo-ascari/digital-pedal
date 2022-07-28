#ifndef _OVERDRIVE_H
#define _OVERDRIVE_H

#include "../../pedalboard.h"

void overdrive_pedal_init(pedal_config_t *conf);

float overdrive_process(float in, pedal_config_t *conf);

#endif