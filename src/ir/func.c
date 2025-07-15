#include <llace/ir/func.h>
#include <llace/ir/var.h>
#include <llace/ir/instr.h>
#include <string.h>

// ================ Function Implementation ================ //

llace_error_t llace_function_init(llace_function_t *func) {
  if (!func) return LLACE_ERROR_BADARG;
  
  memset(func, 0, sizeof(llace_function_t));
  func->name = 0;
  func->abi = LLACE_ABI_CDECL;
  func->attrval = 0;
  func->rets   = LLACE_NEW_ARRAY(sizeof(llace_variable_t), 8);
  func->params = LLACE_NEW_ARRAY(sizeof(llace_variable_t), 16);
  LLACE_RUNCHECK(llace_block_init(&func->block));

  return LLACE_ERROR_NONE;
}

llace_error_t llace_function_destroy(llace_function_t *func) {
  if (!func) return LLACE_ERROR_BADARG;
  

  LLACE_ARRAY_FOREACH(llace_variable_t, var, func->rets) {
    llace_variable_destroy(var);
  }  
  LLACE_FREE_ARRAY(func->rets);

  LLACE_ARRAY_FOREACH(llace_variable_t, var, func->params) {
    llace_variable_destroy(var);
  }  
  LLACE_FREE_ARRAY(func->params);

  llace_block_destroy(&func->block);
  
  memset(func, 0, sizeof(llace_function_t));
  
  return LLACE_ERROR_NONE;
}
