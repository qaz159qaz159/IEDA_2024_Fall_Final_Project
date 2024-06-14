#ifndef UTILS_H
#define UTILS_H

#include "place.h"
#include <math.h>

int compare_ff(const void* a, const void* b);
FF** extract_ff_array(FFs* ff_blocks, size_t* count);
void replace_ff_map(FFs* ff_blocks, FF** ff_array, size_t count);
void sort_ff_by_size(FFs* ff_blocks);
Net* find_net_by_inst_name(InstNetMapping* mappings, size_t count, const char* instName);
InstNetMapping* populate_inst_net_mapping(Nets* nets, size_t* count);
uint32_t manhattan_distance_inst(Inst* inst1, Inst* inst2);
uint32_t manhattan_distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

// FF** extract_ff_lib_with_n_bits(FFs* ff_lib, uint16_t n_bits, size_t* count);
// Inst** extract_ff_insts_with_n_bits(Insts* insts, FFs* ff_lib, uint16_t n_bits, uint8_t used, size_t* count);
// void calculate_FF_innate_cost(FFs* ff_lib, GatePowers* powers, const double beta, const double gamma);
// int compare_ff_innate_cost(const void *a, const void *b);
// void sort_ff_by_innate_cost(FFs* ff_lib);

#endif // UTILS_H