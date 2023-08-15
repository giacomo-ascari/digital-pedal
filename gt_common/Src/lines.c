/*
 * lines.c
 *
 *  Created on: 11 mag 2023
 *      Author: Giacomo
 */

#include "lines.h"

const char input_line_manifest[INPUT_LINE_TYPES][LINE_STRING_SIZE] = {
	[TS] = {"ts"},
	[RS] = {"rs"},
};

const char output_line_manifest[OUTPUT_LINE_TYPES][LINE_STRING_SIZE] = {
	[TS] = {"ts"},
	[RS] = {"rs"},
	[TRS_B] = {"trs bal."},
	[TRS_U] = {"trs unb."}
};

void Lines_Init(lines_conf_t *lines_conf) {
	lines_conf->input_line = TS;
	lines_conf->output_line = TS;
}
