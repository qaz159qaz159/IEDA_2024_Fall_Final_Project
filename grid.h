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
#include <iostream>
#include "structure.h"

class GridCell {
public:
    shared_ptr<Inst> instance;
    shared_ptr<GridCell> next;

    GridCell() : instance(nullptr), next(nullptr) {};
};

class Grid {
public:
    uint32_t rows;
    uint32_t cols;
    uint32_t gridSizeX;
    uint32_t gridSizeY;
    uint32_t startX;
    uint32_t startY;
    vector<shared_ptr<GridCell> > cells;

    Grid() : rows(0), cols(0), gridSizeX(0), gridSizeY(0), startX(0), startY(0) {};

    void create_grid(uint32_t width, uint32_t height, uint32_t gridSizeX, uint32_t gridSizeY, uint32_t startX,
                     uint32_t startY);

    void free_grid();

    void insert_to_grid(const shared_ptr<Inst> &instance);

    void get_position_by_index(uint32_t index, uint32_t &x, uint32_t &y) const;

    [[nodiscard]] uint32_t get_cell_index(uint32_t x, uint32_t y) const;

    [[nodiscard]] vector<uint32_t> get_needed_cells(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const;

    bool check_overlap_grid(const shared_ptr<Inst> &instance);

};


#endif //IEDA_FINAL_PROJECT_GRID_H