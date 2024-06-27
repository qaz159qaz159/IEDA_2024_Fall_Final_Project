//
// Created by mhhsu on 2024/6/27.
//

#ifndef IEDA_FINAL_PROJECT_OPTIMIZATION_H
#define IEDA_FINAL_PROJECT_OPTIMIZATION_H

#include <memory>
#include "structure.h"
#include "grid.h"

void bank_flip_flops(const shared_ptr<Insts>& instances, const shared_ptr<Nets>& nets, const shared_ptr<FFs>& ff_blocks, shared_ptr<Grid>& grid);

#endif //IEDA_FINAL_PROJECT_OPTIMIZATION_H
