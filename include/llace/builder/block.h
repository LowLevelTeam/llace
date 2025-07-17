#ifndef LLACE_BUILDER_BLOCK_H
#define LLACE_BUILDER_BLOCK_H

#include <llace/ir/block.h>

#ifdef __cplusplus
extern "C" {
#endif

// Module structure
typedef struct llace_builder_block {
  llace_block_t block;

  // variable hash table
  // utilize variable names  
} llace_builder_block_t;

llace_error_t llace_build_block_init(llace_builder_block_t *block);
llace_error_t llace_build_block_destroy(llace_builder_block_t *block);

#ifdef __cplusplus
}
#endif

#endif // LLACE_BUILDER_BLOCK_H