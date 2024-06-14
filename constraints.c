#include "constraints.h"

#define max_displacement 10000
#define MAX_Y_DIS 6000

// uint32_t manhattan_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
//     return abs((int)x1 - (int)x2) + abs((int)y1 - (int)y2);
// }

int place_ff(Inst* old_inst, Inst* new_inst, Grid* grid, uint32_t max_distance) {
    // 開始搜索的位置，上下各五格，左右各五格，共100個cell的index
    uint32_t start_x = old_inst->x;
    uint32_t start_y = old_inst->y;
    uint32_t cell_index_table[10][10] = {0};
    uint32_t over = 0;
    for (uint32_t radius_x = start_x - 5 * grid->GRID_SIZE_X; radius_x < start_x + 5 * grid->GRID_SIZE_X; radius_x += grid->GRID_SIZE_X) {
        if ((radius_x > grid->start_x + grid->cols * grid->GRID_SIZE_X) || (radius_x < grid->start_x)) {
            over = 1;
            break;
        }
        for (uint32_t radius_y = start_y - 5 * grid->GRID_SIZE_Y; radius_y < start_y + 5 * grid->GRID_SIZE_Y; radius_y += grid->GRID_SIZE_Y) {
            if ((radius_y > grid->start_y + grid->rows * grid->GRID_SIZE_Y) || (radius_y < grid->start_y)) {
                over = 1;
                break;
            }
            cell_index_table[(radius_x + 5 * grid->GRID_SIZE_X - start_x) / grid->GRID_SIZE_X][(radius_y + 5 * grid->GRID_SIZE_Y - start_y) / grid->GRID_SIZE_Y] = get_cell_index(grid, radius_x, radius_y);
        }
    }
    if (over) {
        return FALSE;
    }


    // 確認這個inst需要幾格cell
    uint32_t new_inst_cell_width = (new_inst->width + grid->GRID_SIZE_X - 1) / grid->GRID_SIZE_X;
    uint32_t new_inst_cell_height = (new_inst->height + grid->GRID_SIZE_Y - 1) / grid->GRID_SIZE_Y;
    // printf("new_inst_cell_width: %u, new_inst_cell_height: %u\n", new_inst_cell_width, new_inst_cell_height);

    // 開始搜尋，看看在這10*10的cell中，是否有合適的位置
    uint32_t overlap = 0;
    for (uint32_t i = 0; i < 10; ++i) {
        for (uint32_t j = 0; j < 10; ++j) {
            overlap = 0;
            uint32_t cell_index = cell_index_table[i][j];
            for (uint32_t k = 0; k < new_inst_cell_width; ++k) {
                for (uint32_t l = 0; l < new_inst_cell_height; ++l) {
                    uint32_t index = cell_index_table[i + k][j + l];
                    GridCell* cell = grid->cells[index];
                    if (cell) {
                        overlap = 1;
                        break;
                    }
                }
                if (overlap) break;
            }
            if (!overlap) {
                get_position_by_index(grid, cell_index, &new_inst->x, &new_inst->y);
                // printf("FF placed at (%u, %u)\n", new_inst->x, new_inst->y);
                return TRUE;
            }
        }
    }
    return FALSE;
}