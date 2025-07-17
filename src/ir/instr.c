#include <llace/ir/instr.h>
#include <llace/ir/value.h>
#include <string.h>

// ================ Instruction Implementation ================ //

llace_error_t llace_instr_create(llace_instr_t *instr) {
  if (!instr) return LLACE_ERROR_BADARG;
  
  memset(instr, 0, sizeof(llace_instr_t));
  instr->opcode = LLACE_OP_NULL; // Default to safe opcode
  instr->params = LLACE_NEW_ARRAY(llace_value_t, 2);

  return LLACE_ERROR_NONE;
}

void llace_instr_destroy(llace_instr_t *instr) {
  if (!instr) return;
  
  LLACE_ARRAY_FOREACH(llace_value_t, val, instr->params) {
    llace_value_destroy(val);
  }  
  LLACE_FREE_ARRAY(instr->params);
  
  memset(instr, 0, sizeof(llace_instr_t));
}

const char *llace_instr_opstr(llace_opcode_t op) {
  switch (op) {
    case LLACE_OP_NULL: return "null";
    case LLACE_OP_ADD: return "add";
    case LLACE_OP_SUB: return "sub";
    case LLACE_OP_MUL: return "mul";
    case LLACE_OP_DIV: return "div";
    case LLACE_OP_MOD: return "mod";
    case LLACE_OP_AND: return "and";
    case LLACE_OP_OR: return "or";
    case LLACE_OP_XOR: return "xor";
    case LLACE_OP_SHL: return "shl";
    case LLACE_OP_SHR: return "shr";
    case LLACE_OP_LOAD: return "load";
    case LLACE_OP_STORE: return "store";
    case LLACE_OP_ALLOC: return "alloc";
    case LLACE_OP_DEALLOC: return "dealloc";
    case LLACE_OP_JMP: return "jmp";
    case LLACE_OP_BR: return "br";
    case LLACE_OP_CALL: return "call";
    case LLACE_OP_RET: return "ret";
    default: return "unknown";
  }
}