#ifndef LLACE_IR_FUNC_H
#define LLACE_IR_FUNC_H

#include <llace/ir/common.h>
#include <llace/ir/block.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================================ IR ================================================ //

typedef struct llace_function_attributes {
  unsigned _extern : 1; // The function is declared but not defined
  unsigned _public : 1; // The function is visible outside this module
  unsigned _const : 1; // The function has no side effects and does not modify or inspect any global state
  unsigned _pure : 1; // The function has no side effects but can inspect global state
  unsigned _noreturn : 1; // The function does not return to the caller
  unsigned _inline : 1; // The function can be inlined by the compiler
  unsigned _noinline : 1; // The function should not be inlined by the compiler
  unsigned _alwaysinline : 1; // The function should always be inlined by the compiler
  unsigned _nooptimize : 1; // The function should not be optimized by the compiler
  unsigned _weak : 1; // The function can be overridden by another definition
} llace_function_attributes_t;

typedef struct llace_function {
  llace_nameref_t name; // symbol name
  llace_array_t rets; // llace_variable_t[]
  llace_array_t params; // llace_variable_t[]
  llace_abi_t abi; // calling convention
  llace_block_t block; // instructions

  union {
    llace_function_attributes_t attr; // bit field
    uint16_t attrval; // raw value for attributes
  };
} llace_function_t;

llace_error_t llace_function_init(llace_function_t *func);
llace_error_t llace_function_destroy(llace_function_t *func);

// ================================================ Builder ================================================ /

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_FUNC_H