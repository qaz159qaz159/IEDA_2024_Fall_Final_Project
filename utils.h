#include "place.h"

int compare_ff(const void* a, const void* b);
FF** extract_ff_array(FFs* ff_blocks, size_t* count);
void replace_ff_map(FFs* ff_blocks, FF** ff_array, size_t count);
void sort_ff_by_size(FFs* ff_blocks);
Net* find_net_by_inst_name(InstNetMapping* mappings, size_t count, const char* instName);
InstNetMapping* populate_inst_net_mapping(Nets* nets, size_t* count);