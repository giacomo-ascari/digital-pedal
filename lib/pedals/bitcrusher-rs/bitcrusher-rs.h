#ifndef _BITCRUSHER_RS_H
#define _BITCRUSHER_RS_H

#include "../../pedalboard.h"

void bitcrusher_rs_pedal_init(pedal_config_t *conf);

float bitcrusher_rs_process(float in, pedal_config_t *conf);

#endif