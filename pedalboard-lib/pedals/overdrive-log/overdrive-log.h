#ifndef _OVERDRIVE_LOG_H
#define _OVERDRIVE_LOG_H

#include "../../pedalboard.h"

void overdrive_log_pedal_init(pedal_config_t *conf);

float overdrive_log_process(float in, pedal_config_t *conf);

#endif