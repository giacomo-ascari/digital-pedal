/*
 * mode.c
 *
 *  Created on: Jun 30, 2022
 *      Author: asky
 */

#include "mode.h"

#ifdef F103

#include <string.h>

void Mode_Manifest_Init(mode_manifest_t * manifest) {
	strcpy(manifest[TS_TO_TS].desc, "ts to ts");
	strcpy(manifest[TS_TO_RS].desc, "ts to rs");
	strcpy(manifest[RS_TO_TS].desc, "rs to ts");
	strcpy(manifest[RS_TO_RS].desc, "rs to rs");
	strcpy(manifest[TS_TO_TRS_BALANCED].desc, "ts to trs bal.");
	strcpy(manifest[TS_TO_TRS_UNBALANCED].desc, "ts to trs unbal.");
	strcpy(manifest[RS_TO_TRS_BALANCED].desc, "rs to trs bal.");
	strcpy(manifest[RS_TO_TRS_UNBALANCED].desc, "rs to trs unbal.");
}

#endif
