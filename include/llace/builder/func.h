#ifndef LLACE_BUILDER_FUNCTION_H
#define LLACE_BUILDER_FUNCTION_H

#include <llace/ir/function.h>

#ifdef __cplusplus
extern "C" {
#endif

// Module structure
typedef struct llace_builder_function {
  llace_function_t func;

  // variable hash table
  // utilize variable names
} llace_builder_function_t;

llace_error_t llace_build_function_init(llace_builder_function_t *func);
llace_error_t llace_build_function_destroy(llace_builder_function_t *func);

#ifdef __cplusplus
}
#endif

#endif // LLACE_BUILDER_FUNCTION_H