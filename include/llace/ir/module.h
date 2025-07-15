#ifndef LLACE_IR_MODULE_H
#define LLACE_IR_MODULE_H

#include <llace/ir/common.h>
#include <llace/ir/type.h>
#include <llace/ir/global.h>
#include <llace/ir/func.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================ IR ================ //

// Module structure
typedef struct llace_module {
  llace_nameref_t name; // Module name (typically filename)
  llace_array_t nametab; // const char *
  llace_array_t types; // llace_type_t
  llace_array_t globs; // llace_global_t
  llace_array_t funcs; // llace_function_t
} llace_module_t;

// ================ Implementation ================ //
llace_error_t llace_module_init(llace_module_t *module, const char *name, size_t namelen);
llace_error_t llace_module_destroy(llace_module_t *module);

// ================ Builders ================ //
llace_error_t llace_module_add_name(llace_module_t *module, const char *name, size_t namelen, llace_nameref_t *ref);
llace_error_t llace_module_add_type(llace_module_t *module, const llace_type_t *type);
llace_error_t llace_module_add_global(llace_module_t *module, const llace_global_t *global);
llace_error_t llace_module_add_function(llace_module_t *module, const llace_function_t *function);

// ================ Accessors ================ //
char *llace_module_get_name(llace_module_t *module, llace_nameref_t ref);
llace_type_t *llace_module_get_type(llace_module_t *module, llace_typeref_t ref);
llace_global_t *llace_module_get_global(llace_module_t *module, llace_globalref_t ref);
llace_function_t *llace_module_get_function(llace_module_t *module, llace_funcref_t ref);

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_MODULE_H