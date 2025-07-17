#ifndef LLACE_BUILDER_MODULE_H
#define LLACE_BUILDER_MODULE_H

#include <llace/ir/module.h>

#ifdef __cplusplus
extern "C" {
#endif

// Module structure
typedef struct llace_builder {
  llace_module_t _module;

  // string hash table
  // helps stop deduplication

  // type hash table
  // utilize type names

  // global hash table
  // utilize global names

  // function hash table
  // utilize function names
} llace_builder_t;

llace_error_t llace_build_module_init(llace_builder_t *builder);
llace_error_t llace_build_module_destroy(llace_builder_t *builder);

#ifdef __cplusplus
}
#endif

#endif // LLACE_BUILDER_MODULE_H