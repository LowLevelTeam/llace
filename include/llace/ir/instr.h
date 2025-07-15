#ifndef LLACE_IR_INSTR_H
#define LLACE_IR_INSTR_H

#include <llace/ir/common.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================================ IR ================================================ //

typedef enum llace_opcode {
  LLACE_OP_NULL,

  // Arithmetic
  LLACE_OP_ADD,
  LLACE_OP_SUB,
  LLACE_OP_MUL,
  LLACE_OP_DIV,
  LLACE_OP_MOD,
  
  // Bitwise
  LLACE_OP_AND,
  LLACE_OP_OR,
  LLACE_OP_XOR,
  LLACE_OP_SHL,
  LLACE_OP_SHR,
  
  // Memory
  LLACE_OP_LOAD,
  LLACE_OP_STORE,
  LLACE_OP_ALLOC, // marks a variable is used beyond this point
  LLACE_OP_DEALLOC, // marks a variable is not used beyond this point

  // Control flow
  LLACE_OP_JMP, // unconditional jump with no return
  LLACE_OP_BR, // conditional jump with no return (utilize child instructions for conditions following immediate branching structure)
  LLACE_OP_CALL, // unconditonal jump with return
  LLACE_OP_RET, // unconditional jump to return
} llace_opcode_t;

typedef struct llace_instr {
  llace_opcode_t opcode;
  llace_array_t params; // llace_value_t[] Operands for the instruction
} llace_instr_t;

llace_error_t llace_instr_create(llace_instr_t *instr);
void llace_instr_destroy(llace_instr_t *instr);
const char *llace_instr_opstr(llace_opcode_t op);

// ================================================ Builder ================================================ //

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_INSTR_H