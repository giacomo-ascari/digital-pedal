#ifndef _TREMOLO_H
#define _TREMOLO_H

#include "../../pedalboard.h"

void tremolo_pedal_init(pedal_config_t *conf);

float tremolo_process(float in, pedal_config_t *conf);

#endif