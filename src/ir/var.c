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
