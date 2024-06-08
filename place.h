#ifndef PLACE_H
#define PLACE_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NAME_LEN 32

#define TYPE_FF 	0x00
#define TYPE_GATE 	0x01

typedef struct {
	double x;
	double y;
	char* name[MAX_NAME_LEN];
} Pin;

typedef struct {
	char* name[MAX_NAME_LEN];
	uint64_t x;
	uint64_t y;
} IO;

typedef IO Input;
typedef IO Output;

typedef struct {
	uint64_t count;
	Input* items;
} IOs;

typedef IOs Inputs;
typedef IOs Outputs;

typedef struct {
	uint64_t bits;
	char* name[MAX_NAME_LEN];
	double width;
	double height;
	uint64_t pin_count;
	Pin* pins;
} Block;

typedef Block FF;
typedef Block Gate;

typedef struct {
	uint64_t count;
	Block* items;
} Blocks;

typedef Blocks FFs;
typedef Blocks Gates;

typedef struct {
	double x;
	double y;
	char* inst_name[MAX_NAME_LEN];
	char* lib_cell_name[MAX_NAME_LEN];
} Inst;

typedef struct {
	uint64_t count;
	Inst* items;
} Insts;

typedef struct {
    char name[MAX_NAME_LEN];
    int pinCount;
    struct {
        char instName[MAX_NAME_LEN];
        char libPinName[MAX_NAME_LEN];
    } *pins;
} Net;

typedef struct {
	uint64_t count;
	Net* items;
} Nets;

typedef struct {
	double start_x;
	double start_y;
	double width;
	double height;
	uint64_t totalNumOfSites;
} PlacementsRows;

typedef struct {
	uint64_t count;
	PlacementsRows* items;
} PlacementsRowsSet;

typedef struct {
    char libCellName[MAX_NAME_LEN];
    float delay;
} QpinDelay;

typedef struct {
	uint64_t count;
	QpinDelay* items;
} QpinDelays;

typedef struct {
    char instanceCellName[MAX_NAME_LEN];
    char pinName[MAX_NAME_LEN];
    double slack;
} TimingSlack;

typedef struct {
	uint64_t count;
	TimingSlack* items;
} TimingSlacks;

typedef struct {
    char libCellName[MAX_NAME_LEN];
    double powerConsumption;
} GatePower;

typedef struct {
	uint64_t count;
	GatePower* items;
} GatePowers;


typedef struct {
    float coefficient;
} DisplacementDelay;









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
} Placement;

typedef struct{
	uint64_t count;
	Placement* items;
} Placements;





typedef struct {
    double width, height, maxUtil;
} Bin;

int place_main(FF, Gate, Inst*, Pin* inputs, uint64_t inputLen, Pin* outputs, uint64_t outputLen, PlacementsRows*, double DisplacementDelay,  double* timeslack, uint64_t slack_len, double Alpha, double Lambda, double Die_st_x/* Die start x*/, double Die_st_y, double Die_width, double Die_height, Placements* rt/*return*/);

#endif // PLACE_H
