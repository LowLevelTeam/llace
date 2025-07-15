#include <llace/ir/global.h>
#include <llace/detail/common.h>
#include <string.h>

// ================ Global Implementation ================ //

llace_error_t llace_global_init(llace_global_t *global) {
  if (!global) return LLACE_ERROR_BADARG;
  
  memset(global, 0, sizeof(llace_global_t));
  global->name = 0;
  global->type = 0;
  global->attrval = 0;
  
  // Initialize value as void
  llace_value_init(&global->value);
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_global_destroy(llace_global_t *global) {
  if (!global) return LLACE_ERROR_BADARG;
  
  // Destroy the value
  llace_value_destroy(&global->value);
  
  memset(global, 0, sizeof(llace_global_t));
  
  return LLACE_ERROR_NONE;
}

// ================ Global Builders ================ //

llace_error_t llace_global_set_name(llace_global_t *global, llace_nameref_t name) {
  if (!global) return LLACE_ERROR_BADARG;
  
  global->name = name;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_global_set_type(llace_global_t *global, llace_typeref_t type) {
  if (!global) return LLACE_ERROR_BADARG;
  
  global->type = type;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_global_set_value(llace_global_t *global, const llace_value_t *value) {
  if (!global || !value) return LLACE_ERROR_BADARG;
  
  // Destroy existing value
  llace_value_destroy(&global->value);
  
  // Copy new value
  global->value = *value;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_global_set_extern(llace_global_t *global, bool is_extern) {
  if (!global) return LLACE_ERROR_BADARG;
  
  global->attr._extern = is_extern ? 1 : 0;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_global_set_public(llace_global_t *global, bool is_public) {
  if (!global) return LLACE_ERROR_BADARG;
  
  global->attr._public = is_public ? 1 : 0;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_global_set_const(llace_global_t *global, bool is_const) {
  if (!global) return LLACE_ERROR_BADARG;
  
  global->attr._const = is_const ? 1 : 0;
  return LLACE_ERROR_NONE;
}

// ================ Global Accessors ================ //

llace_nameref_t llace_global_get_name(const llace_global_t *global) {
  if (!global) return 0;
  return global->name;
}

llace_typeref_t llace_global_get_type(const llace_global_t *global) {
  if (!global) return 0;
  return global->type;
}

const llace_value_t *llace_global_get_value(const llace_global_t *global) {
  if (!global) return NULL;
  return &global->value;
}

bool llace_global_is_extern(const llace_global_t *global) {
  if (!global) return false;
  return global->attr._extern == 1;
}

bool llace_global_is_public(const llace_global_t *global) {
  if (!global) return false;
  return global->attr._public == 1;
}

bool llace_global_is_const(const llace_global_t *global) {
  if (!global) return false;
  return global->attr._const == 1;
}