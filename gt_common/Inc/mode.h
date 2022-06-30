/*
 * mode.h
 *
 *  Created on: Jun 30, 2022
 *      Author: asky
 */

#ifndef INC_MODE_H_
#define INC_MODE_H_

#define MODE_TYPES 8

// remember tip is right, ring is left
enum mode_type {
	TS_TO_TS = 0,
	TS_TO_RS = 1,
	RS_TO_TS = 2,
	RS_TO_RS = 3,
	TS_TO_TRS_BALANCED = 4,
	TS_TO_TRS_UNBALANCED = 5,
	RS_TO_TRS_BALANCED = 6,
	RS_TO_TRS_UNBALANCED = 7,
};

typedef struct _mode_manifest_t {
	char desc[24];
} mode_manifest_t;

void Mode_Manifest_Init(mode_manifest_t * manifest);

#endif /* INC_MODE_H_ */
