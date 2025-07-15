// ================ Function Implementation ================ //

void llace_function_init(llace_function_t *func, llace_abi_t abi) {
  if (!func) return;
  memset(func, 0, sizeof(llace_function_t));
  
  func->abi = abi;
  func->return_count = 0;
  func->parameter_count = 0;
  func->instruction_count = 0; // Initialize instruction count to zero
  func->attributes_value = 0; // Initialize attributes to zero
}
void llace_function_destroy(llace_function_t *func) {
  if (!type) return;
  
  // Free the name if it exists
  if (type->name) {
    free((void *)type->name);
  }

  // Free parameters and returns

  // Free instructions
  // TODO...

  // Reset the function attributes
  memset(type, 0, sizeof(llace_function_t));
}
llace_error_t llace_function_name(llace_function_t *type, const char *name) {
  if (!type || !name) return LLACE_ERROR_INVALID_ARGUMENT;
  
  // Free existing name if any
  if (type->name) {
    free((void *)type->name);
  }
  
  // Allocate new name
  type->name = strdup(name);
  if (!type->name) {
    return LLACE_ERROR_NOMEM; // Memory allocation failed
  }

  return LLACE_ERROR_NONE; // Success
}
llace_error_t llace_function_value(llace_function_t *func, llace_value_t *value) {
  if (!global || !value) return LLACE_ERROR_INVALID_ARGUMENT;
  
  // TODO...

  return LLACE_ERROR_NONE; // Success
}
llace_error_t llace_function_params(llace_function_t *func, size_t count, llace_type_t *type) {

}
llace_error_t llace_function_rets(llace_function_t *func, size_t count, llace_type_t *type) {

}