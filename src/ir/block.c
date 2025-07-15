#include <llace/ir/value.h>
#include <llace/ir/cinstr.h>
#include <llace/ir/instr.h>
#include <llace/ir/var.h>
#include <llace/detail/common.h>
#include <llace/mem.h>
#include <string.h>

llace_error_t llace_block_init(llace_block_t *block) {
  if (!block) return LLACE_ERROR_BADARG;

  block->locals = LLACE_NEW_ARRAY(sizeof(llace_variable_t), 8);
  block->instrs = LLACE_NEW_ARRAY(sizeof(llace_instr_t), 16);

  return LLACE_ERROR_NONE;
}

llace_error_t llace_block_destroy(llace_block_t *block) {
  if (!block) return LLACE_ERROR_BADARG;
  
  LLACE_ARRAY_FOREACH(llace_variable_t, var, block->locals) {
    llace_variable_destroy(var);
  }  
  LLACE_FREE_ARRAY(block->locals);

  LLACE_ARRAY_FOREACH(llace_instr_t, instr, block->instrs) {
    llace_instr_destroy(instr);
  }  
  LLACE_FREE_ARRAY(block->instrs);
  
  memset(block, 0, sizeof(llace_block_t));
  
  return LLACE_ERROR_NONE;
}