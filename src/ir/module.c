#include <llace/ir/module.h>
#include <llace/detail/common.h>
#include <llace/mem.h>
#include <string.h>

// ================ Implementation ================ //

llace_error_t llace_module_init(llace_module_t *module, const char *name, size_t namelen) {
  if (!module) { LLACE_LOG_ERROR("Module is NULL"); return LLACE_ERROR_BADARG; }
  
  memset(module, 0, sizeof(llace_module_t));
  module->name = 0;

  // initialize with default capacity for faster initial allocations
  module->nametab = LLACE_NEW_ARRAY(char, 1024);
  module->types = LLACE_NEW_ARRAY(llace_type_t, 64);
  module->globs = LLACE_NEW_ARRAY(llace_global_t, 64);
  module->funcs = LLACE_NEW_ARRAY(llace_function_t, 64);
  
  char nullchar = '\0';
  LLACE_ARRAY_PUSH(module->nametab, nullchar); // 0th index always zero

  // Set module name if provided
  if (name) {
    llace_module_add_name(module, name, namelen, &module->name);
  }

  return LLACE_ERROR_NONE;
}

llace_error_t llace_module_destroy(llace_module_t *module) {
  if (!module) { LLACE_LOG_ERROR("Module is NULL"); return LLACE_ERROR_BADARG; }
  
  // Free all types
  LLACE_ARRAY_FOREACH(llace_type_t, type, module->types) {
    llace_type_destroy(type);
  }
  LLACE_FREE_ARRAY(module->types);
  
  // Free all globals
  LLACE_ARRAY_FOREACH(llace_global_t, glob, module->globs) {
    llace_global_destroy(glob);
  }
  LLACE_FREE_ARRAY(module->globs);
  
  // Free all functions
  LLACE_ARRAY_FOREACH(llace_function_t, func, module->funcs) {
    llace_function_destroy(func);
  }
  LLACE_FREE_ARRAY(module->funcs);
  
  // Free name table
  LLACE_FREE_ARRAY(module->nametab);

  memset(module, 0, sizeof(llace_module_t));
  
  return LLACE_ERROR_NONE;
}

// ================ Builders ================ //

llace_error_t llace_module_add_name(llace_module_t *module, const char *name, size_t namelen, llace_nameref_t *ref) {
  if (!module || !name || !ref) {
    LLACE_LOG_ERROR("Module is %p, Name is %p, Name Reference is %p", module, name, ref);
    return LLACE_ERROR_BADARG;
  }

  *ref = LLACE_ARRAY_COUNT(module->nametab); // reference is to the character after the last string

  size_t len = namelen > 0 ? namelen : strlen(name);
  
  // Push the string characters
  LLACE_ARRAY_PUSHA(module->nametab, name, len);
  // Push null terminator
  char nullchar = '\0';
  LLACE_ARRAY_PUSH(module->nametab, nullchar);

  return LLACE_ERROR_NONE;
}

llace_error_t llace_module_add_type(llace_module_t *module, const llace_type_t *type) {
  if (!module || !type) {
    LLACE_LOG_ERROR("Module is %p, Type is %p", module, type);
    return LLACE_ERROR_BADARG;
  }
  LLACE_ARRAY_PUSHP(module->types, type);
  return LLACE_ERROR_NONE;
}

llace_error_t llace_module_add_global(llace_module_t *module, const llace_global_t *global) {
  if (!module || !global) {
    LLACE_LOG_ERROR("Module is %p, Global is %p", module, global);
    return LLACE_ERROR_BADARG;
  }
  LLACE_ARRAY_PUSHP(module->globs, global);  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_module_add_function(llace_module_t *module, const llace_function_t *func) {
  if (!module || !func) {
    LLACE_LOG_ERROR("Module is %p, Function is %p", module, func);
    return LLACE_ERROR_BADARG;
  }
  LLACE_ARRAY_PUSHP(module->funcs, func);
  return LLACE_ERROR_NONE;
}

// ================ Accessors ================ //

char *llace_module_get_name(llace_module_t *module, llace_nameref_t ref) {
  if (!module) {
    LLACE_LOG_ERROR("Module is %p", module);
    return NULL;
  }
  
  // ADDED: Bounds checking
  if (ref >= LLACE_ARRAY_COUNT(module->nametab)) {
    LLACE_LOG_ERROR("Name reference %zu out of bounds (count: %zu)", ref, LLACE_ARRAY_COUNT(module->nametab));
    return NULL;
  }
  
  return LLACE_ARRAY_GET(char, module->nametab, ref);
}

llace_type_t *llace_module_get_type(llace_module_t *module, llace_typeref_t ref) {
  if (!module) {
    LLACE_LOG_ERROR("Module is %p", module);
    return NULL;
  }
  
  // ADDED: Bounds checking
  if (ref >= LLACE_ARRAY_COUNT(module->types)) {
    LLACE_LOG_ERROR("Type reference %zu out of bounds (count: %zu)", ref, LLACE_ARRAY_COUNT(module->types));
    return NULL;
  }
  
  return LLACE_ARRAY_GET(llace_type_t, module->types, ref);
}

llace_global_t *llace_module_get_global(llace_module_t *module, llace_globalref_t ref) {
  if (!module) {
    LLACE_LOG_ERROR("Module is %p", module);
    return NULL;
  }
  
  // ADDED: Bounds checking
  if (ref >= LLACE_ARRAY_COUNT(module->globs)) {
    LLACE_LOG_ERROR("Global reference %zu out of bounds (count: %zu)", ref, LLACE_ARRAY_COUNT(module->globs));
    return NULL;
  }
  
  return LLACE_ARRAY_GET(llace_global_t, module->globs, ref);
}

llace_function_t *llace_module_get_function(llace_module_t *module, llace_funcref_t ref) {
  if (!module) {
    LLACE_LOG_ERROR("Module is %p", module);
    return NULL;
  }
  
  // ADDED: Bounds checking
  if (ref >= LLACE_ARRAY_COUNT(module->funcs)) {
    LLACE_LOG_ERROR("Function reference %zu out of bounds (count: %zu)", ref, LLACE_ARRAY_COUNT(module->funcs));
    return NULL;
  }
  
  return LLACE_ARRAY_GET(llace_function_t, module->funcs, ref);
}