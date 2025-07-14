#include <llace/ir.h>
#include <llace/detail/common.h>
#include <string.h>

// ================ ABI Utilities ================ //

const char *llace_abi_str(llace_abi_t abi) {
  switch (abi) {
    case LLACE_ABI_NULL: return "null";
    case LLACE_ABI_CDECL: return "cdecl";
    default: return "unknown";
  }
}

// ================ Type Implementation ================ //

static size_t llace_type_calculate_size(llace_type_kind_t kind, unsigned long bit_width) {
  switch (kind) {
    case LLACE_TYPE_VOID: return 0;
    case LLACE_TYPE_INT:
    case LLACE_TYPE_UNT:
      return (bit_width + 7) / 8; // Round up to nearest byte
    case LLACE_TYPE_FLOAT:
      if (bit_width <= 32) return 4;
      if (bit_width <= 64) return 8;
      return 16; // long double
    case LLACE_TYPE_PTR:
      return sizeof(void*);
    default:
      return 0;
  }
}

static size_t llace_type_calculate_alignment(llace_type_kind_t kind, size_t size) {
  switch (kind) {
    case LLACE_TYPE_VOID: return 0; // no data to be alligned
    case LLACE_TYPE_INT:
    case LLACE_TYPE_UNT:
    case LLACE_TYPE_FLOAT:
      // Align to size, but cap at pointer size
      if (size <= 1) return 1;
      if (size <= 2) return 2;
      if (size <= 4) return 4;
      return sizeof(void*);
    case LLACE_TYPE_PTR:
      return sizeof(void*);
    default:
      return -1;
  }
}

// Type functions
void llace_type_destroy(llace_type_t *type) {
  if (!type) return;
  
  if (type->name) {
    free((void *)type->name);
  }
  // Free any other resources associated with the type if necessary
  // This could include freeing arrays, structs, etc.
  // For now, we just zero out the type structure since we don't dynamically allocate

  memset(type, 0, sizeof(llace_type_t));
}

void llace_type_init(llace_type_t *type, llace_type_kind_t kind, size_t size, size_t alignment) {
  if (!type) return;
  memset(type, 0, sizeof(llace_type_t));
  type->kind = kind;
  type->size = size;
  type->alignment = alignment;
}

llace_error_t llace_type_name(llace_type_t *type, const char *name) {
  if (!type || !name) return;
  
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

// ================ Module Implementation ================ //

llace_error_t llace_module_init(llace_module_t *module, const char *name) {
  if (!module) return;
  memset(module, 0, sizeof(llace_module_t));

  module->name = strdup(name);
  if (!module->name) {
    return LLACE_ERROR_NOMEM; // Memory allocation failed
  }
  return LLACE_ERROR_NONE; // Success
}

void llace_module_destroy(llace_module_t *module) {
  if (!module) return;
  
  // Free types
  if (module->types) {
    for (size_t i = 0; i < module->type_count; i++) {
      llace_type_destroy(&module->types[i]);
    }
    free(module->types);
  }
  
  // Free globals
  if (module->globals) {
    
  }
  
  // Free functions
  if (module->functions) {
    
  }
  
  memset(module, 0, sizeof(llace_module_t));
}

llace_error_t llace_module_add_type(llace_module_t *module, llace_type_t *type) {
  if (!module || !type) return;
  
  // Resize types array
  if (module->type_count >= module->type_capacity) {
    size_t new_capacity = module->type_capacity ? module->type_capacity * 2 : 8; // Start with a small capacity
    llace_type_t *new_types = realloc(module->types, new_capacity * sizeof(llace_type_t));
    if (!new_types) {
      return LLACE_ERROR_NOMEM; // Memory allocation failed
    }
    module->types = new_types;
    module->type_capacity = new_capacity;
  }
  
  // Copy type
  module->types[module->type_count] = *type;
  module->type_count++;

  return LLACE_ERROR_NONE; // Success
}

llace_error_t llace_module_add_global(llace_module_t *module, llace_global_t *global) {
  if (!module || !global) return;
  
  // Resize globals array
  if (module->global_count >= module->global_capacity) {
    size_t new_capacity = module->global_capacity ? module->global_capacity * 2 : 8; // Start with a small capacity
    llace_global_t *new_globals = realloc(module->globals, new_capacity * sizeof(llace_global_t));
    if (!new_globals) {
      return LLACE_ERROR_NOMEM; // Memory allocation failed
    }
    module->globals = new_globals;
    module->global_capacity = new_capacity;
  }
  
  // Copy global
  module->globals[module->global_count] = *global;
  module->global_count++;

  return LLACE_ERROR_NONE; // Success
}

llace_error_t llace_module_add_function(llace_module_t *module, llace_function_t *function) {
  if (!module || !function) return;
  
  // Resize functions array
  if (module->functions_count >= module->functions_capacity) {
    size_t new_capacity = module->functions_capacity ? module->functions_capacity * 2 : 8; // Start with a small capacity
    llace_function_t *new_functions = realloc(module->functions, new_capacity * sizeof(llace_function_t));
    if (!new_functions) {
      return LLACE_ERROR_NOMEM; // Memory allocation failed
    }
    module->functions = new_functions;
    module->functions_capacity = new_capacity;
  }
  
  // Copy function
  module->functions[module->functions_count] = *function;
  module->functions_count++;

  return LLACE_ERROR_NONE; // Success
}

llace_type_t *llace_module_get_type(llace_module_t *module, size_t index) {
  if (!module || index >= module->type_count) return NULL;
  return &module->types[index];
}
llace_global_t *llace_module_get_global(llace_module_t *module, size_t index) {
  if (!module || index >= module->global_count) return NULL;
  return &module->globals[index];
}
llace_function_t *llace_module_get_function(llace_module_t *module, size_t index) {
  if (!module || index >= module->functions_count) return NULL;
  return &module->functions[index];
}