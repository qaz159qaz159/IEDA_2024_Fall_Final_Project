#ifndef GRID_H
#define GRID_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "uthash.h"
#include "place.h"

typedef struct GridCell {
    Inst* instance;
    struct GridCell* next;
} GridCell;

typedef struct {
    uint32_t rows;
    uint32_t cols;
    uint32_t GRID_SIZE_X;
    uint32_t GRID_SIZE_Y;
    uint32_t start_x;
    uint32_t start_y;
    GridCell** cells;
} Grid;

void create_grid(Grid* grid, uint32_t width, uint32_t height, uint32_t GRID_SIZE_X, uint32_t GRID_SIZE_Y, uint32_t start_x, uint32_t start_y);
void free_grid(Grid* grid);
uint32_t get_cell_index(Grid* grid, uint32_t x, uint32_t y);
void insert_to_grid(Grid* grid, Inst* instance);
bool check_overlap_grid(Grid* grid, Inst* instance);
void get_position_by_index(Grid* grid, uint32_t index, uint32_t* x, uint32_t* y);

#endif // GRID_H