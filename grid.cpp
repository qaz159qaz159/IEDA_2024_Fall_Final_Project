//
// Created by Ming-Hao Hsu on 2024/6/15.
//

#include "grid.h"

void Grid::create_grid(uint32_t width, uint32_t height, uint32_t gridSizeX, uint32_t gridSizeY, uint32_t startX,
                       uint32_t startY) {
    this->GRID_SIZE_X = gridSizeX;
    this->GRID_SIZE_Y = gridSizeY;
    this->rows = (height + gridSizeY - 1) / gridSizeY;
    this->cols = (width + gridSizeX - 1) / gridSizeX;
    this->start_x = startX;
    this->start_y = startY;
    this->cells.resize(rows * cols);

//    cout << "Grid created with " << rows << " rows and " << cols << " cols\n";
//    cout << "Grid cell size: " << gridSizeX << " x " << gridSizeY << "\n";
//    cout << "Grid start position: (" << startX << ", " << startY << ")\n";
}

void Grid::free_grid() {
    cells.clear();
}

uint32_t Grid::get_cell_index(uint32_t x, uint32_t y) const {
    uint32_t col = (x - start_x) / GRID_SIZE_X;
    uint32_t row = (y - start_y) / GRID_SIZE_Y;
    return row * cols + col;
}

void Grid::insert_to_grid(const shared_ptr<Inst> &instance) {
    uint32_t index = get_cell_index(instance->x, instance->y);
    auto new_cell = make_shared<GridCell>();
    new_cell->instance = instance;
    new_cell->next = cells[index];
    cells[index] = new_cell;

    // cout << "Instance " << instance->inst_name << " inserted to grid cell " << index << "\n";
}

bool Grid::check_overlap_grid(const shared_ptr<Inst> &instance) {
    uint32_t x_min = (instance->x - start_x) / GRID_SIZE_X;
    uint32_t y_min = (instance->y - start_y) / GRID_SIZE_Y;
    uint32_t x_max = (instance->x + instance->width - start_x) / GRID_SIZE_X;
    uint32_t y_max = (instance->y + instance->height - start_y) / GRID_SIZE_Y;

    for (uint32_t row = y_min; row <= y_max; ++row) {
        for (uint32_t col = x_min; col <= x_max; ++col) {
            uint32_t index = row * cols + col;
            auto cell = cells[index];
            while (cell) {
                auto other = cell->instance;
                if (other != instance &&
                    instance->x < other->x + other->width &&
                    instance->x + instance->width > other->x &&
                    instance->y < other->y + other->height &&
                    instance->y + instance->height > other->y) {
//                    cout << "Overlap detected between " << instance->inst_name << " and " << other->inst_name << "\n";
                    return true;
                }
                cell = cell->next;
            }
        }
    }
//    cout << "No overlap detected for " << instance->inst_name << "\n";
    return false;
}

void Grid::get_position_by_index(uint32_t index, uint32_t &x, uint32_t &y) const {
    uint32_t row = index / cols;
    uint32_t col = index % cols;
    x = col * GRID_SIZE_X;
    y = row * GRID_SIZE_Y;
}
