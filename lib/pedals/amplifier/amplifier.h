#ifndef _AMPLIFIER_H
#define _AMPLIFIER_H

#include "../../pedalboard.h"

void amplifier_pedal_init(pedal_config_t *conf);

float amplifier_process(float in, pedal_config_t *conf);

#endif