#ifndef _BYPASS_H
#define _BYPASS_H

#include "../../pedalboard.h"

void bypass_pedal_init(pedal_config_t *conf);

float bypass_process(float in, pedal_config_t *conf);

#endif