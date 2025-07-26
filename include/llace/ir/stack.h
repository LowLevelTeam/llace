#ifndef LLACE_IR_STACK_H
#define LLACE_IR_STACK_H

#ifndef __cplusplus
extern "C" {
#endif

typedef struct llace_ir_typeattr {
  union {
    struct {
      unsigned int _const : 1; // dont optimize
      unsigned int _volatile : 1; // dont optimize
    } attr;
    uint16_t attraw;
  };
  size_t depth; // redirection depth (i.e. for pointers)
} llace_ir_typeattr_t;

typedef struct llace_ir_type {
  // Debug Name

  // Type Information
  union {
    size_t _int; // integer bit width
    size_t _unt; // unsigned bit width
    struct { size_t mantissa; size_t exponent; } _float; // mantissa bit width, exponent bit width
  };
} llace_ir_type_t;

typedef struct llace_ir_value {
  // Type

  // Value
  union {
    // Constant
    // Variable
    // Global
    // Function
    // Instruction
  };

  // 10 10 +
  // Constant Constant Instruction
} llace_ir_value_t;

typedef struct llace_ir_variable {
  // Debug Name

  // Type

  // Type Attributes
} llace_ir_variable_t;

typedef struct llace_ir_global {
  // Debug Name

  // Value

  // Type

  // Type Attributes
} llace_ir_global_t;

typedef struct llace_ir_basicblock {
  llace_array_t stack; // llace_ir_value_t

  // information for optimize
} llace_ir_basicblock_t;

typedef struct llace_ir_function {
  // Debug Name
  
  // Signature
  // abi calling convention

  // Basic Blocks

} llace_ir_function_t;

typedef struct llace_ir_context {
  // Globals
  llace_globmap_t globmap;

  // Functions
  llace_funcmap_t funcmap;
} llace_ir_context_t;


#ifndef __cplusplus
}
#endif

#endif // LLACE_IR_STACK_H