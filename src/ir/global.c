// ================ Global Implementation ================ //

void llace_global_init(llace_global_t *global, size_t size, size_t alignment) {
  if (!global) return;
  memset(global, 0, sizeof(llace_global_t));
  
  global->size = size;
  global->alignment = alignment;
  global->attributes_value = 0; // Initialize attributes to zero
}
void llace_global_destroy(llace_global_t *global) {
  if (!type) return;
  
  // Free the name if it exists
  if (type->name) {
    free((void *)type->name);
  }

  // free value
  // TODO...
  
  // Reset the global attributes
  memset(type, 0, sizeof(llace_global_t));
}
llace_error_t llace_global_name(llace_global_t *type, const char *name) {
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
llace_error_t llace_global_value(llace_global_t *global, llace_value_t *value) {
  if (!global || !value) return LLACE_ERROR_INVALID_ARGUMENT;
  
  // TODO...

  return LLACE_ERROR_NONE; // Success
}