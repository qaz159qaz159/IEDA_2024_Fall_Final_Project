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
    for (size_t i = 0; i < 20; i++) {
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
