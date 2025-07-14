#ifndef LLACE_IR_FUNC_H
#define LLACE_IR_FUNC_H

#include <llace/llace.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================================ IR ================================================ //

typedef struct llace_function_attributes {
  unsigned _extern : 1; // The function is declared but not defined
  unsigned _public : 1; // The function is visible outside this module
  // unsigned _const : 1; // The function has no side effects and does not modify or inspect any global state
  // unsigned _pure : 1; // The function has no side effects but can inspect global state
  // unsigned _noreturn : 1; // The function does not return to the caller
  // unsigned _vararg : 1; // The function can take a variable number of arguments
  // unsigned _inline : 1; // The function can be inlined by the compiler
  // unsigned _noinline : 1; // The function should not be inlined by the compiler
  // unsigned _alwaysinline : 1; // The function should always be inlined by the compiler
  // unsigned _optimize : 1; // The function should be optimized by the compiler
  // unsigned _nooptimize : 1; // The function should not be optimized by the compiler
  // unsigned _linkonce : 1; // The function can be merged with other definitions
  // unsigned _weak : 1; // The function can be overridden by another definition
} llace_function_attributes_t;

typedef struct llace_function {
  llace_nameref_t name; // symbol name
  llace_abi_t abi; // calling convention
  llace_handler_t rets; // llace_variable_t[]
  llace_handler_t params; // llace_variable_t[]
  llace_handle_t locals; // llace_variable_t[]
  llace_handle_t instrs; // llace_instr_t[]

  union {
    llace_function_attributes_t attr; // bit field
    uint16_t attrval; // raw value for attributes
  };
} llace_function_t;

/** 
* @param func uninitalized llace_function_t structure, if NULL fail with BADARG
*
* @return LLACE_ERROR_NONE
* @return LLACE_ERROR_BADARG
*/
llace_error_t llace_function_init(llace_function_t *func);

/** 
* @param func empty llace_function_t structure, if NULL fail with BADARG
*
* @return LLACE_ERROR_NONE
* @return LLACE_ERROR_BADARG
*/
llace_error_t llace_function_destroy(llace_function_t *func);

// ================================================ Builder ================================================ /

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_FUNC_H