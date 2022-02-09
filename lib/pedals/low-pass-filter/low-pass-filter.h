#ifndef _LOW_PASS_FILTER_H
#define _LOW_PASS_FILTER_H

#include "../../pedalboard.h"

void low_pass_filter_pedal_init(pedal_config_t *conf);

float low_pass_filter_process(float in, pedal_config_t *conf);

#endif