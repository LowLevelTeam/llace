#include <llace/ir/module.h>
#include <llace/detail/common.h>
#include <llace/mem.h>
#include <string.h>

// ================ Module Implementation ================ //

llace_error_t llace_module_init(llace_module_t *module, const char *name, size_t namelen) {
  if (!module) return LLACE_ERROR_BADARG;
  
  memset(module, 0, sizeof(llace_module_t));
  module->conf = NULL;
  module->name = 0;
  module->nametab = LLACE_HANDLE_INVALID;
  module->types = LLACE_HANDLE_INVALID;
  module->globs = LLACE_HANDLE_INVALID;
  module->funcs = LLACE_HANDLE_INVALID;
  
  // Initialize name table
  llace_error_t err = LLACE_MEM_ALLOC_ARRAY(char, 1024, &module->nametab);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Initialize small arrays
  err = LLACE_MEM_ALLOC_ARRAY(llace_type_t, 32, &module->types);
  if (err != LLACE_ERROR_NONE) {
    llace_mem_free(module->nametab);
    return err;
  }
  
  err = LLACE_MEM_ALLOC_ARRAY(llace_global_t, 32, &module->globs);
  if (err != LLACE_ERROR_NONE) {
    llace_mem_free(module->nametab);
    llace_mem_free(module->types);
    return err;
  }
  
  err = LLACE_MEM_ALLOC_ARRAY(llace_function_t, 32, &module->funcs);
  if (err != LLACE_ERROR_NONE) {
    llace_mem_free(module->nametab);
    llace_mem_free(module->types);
    llace_mem_free(module->globs);
    return err;
  }
  
  // Set module name if provided
  if (name) {
    size_t len = namelen > 0 ? namelen : strlen(name);
    err = llace_module_add_name(module, name, &module->name);
    if (err != LLACE_ERROR_NONE) {
      llace_module_destroy(module);
      return err;
    }
  }
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_module_destroy(llace_module_t *module) {
  if (!module) return LLACE_ERROR_BADARG;
  
  // Free all types
  if (module->types != LLACE_HANDLE_INVALID) {
    llace_type_t *types = LLACE_MEM_GET_TYPED(llace_type_t, module->types);
    size_t count = llace_mem_array_count(module->types);
    for (size_t i = 0; i < count; i++) {
      llace_type_destroy(&types[i]);
    }
    llace_mem_free(module->types);
  }
  
  // Free all globals
  if (module->globs != LLACE_HANDLE_INVALID) {
    llace_global_t *globs = LLACE_MEM_GET_TYPED(llace_global_t, module->globs);
    size_t count = llace_mem_array_count(module->globs);
    for (size_t i = 0; i < count; i++) {
      llace_global_destroy(&globs[i]);
    }
    llace_mem_free(module->globs);
  }
  
  // Free all functions
  if (module->funcs != LLACE_HANDLE_INVALID) {
    llace_function_t *funcs = LLACE_MEM_GET_TYPED(llace_function_t, module->funcs);
    size_t count = llace_mem_array_count(module->funcs);
    for (size_t i = 0; i < count; i++) {
      llace_function_destroy(&funcs[i]);
    }
    llace_mem_free(module->funcs);
  }
  
  // Free name table
  if (module->nametab != LLACE_HANDLE_INVALID) {
    llace_mem_free(module->nametab);
  }
  
  memset(module, 0, sizeof(llace_module_t));
  
  return LLACE_ERROR_NONE;
}

// ================ Module Builders ================ //

llace_error_t llace_module_add_name(llace_module_t *module, const char *name, llace_nameref_t *ref) {
  if (!module || !name || !ref) return LLACE_ERROR_BADARG;
  
  size_t len = strlen(name);
  if (len == 0) return LLACE_ERROR_BADARG;
  
  // Get current name table
  char *nametab = LLACE_MEM_GET_TYPED(char, module->nametab);
  size_t current_size = llace_mem_size(module->nametab);
  
  // Find current end of name table
  size_t offset = 0;
  while (offset < current_size && nametab[offset] != '\0') {
    offset += strlen(&nametab[offset]) + 1;
  }
  
  // Check if we need to expand
  if (offset + len + 1 >= current_size) {
    size_t new_size = current_size + len + 1024; // Add some extra space
    llace_error_t err = llace_mem_realloc(module->nametab, new_size);
    if (err != LLACE_ERROR_NONE) {
      return err;
    }
    nametab = LLACE_MEM_GET_TYPED(char, module->nametab);
  }
  
  // Copy name
  strcpy(&nametab[offset], name);
  *ref = offset;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_module_add_type(llace_module_t *module, const llace_type_t *type, llace_typeref_t *ref) {
  if (!module || !type || !ref) return LLACE_ERROR_BADARG;
  
  size_t current_count = llace_mem_array_count(module->types);
  
  // Expand array
  llace_error_t err = llace_mem_realloc_array(module->types, current_count + 1);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Add type
  llace_type_t *types = LLACE_MEM_GET_TYPED(llace_type_t, module->types);
  types[current_count] = *type;
  
  *ref = current_count;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_module_add_global(llace_module_t *module, const llace_global_t *global, llace_globalref_t *ref) {
  if (!module || !global || !ref) return LLACE_ERROR_BADARG;
  
  size_t current_count = llace_mem_array_count(module->globs);
  
  // Expand array
  llace_error_t err = llace_mem_realloc_array(module->globs, current_count + 1);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Add global
  llace_global_t *globs = LLACE_MEM_GET_TYPED(llace_global_t, module->globs);
  globs[current_count] = *global;
  
  *ref = current_count;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_module_add_function(llace_module_t *module, const llace_function_t *function, llace_funcref_t *ref) {
  if (!module || !function || !ref) return LLACE_ERROR_BADARG;
  
  size_t current_count = llace_mem_array_count(module->funcs);
  
  // Expand array
  llace_error_t err = llace_mem_realloc_array(module->funcs, current_count + 1);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Add function
  llace_function_t *funcs = LLACE_MEM_GET_TYPED(llace_function_t, module->funcs);
  funcs[current_count] = *function;
  
  *ref = current_count;
  
  return LLACE_ERROR_NONE;
}

// ================ Module Accessors ================ //

char *llace_module_get_name(llace_module_t *module, llace_nameref_t ref) {
  if (!module || module->nametab == LLACE_HANDLE_INVALID) return NULL;
  
  char *nametab = LLACE_MEM_GET_TYPED(char, module->nametab);
  size_t size = llace_mem_size(module->nametab);
  
  if (ref >= size) return NULL;
  
  return &nametab[ref];
}

llace_type_t *llace_module_get_type(llace_module_t *module, llace_typeref_t ref) {
  if (!module || module->types == LLACE_HANDLE_INVALID) return NULL;
  
  size_t count = llace_mem_array_count(module->types);
  if (ref >= count) return NULL;
  
  llace_type_t *types = LLACE_MEM_GET_TYPED(llace_type_t, module->types);
  return &types[ref];
}

llace_global_t *llace_module_get_global(llace_module_t *module, llace_globalref_t ref) {
  if (!module || module->globs == LLACE_HANDLE_INVALID) return NULL;
  
  size_t count = llace_mem_array_count(module->globs);
  if (ref >= count) return NULL;
  
  llace_global_t *globs = LLACE_MEM_GET_TYPED(llace_global_t, module->globs);
  return &globs[ref];
}

llace_function_t *llace_module_get_function(llace_module_t *module, llace_funcref_t ref) {
  if (!module || module->funcs == LLACE_HANDLE_INVALID) return NULL;
  
  size_t count = llace_mem_array_count(module->funcs);
  if (ref >= count) return NULL;
  
  llace_function_t *funcs = LLACE_MEM_GET_TYPED(llace_function_t, module->funcs);
  return &funcs[ref];
}

// ================ Module Utilities ================ //

size_t llace_module_type_count(const llace_module_t *module) {
  if (!module || module->types == LLACE_HANDLE_INVALID) return 0;
  return llace_mem_array_count(module->types);
}

size_t llace_module_global_count(const llace_module_t *module) {
  if (!module || module->globs == LLACE_HANDLE_INVALID) return 0;
  return llace_mem_array_count(module->globs);
}

size_t llace_module_function_count(const llace_module_t *module) {
  if (!module || module->funcs == LLACE_HANDLE_INVALID) return 0;
  return llace_mem_array_count(module->funcs);
}

llace_error_t llace_module_set_config(llace_module_t *module, llace_config_t *config) {
  if (!module) return LLACE_ERROR_BADARG;
  
  module->conf = config;
  return LLACE_ERROR_NONE;
}

llace_config_t *llace_module_get_config(llace_module_t *module) {
  if (!module) return NULL;
  return module->conf;
}