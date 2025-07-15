#include <llace/ir.h>
#include <string.h>


// ================ Module Implementation ================ //

llace_error_t llace_module_init(llace_module_t *module, const char *name, size_t namelen) {
  if (!module) return LLACE_ERROR_BADARG;
  memset(module, 0, sizeof(llace_module_t));

  // TODO: Allocate space for and copy name into buffer

  return LLACE_ERROR_NONE; // Success
}

llace_error_t llace_module_destroy(llace_module_t *module) {
  if (!module) return LLACE_ERROR_BADARG;
  
  // Free types
  if (module->types) {
    // TODO...
  }
  
  // Free globals
  if (module->globs) {
    // TODO...
  }
  
  // Free functions
  if (module->funcs) {
    // TODO...
  }
  
  memset(module, 0, sizeof(llace_module_t));
  return LLACE_ERROR_NONE;
}

llace_error_t llace_module_add_name(llace_module_t *module, const char *name, llace_nameref_t *ref) {
  // TODO...
  return LLACE_ERROR_NONE; // Success
}

llace_error_t llace_module_add_type(llace_module_t *module, const llace_type_t *type, llace_typeref_t *ref) {
  // TODO...
  return LLACE_ERROR_NONE; // Success
}

llace_error_t llace_module_add_global(llace_module_t *module, const llace_global_t *global, llace_globalref_t *ref) {
  // TODO...
  return LLACE_ERROR_NONE; // Success
}

llace_error_t llace_module_add_function(llace_module_t *module, const llace_function_t *function, llace_funcref_t *ref) {
  // TODO...
  return LLACE_ERROR_NONE; // Success
}

char *llace_module_get_name(llace_module_t *module, llace_nameref_t ref) {
  // TODO...
  return NULL;
}
llace_type_t *llace_module_get_type(llace_module_t *module, llace_typeref_t ref) {
  // TODO...
  return NULL;
}
llace_global_t *llace_module_get_global(llace_module_t *module, llace_globalref_t ref) {
  // TODO...
  return NULL;
}
llace_function_t *llace_module_get_function(llace_module_t *module, llace_funcref_t ref) {
  // TODO...
  return NULL;
}