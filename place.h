#ifndef PLACE_H
#define PLACE_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NAME_LEN 16

typedef struct {
	uint16_t x;
	uint16_t y;
	char* name[MAX_NAME_LEN];
} Pin;

typedef struct {
	char* name[MAX_NAME_LEN];
	uint32_t x;
	uint32_t y;
} IO;

typedef IO Input;
typedef IO Output;

typedef struct {
	uint32_t count;
	Input** items;
} IOs;

typedef IOs Inputs;
typedef IOs Outputs;

typedef struct {
	char* name[MAX_NAME_LEN];
	uint16_t bits;
	uint16_t width;
	uint16_t height;
	uint16_t pin_count;
	Pin** pins;
} FF;

typedef struct {
	char* name[MAX_NAME_LEN];
	uint16_t width;
	uint16_t height;
	uint16_t pin_count;
	Pin** pins;
} Gate;

typedef struct {
	uint32_t count;
	FF** items;
} FFs;

typedef struct {
	uint32_t count;
	Gate** items;
} Gates;

typedef struct {
	uint32_t x;
	uint32_t y;
	char* inst_name[MAX_NAME_LEN];
	char* lib_cell_name[MAX_NAME_LEN];
} Inst;

typedef struct {
	uint32_t count;
	Inst** items;
} Insts;

typedef struct {
	char instName[MAX_NAME_LEN];
	char libPinName[MAX_NAME_LEN];
} NetPin;

typedef struct {
    char name[MAX_NAME_LEN];
    int pinCount;
    NetPin** pins;
} Net;

typedef struct {
	uint32_t count;
	Net** items;
} Nets;

typedef struct {
	uint32_t start_x;
	uint32_t start_y;
	uint32_t width;
	uint32_t height;
	uint32_t totalNumOfSites;
} PlacementsRows;

typedef struct {
	uint32_t count;
	PlacementsRows** items;
} PlacementsRowsSet;

typedef struct {
    char libCellName[MAX_NAME_LEN];
    double delay;
} QpinDelay;

typedef struct {
	uint32_t count;
	QpinDelay** items;
} QpinDelays;

typedef struct {
    char instanceCellName[MAX_NAME_LEN];
    char pinName[MAX_NAME_LEN];
    double slack;
} TimingSlack;

typedef struct {
	uint32_t count;
	TimingSlack** items;
} TimingSlacks;

typedef struct {
    char libCellName[MAX_NAME_LEN];
    double powerConsumption;
} GatePower;

typedef struct {
	uint32_t count;
	GatePower** items;
} GatePowers;

typedef struct {
    double coefficient;
} DisplacementDelay;

typedef struct {
    uint32_t width;
	uint32_t height; 
	double maxUtil;
} Bin;

int place_main(FF, Gate, Inst*, Pin* inputs, uint32_t inputLen, Pin* outputs, uint32_t outputLen, PlacementsRows*, double DisplacementDelay,  double* timeslack, uint32_t slack_len, double Alpha, double Lambda, double Die_st_x/* Die start x*/, double Die_st_y, double Die_width, double Die_height, PlacementsRowsSet* rt/*return*/);

#endif // PLACE_H
