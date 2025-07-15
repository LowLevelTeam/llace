#include <llace/ir/var.h>
#include <llace/detail/common.h>
#include <string.h>

// ================ Variable Implementation ================ //

llace_error_t llace_variable_init(llace_variable_t *variable) {
  if (!variable) return LLACE_ERROR_BADARG;
  
  memset(variable, 0, sizeof(llace_variable_t));
  variable->name = 0;
  variable->type = 0;
  variable->attrval = 0;
  
  // Initialize value as void
  llace_value_init(&variable->value);
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_variable_destroy(llace_variable_t *variable) {
  if (!variable) return LLACE_ERROR_BADARG;
  
  // Destroy the value
  llace_value_destroy(&variable->value);
  
  memset(variable, 0, sizeof(llace_variable_t));
  
  return LLACE_ERROR_NONE;
}

// ================ Variable Builders ================ //

llace_error_t llace_variable_set_name(llace_variable_t *variable, llace_nameref_t name) {
  if (!variable) return LLACE_ERROR_BADARG;
  
  variable->name = name;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_variable_set_type(llace_variable_t *variable, llace_typeref_t type) {
  if (!variable) return LLACE_ERROR_BADARG;
  
  variable->type = type;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_variable_set_value(llace_variable_t *variable, const llace_value_t *value) {
  if (!variable || !value) return LLACE_ERROR_BADARG;
  
  // Destroy existing value
  llace_value_destroy(&variable->value);
  
  // Copy new value
  variable->value = *value;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_variable_set_const(llace_variable_t *variable, bool is_const) {
  if (!variable) return LLACE_ERROR_BADARG;
  
  variable->attr._const = is_const ? 1 : 0;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_variable_set_volatile(llace_variable_t *variable, bool is_volatile) {
  if (!variable) return LLACE_ERROR_BADARG;
  
  variable->attr._volatile = is_volatile ? 1 : 0;
  return LLACE_ERROR_NONE;
}

// ================ Variable Accessors ================ //

llace_nameref_t llace_variable_get_name(const llace_variable_t *variable) {
  if (!variable) return 0;
  return variable->name;
}

llace_typeref_t llace_variable_get_type(const llace_variable_t *variable) {
  if (!variable) return 0;
  return variable->type;
}

const llace_value_t *llace_variable_get_value(const llace_variable_t *variable) {
  if (!variable) return NULL;
  return &variable->value;
}

bool llace_variable_is_const(const llace_variable_t *variable) {
  if (!variable) return false;
  return variable->attr._const == 1;
}

bool llace_variable_is_volatile(const llace_variable_t *variable) {
  if (!variable) return false;
  return variable->attr._volatile == 1;
}