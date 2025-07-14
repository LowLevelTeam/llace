#ifndef LLACE_IR_ABI_H
#define LLACE_IR_ABI_H

#include <llace/llace.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum llace_abi {
  // If used then no arguments are passed, nothing is returned and function can not return. 
  // Essentially used for jumps.
  LLACE_ABI_NULL, 
  LLACE_ABI_CDECL,
  // introducing more for operating system specific conventions
  // for system calls, specific calling conventions, etc.
} llace_abi_t;

const char *llace_abi_str(llace_abi_t abi);

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_ABI_H