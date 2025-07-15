#ifndef LLACE_IR_BLOCK_H
#define LLACE_IR_BLOCK_H

#include <llace/ir/common.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================================ IR ================================================ //

typedef struct llace_block {
  llace_array_t locals; // llace_variable_t[]
  llace_array_t instrs; // llace_instr_t[]
} llace_block_t;


llace_error_t llace_block_init(llace_block_t *block);
llace_error_t llace_block_destroy(llace_block_t *block);

// ================================================ Builder ================================================ /

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_BLOCK_H