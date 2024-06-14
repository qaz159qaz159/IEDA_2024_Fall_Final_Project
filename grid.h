//
// Created by Ming-Hao Hsu on 2024/6/15.
//

#ifndef IEDA_FINAL_PROJECT_GRID_H
#define IEDA_FINAL_PROJECT_GRID_H

#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>
#include <memory>
#include "structure.h"

using namespace std;

class GridCell {
public:
    shared_ptr<Inst> instance;
    shared_ptr<GridCell> next;

    GridCell() : instance(nullptr), next(nullptr) {}
};

class Grid {
public:
    uint32_t rows;
    uint32_t cols;
    uint32_t GRID_SIZE_X;
    uint32_t GRID_SIZE_Y;
    uint32_t start_x;
    uint32_t start_y;
    vector<shared_ptr<GridCell> > cells;

    Grid() : rows(0), cols(0), GRID_SIZE_X(0), GRID_SIZE_Y(0), start_x(0), start_y(0) {}

    void create_grid(uint32_t width, uint32_t height, uint32_t gridSizeX, uint32_t gridSizeY, uint32_t startX,
                     uint32_t startY);

    void free_grid();

    void insert_to_grid(const shared_ptr<Inst> &instance);

    void get_position_by_index(uint32_t index, uint32_t &x, uint32_t &y) const;

    uint32_t get_cell_index(uint32_t x, uint32_t y) const;

    bool check_overlap_grid(const shared_ptr<Inst> &instance);

};


#endif //IEDA_FINAL_PROJECT_GRID_H