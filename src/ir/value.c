#include <llace/ir/value.h>
#include <llace/ir/cinstr.h>
#include <llace/ir/instr.h>
#include <llace/detail/common.h>
#include <llace/mem.h>
#include <string.h>

llace_error_t llace_value_init(llace_value_t *val) {
  if (!val) return LLACE_ERROR_BADARG;
  
  memset(val, 0, sizeof(llace_value_t));
  val->kind = LLACE_VALUE_VOID;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_destroy(llace_value_t *val) {
  if (!val) return LLACE_ERROR_BADARG;
  
  switch (val->kind) {
    case LLACE_VALUE_VOID:
    case LLACE_VALUE_VARIABLE:
    case LLACE_VALUE_GLOBAL:
    case LLACE_VALUE_FUNCTION:
      // These don't own any resources
      break;
      
    case LLACE_VALUE_CONST:
      // Free constant data handle
      LLACE_FREE(val->_const.data);
      break;
      
    case LLACE_VALUE_INSTRUCTION:
      // Destroy child instruction
      llace_cinstr_destroy(&val->_instr);
      break;
      
    case LLACE_VALUE_BLOCK:
      // Free instruction block
      llace_block_destroy(&val->_block);
      break;
  }
  
  memset(val, 0, sizeof(llace_value_t));
  
  return LLACE_ERROR_NONE;
}
