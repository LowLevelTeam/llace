#ifndef LLACE_IR_CINSTR_H
#define LLACE_IR_CINSTR_H

#include <llace/llace.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================================ IR ================================================ //

typedef enum llace_copcode {
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
  LLACE_OP_NOT,
  LLACE_OP_SHL,
  LLACE_OP_SHR,
  
  // Comparison
  LLACE_OP_EQ,
  LLACE_OP_NE,
  LLACE_OP_LT,
  LLACE_OP_LE,
  LLACE_OP_GT,
  LLACE_OP_GE,

  // Control flow
  LLACE_OP_CALL,
} llace_copcode_t;

typedef struct llace_cinstr {
  llace_copcode_t opcode;
  llace_value_t ret;
  llace_handle_t params; // llace_value_t[] Operands for the instruction
} llace_cinstr_t;

void llace_cinstr_create(llace_cinstr_t *instr);
void llace_cinstr_destroy(llace_cinstr_t *instr);

// ================================================ Builder ================================================ //

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_CINSTR_H