#ifndef LLACE_IR_VALUE_H
#define LLACE_IR_VALUE_H

#include <llace/llace.h>
#include <llace/ir/cinstr.h>
#include <llace/ir/func.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================================ IR ================================================ //

typedef enum llace_value_kind {
  LLACE_VALUE_VOID, // No value (needed for optional values)
  LLACE_VALUE_CONST, // Constant value
  LLACE_VALUE_VARIABLE, // Local Variable Reference
  LLACE_VALUE_GLOBAL, // Global Variable Reference
  LLACE_VALUE_FUNCTION, // Function Reference
  LLACE_VALUE_INSTRUCTION, // Instruction
  LLACE_VALUE_BLOCK, // Block of Instructions
} llace_value_kind_t;

typedef struct llace_value {
  llace_value_kind_t kind; // Value kind

  union {
    // void _void;
    struct {
      llace_typeref_t type; // Type of the constant value
      llace_item_t data; // Pointer to the constant data (e.g., integer, float)
    } _const; // Constant  (LLACE_VALUE_CONST)
    llace_varref_t _var; // Variable Reference (LLACE_VALUE_VARIABLE)
    llace_globalref_t _glob; // Global Reference (LLACE_VALUE_GLOBAL)
    llace_funcref_t _func; // Function Reference (LLACE_VALUE_FUNCTION)
    llace_cinstr_t _instr; // Child Instruction (LLACE_VALUE_INSTRUCTION)
    llace_block_t _block; // Instruction Block (LLACE_VALUE_BLOCK)
  };
} llace_value_t;

llace_error_t llace_value_init(llace_value_t *val);
llace_error_t llace_value_destroy(llace_value_t *val);

// ================================================ Builder ================================================ //

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_VALUE_H