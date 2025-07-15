#ifndef LLACE_IR_CINSTR_H
#define LLACE_IR_CINSTR_H

#include <llace/ir/common.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================================ IR ================================================ //

typedef enum llace_copcode {
  // Arithmetic
  LLACE_COP_ADD,
  LLACE_COP_SUB,
  LLACE_COP_MUL,
  LLACE_COP_DIV,
  LLACE_COP_MOD,
  
  // Bitwise
  LLACE_COP_AND,
  LLACE_COP_OR,
  LLACE_COP_XOR,
  LLACE_COP_NOT,
  LLACE_COP_SHL,
  LLACE_COP_SHR,
  
  // Comparison
  LLACE_COP_EQ,
  LLACE_COP_NE,
  LLACE_COP_LT,
  LLACE_COP_LE,
  LLACE_COP_GT,
  LLACE_COP_GE,

  // Control flow
  LLACE_COP_CALL,
} llace_copcode_t;

typedef struct llace_cinstr {
  llace_copcode_t opcode;
  llace_handle_t params; // llace_value_t[] Operands for the instruction (zeroth index contains return value)
} llace_cinstr_t;

void llace_cinstr_create(llace_cinstr_t *instr);
void llace_cinstr_destroy(llace_cinstr_t *instr);

// ================================================ Builder ================================================ //

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_CINSTR_H