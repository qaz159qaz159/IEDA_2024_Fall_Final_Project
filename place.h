#ifndef PLACE_H
#define PLACE_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define type_FF (0x00)
#define type_GATE (0x01)

typedef struct {
	double x;
	double y;
	char* name;
} pin;

typedef struct {
	double width;
	double height;
	uint64_t pinLen;
	uint64_t id;
	pin* pins;
} block;

typedef struct {
	uint64_t len;
	block* item;
} blocks;

typedef blocks FF;
typedef blocks GATE;

/*	for example
 *	Inst C8784 G220 420750 926100
 *	->
 *	struct{
 *		x = 420750;
 *		y = 926100;
 *		type = type_GATE;
 *		type_id = 219 = 220-1;
 *	}
 *	and save at
 *
 * */
typedef struct {
	double x;
	double y;
	uint8_t type; // FF or GATE
	uint64_t type_id;
} Inst;

typedef struct {
	double x;
	double y;
} placement;

typedef struct{
	uint64_t len;
	placement* items;
} placements;

typedef struct {
	double st_x;
	double st_y;
	double width;
	uint64_t count;
} placementsRows;

int place_main(FF, GATE, Inst*, pin* inputs, uint64_t inputLen, pin* outputs, uint64_t outputLen, placementsRows*, double DisplacementDelay,  double* timeslack, uint64_t slack_len, double Alpha, double Lambda, double Die_st_x/* Die start x*/, double Die_st_y, double Die_width, double Die_height, placements* rt/*return*/);

#endif
