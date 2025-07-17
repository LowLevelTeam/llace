#include <llace/ir/cinstr.h>
#include <llace/detail/common.h>
#include <llace/mem.h>
#include <string.h>

#include <llace/ir/value.h>

// ================ Child Instruction Implementation ================ //

llace_error_t llace_cinstr_create(llace_cinstr_t *instr) {
  if (!instr) return LLACE_ERROR_BADARG;
  
  memset(instr, 0, sizeof(llace_cinstr_t));
  instr->opcode = LLACE_COP_NULL; // Default to safe opcode
  instr->params = LLACE_NEW_ARRAY(llace_value_t, 2); // standard parameter size for operands

  return LLACE_ERROR_NONE;
}

void llace_cinstr_destroy(llace_cinstr_t *instr) {
  if (!instr) return;
  
  LLACE_ARRAY_FOREACH(llace_value_t, val, instr->params) {
    llace_value_destroy(val);
  }  
  LLACE_FREE_ARRAY(instr->params);

  memset(instr, 0, sizeof(llace_cinstr_t));
}

const char *llace_cinstr_opstr(llace_copcode_t op) {
  switch (op) {
    case LLACE_COP_NULL: return "null";
    case LLACE_COP_ADD: return "add";
    case LLACE_COP_SUB: return "sub";
    case LLACE_COP_MUL: return "mul";
    case LLACE_COP_DIV: return "div";
    case LLACE_COP_MOD: return "mod";
    case LLACE_COP_AND: return "and";
    case LLACE_COP_OR: return "or";
    case LLACE_COP_XOR: return "xor";
    case LLACE_COP_NOT: return "not";
    case LLACE_COP_SHL: return "shl";
    case LLACE_COP_SHR: return "shr";
    case LLACE_COP_EQ: return "eq";
    case LLACE_COP_NE: return "ne";
    case LLACE_COP_LT: return "lt";
    case LLACE_COP_LE: return "le";
    case LLACE_COP_GT: return "gt";
    case LLACE_COP_GE: return "ge";
    case LLACE_COP_CALL: return "call";
    default: return "unknown";
  }
}