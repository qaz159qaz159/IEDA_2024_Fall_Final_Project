#include "grid.h"

// Create a grid
void create_grid(Grid* grid, uint32_t width, uint32_t height, uint32_t GRID_SIZE_X, uint32_t GRID_SIZE_Y, uint32_t start_x, uint32_t start_y) {
    grid->GRID_SIZE_X = GRID_SIZE_X;
    grid->GRID_SIZE_Y = GRID_SIZE_Y;
    grid->rows = height / GRID_SIZE_Y;
    grid->cols = width / GRID_SIZE_X;
    grid->start_x = start_x;
    grid->start_y = start_y;
    grid->cells = (GridCell**)calloc(grid->rows * grid->cols, sizeof(GridCell*));
    for (uint32_t i = 0; i < grid->rows * grid->cols; ++i) {
        grid->cells[i] = NULL;
    }

    printf("Grid created with %u rows and %u cols\n", grid->rows, grid->cols);
    // printf("Grid size: %u x %u\n", grid->width, grid->height);
    printf("Grid cell size: %u x %u\n", grid->GRID_SIZE_X, grid->GRID_SIZE_Y);
    printf("Grid start position: (%u, %u)\n", grid->start_x, grid->start_y);
}

// Free the grid
void free_grid(Grid* grid) {
    for (uint32_t i = 0; i < grid->rows * grid->cols; ++i) {
        GridCell* cell = grid->cells[i];
        while (cell) {
            GridCell* tmp = cell;
            cell = cell->next;
            free(tmp);
        }
    }
    free(grid->cells);
    free(grid);
}

// Get the index of the cell in the grid
uint32_t get_cell_index(Grid* grid, uint32_t x, uint32_t y) {
    uint32_t row = (y - grid->start_y) / grid->GRID_SIZE_Y;
    uint32_t col = (x - grid->start_x) / grid->GRID_SIZE_X;
    return row * grid->cols + col;
}

// Insert an instance to the grid
void insert_to_grid(Grid* grid, Inst* instance) {
    uint32_t index = get_cell_index(grid, instance->x, instance->y);
    GridCell* cell = (GridCell*)malloc(sizeof(GridCell));
    cell->instance = instance;
    cell->next = grid->cells[index];
    grid->cells[index] = cell;
    // printf("Instance %s inserted to grid cell %u\n", instance->inst_name, index);
}

// Check if the instance overlaps with any other instance in the grid
bool check_overlap_grid(Grid* grid, Inst* instance) {
    uint32_t x_min = (instance->x - grid->start_x) / grid->GRID_SIZE_X;
    uint32_t y_min = (instance->y - grid->start_y) / grid->GRID_SIZE_Y;
    uint32_t x_max = (instance->x + instance->width - grid->start_x) / grid->GRID_SIZE_X;
    uint32_t y_max = (instance->y + instance->height - grid->start_y) / grid->GRID_SIZE_Y;

    for (uint32_t row = y_min; row <= y_max; ++row) {
        for (uint32_t col = x_min; col <= x_max; ++col) {
            uint32_t index = row * grid->cols + col;
            GridCell* cell = grid->cells[index];
            while (cell) {
                Inst* other = cell->instance;
                if (other != instance &&
                    instance->x < other->x + other->width &&
                    instance->x + instance->width > other->x &&
                    instance->y < other->y + other->height &&
                    instance->y + instance->height > other->y) {
                    printf("Overlap detected between %s and %s\n", instance->inst_name, other->inst_name);
                    return true;
                }
                cell = cell->next;
            }
        }
    }
    printf("No overlap detected for %s\n", instance->inst_name);
    return false;
}

void get_position_by_index(Grid* grid, uint32_t index, uint32_t* x, uint32_t* y) {
    uint32_t row = index / grid->cols;
    uint32_t col = index % grid->cols;
    *x = col * grid->GRID_SIZE_X;
    *y = row * grid->GRID_SIZE_Y;
}