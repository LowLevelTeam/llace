#include <llace/ir/global.h>
#include <llace/detail/common.h>
#include <string.h>

// ================ Global Implementation ================ //

llace_error_t llace_global_init(llace_global_t *global) {
  if (!global) { 
    LLACE_LOG_ERROR("Global is %p", global);
    return LLACE_ERROR_BADARG;
  }

  memset(global, 0, sizeof(llace_global_t));
  global->name = 0;
  global->type = 0;
  global->attrval = 0;
  
  // Initialize value as void
  llace_value_init(&global->value);
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_global_destroy(llace_global_t *global) {
  if (!global) { 
    LLACE_LOG_ERROR("Global is %p", global);
    return LLACE_ERROR_BADARG;
  }
  
  // Destroy the value
  llace_value_destroy(&global->value);
  
  memset(global, 0, sizeof(llace_global_t));
  
  return LLACE_ERROR_NONE;
}
