#ifndef PLACE_H
#define PLACE_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "uthash.h"

#define MAX_NAME_LEN 16

typedef struct {
	char name[MAX_NAME_LEN];
	uint16_t x;
	uint16_t y;
	UT_hash_handle hh;
} Pin;

typedef struct {
	char name[MAX_NAME_LEN];
	uint32_t x;
	uint32_t y;
	UT_hash_handle hh;
} IO;

typedef IO Input;
typedef IO Output;

typedef struct {
	uint32_t count;
	IO* map;
} IOs;

typedef IOs Inputs;
typedef IOs Outputs;

typedef struct {
	char name[MAX_NAME_LEN];
	uint16_t bits;
	uint16_t width;
	uint16_t height;
	uint16_t pin_count;
	Pin* map;
	UT_hash_handle hh;
} FF;

typedef struct {
	char name[MAX_NAME_LEN];
	uint16_t width;
	uint16_t height;
	uint16_t pin_count;
	Pin* map;
	UT_hash_handle hh;
} Gate;

typedef struct {
	uint32_t count;
	FF* map;
} FFs;

typedef struct {
	uint32_t count;
	Gate* map;
} Gates;

typedef struct {
	char inst_name[MAX_NAME_LEN];
	char lib_cell_name[MAX_NAME_LEN];
	uint32_t x;
	uint32_t y;
	UT_hash_handle hh;
} Inst;

typedef struct {
	uint32_t count;
	Inst* map;
} Insts;

typedef struct {
	char instName[MAX_NAME_LEN];
	char libPinName[MAX_NAME_LEN];
	UT_hash_handle hh;
} NetPin;

typedef struct {
	char name[MAX_NAME_LEN];
	int pinCount;
	NetPin* map;
	UT_hash_handle hh;
} Net;

typedef struct {
	uint32_t count;
	Net* map;
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
	UT_hash_handle hh;
} QpinDelay;

typedef struct {
	uint32_t count;
	QpinDelay* map;
} QpinDelays;

typedef struct {
	char instanceCellName[MAX_NAME_LEN];
	char pinName[MAX_NAME_LEN];
	double slack;
	UT_hash_handle hh;
} TimingSlack;

typedef struct {
	uint32_t count;
	TimingSlack* map;
} TimingSlacks;

typedef struct {
	char libCellName[MAX_NAME_LEN];
	double powerConsumption;
	UT_hash_handle hh;
} GatePower;

typedef struct {
	uint32_t count;
	GatePower* map;
} GatePowers;

typedef struct {
	double coefficient;
} DisplacementDelay;

typedef struct {
	uint32_t width;
	uint32_t height; 
	double maxUtil;
} Bin;

typedef struct {
	uint32_t start_x;
	uint32_t start_y;
	uint32_t width;
	uint32_t height;
} Die;

int place_main(FFs ffs, Gates gates, Inputs inputs, Outputs outputs, Insts insts, Nets nets, PlacementsRowsSet placementRowsSet, DisplacementDelay displacementDelay, Bin bin, Die die, double alpha, double lambda);

#endif // PLACE_H
