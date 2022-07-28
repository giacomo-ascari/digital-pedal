#ifndef _DYN_AMPLIFIER_H
#define _DYN_AMPLIFIER_H

#include "../../pedalboard.h"

void dyn_amplifier_pedal_init(pedal_config_t *conf);

float dyn_amplifier_process(float in, pedal_config_t *conf);

#endif