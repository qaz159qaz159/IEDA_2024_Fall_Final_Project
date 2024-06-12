#include "utils.h"

int compare_ff(const void* a, const void* b) {
    FF* ff_a = *(FF**)a;
    FF* ff_b = *(FF**)b;

    int size_a = ff_a->width * ff_a->height;
    int size_b = ff_b->width * ff_b->height;

    return size_b - size_a;
}

FF** extract_ff_array(FFs* ff_blocks, size_t* count) {
    *count = ff_blocks->count;
    FF** ff_array = (FF**)malloc(*count * sizeof(FF*));

    FF* current_ff;
    FF* tmp;
    size_t i = 0;

    HASH_ITER(hh, ff_blocks->map, current_ff, tmp) {
        ff_array[i++] = current_ff;
    }

    return ff_array;
}

void replace_ff_map(FFs* ff_blocks, FF** ff_array, size_t count) {
    FF* current_ff;
    FF* tmp;

    HASH_ITER(hh, ff_blocks->map, current_ff, tmp) {
        HASH_DEL(ff_blocks->map, current_ff);
    }

    for (size_t i = 0; i < count; i++) {
        HASH_ADD_STR(ff_blocks->map, name, ff_array[i]);
    }
}

void sort_ff_by_size(FFs* ff_blocks) {
    size_t count;
    FF** ff_array = extract_ff_array(ff_blocks, &count);

    qsort(ff_array, count, sizeof(FF*), compare_ff);

    replace_ff_map(ff_blocks, ff_array, count);

    free(ff_array);
}

// Function to find the Nets containing a specific Pin by instName
Net* find_net_by_inst_name(InstNetMapping* mappings, size_t count, const char* instName) {
    for (size_t i = 0; i < count; i++) {
        // printf("Instance name: %s\n", mappings[i].instName);
        if (strcmp(mappings[i].instName, instName) == 0) {
            return mappings[i].net;
        }
    }

    return NULL;
}

// Function to populate the mapping between instances and nets
InstNetMapping* populate_inst_net_mapping(Nets* nets, size_t* count) {
    *count = 0;
    // Count the number of pins
    Net* net;
    NetPin* pin;
    for (net = nets->map; net != NULL; net = net->hh.next) {
        for (pin = net->map; pin != NULL; pin = pin->hh.next) {
            (*count)++;
        }
    }

    InstNetMapping* mappings = (InstNetMapping*)malloc(*count * sizeof(InstNetMapping));
    if (mappings == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(1);
    }

    size_t i = 0;
    for (net = nets->map; net != NULL; net = net->hh.next) {
        for (pin = net->map; pin != NULL; pin = pin->hh.next) {
            if (i >= *count) {
                fprintf(stderr, "Index out of bounds\n");
                exit(1);
            }
            strcpy(mappings[i].instName, pin->instName);
            mappings[i].net = net;
            i++;
        }
    }

    return mappings;
}

// Return an array of n-bit-FF library cells
FF** extract_ff_lib_with_n_bits(FFs* ff_lib, uint16_t n_bits, size_t* count) {
    size_t i = 0;
    FF* current_ff = NULL;

    // iterate through all ff lib cells to find the number of n-bit-FF lib cells
    for(current_ff = ff_lib->map; current_ff; current_ff = current_ff->hh.next) {
        if(current_ff->bits == n_bits) i++;
    }
    *count = i;
    FF** array = (FF**)malloc(i * sizeof(FF*));
    // iterate again and add FFs with n bits to the array
    i = 0;
    for(current_ff = ff_lib->map; current_ff; current_ff = current_ff->hh.next) {
        if(current_ff->bits == n_bits) {
            array[i] = current_ff;
            i++;
        }
    }
    return array;
}

// Return an array of used/unused n-bit-FF instances
Inst** extract_ff_insts_with_n_bits(Insts* insts, FFs* ff_lib, uint16_t n_bits, uint8_t used, size_t* count) {
    size_t i = 0;
    FF* current_ff_lib = NULL;
    Inst* current_inst = NULL;

    // iterate through all FF instances to find the number of n-bit-FF instances
    for(current_inst = insts->map; current_inst; current_inst = current_inst->hh.next) {
        HASH_FIND_STR(ff_lib->map, current_inst->lib_cell_name, current_ff_lib);
        if (!current_ff_lib) continue;
        if((current_inst->isUsed == used) && (current_ff_lib->bits == n_bits)) i++;
    }
    *count = i;
    Inst** array = (Inst**)malloc(i * sizeof(Inst*));
    i = 0;
    // iterate again and add to the array
    for(current_inst = insts->map; current_inst; current_inst = current_inst->hh.next) {
        HASH_FIND_STR(ff_lib->map, current_inst->lib_cell_name, current_ff_lib);
        if (!current_ff_lib) continue;
        if((current_inst->isUsed == used) && (current_ff_lib->bits == n_bits)) {
            array[i] = current_inst;
            i++;
        }
    }
    return array;
}