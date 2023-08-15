/*
 * lines.h
 *
 *  Created on: 11 mag 2023
 *      Author: Giacomo
 */

// INPUT AND OUTPUT LINES (LEFT, RIGHT, BALANCING ETC.)

#ifndef INC_LINES_H_
#define INC_LINES_H_

#include <stdio.h>
#include <stdlib.h>

#define INPUT_LINE_TYPES 2
#define OUTPUT_LINE_TYPES 4
#define LINE_STRING_SIZE 8+1

#define TS 0
#define RS 1
#define TRS_B 2
#define TRS_U 3

#define LINES_CONF_SIZE 2

typedef struct _lines_conf_t {
	uint8_t input_line;
	uint8_t output_line;
} lines_conf_t;

extern const char input_line_manifest[INPUT_LINE_TYPES][LINE_STRING_SIZE];

extern const char output_line_manifest[OUTPUT_LINE_TYPES][LINE_STRING_SIZE];

void Lines_Init(lines_conf_t *lines_conf);

#endif
