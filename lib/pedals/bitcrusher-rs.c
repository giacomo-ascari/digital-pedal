#include "../pedalboard.h"

//$ PEDAL BITCRUSHER_RS bitcrusher_rs_pedal_init bitcrusher_rs_process

//$ HEADER

void bitcrusher_rs_pedal_init(pedal_config_t *conf);

float bitcrusher_rs_process(float in, pedal_config_t *conf);

//$ HEADER

//$ SOURCE

/*
void bitcrusher_PROC(int16_t * RL, float value, float mix) {
		float R = (float)RL[0];
		float L = (float)RL[1];
		float mixAlt = 1.0-mix;

		RL[0] = value*(int16_t)((R *    mix + L * mixAlt)/ value);
		RL[1] = value*(int16_t)((R * mixAlt + L *    mix)/ value);
	}
*/

/*
if (USE_ESP)
			for (int i = BUF_SIZE/2; i<BUF_SIZE;i+=2){
				bitcrusher_PROC(&AUDIO_OUT_BUF[i],2000,0.1);
//				AUDIO_OUT_BUF[i] = pedalboard_process(&pedalboard, AUDIO_OUT_BUF[i]);
			}

		complete_2nd_half=0;
	  }
*/

void bitcrusher_rs_pedal_init(pedal_config_t *conf) {
    conf->float_params[REDUCT_INTENSITY] = (float_parameter_t){2000, 10, 2000, 10};
    conf->float_params[BALANCE] = (float_parameter_t){0.5, 0, 1, 0.1};
}

float bitcrusher_rs_process(float in, pedal_config_t *conf) {
    int16_t _out = (float)in / conf->float_params[REDUCT_INTENSITY].value;
    float out = (float)_out * conf->float_params[REDUCT_INTENSITY].value;
    out = mix(out, in, conf->float_params[BALANCE].value);
    return out;
}

//$ SOURCE