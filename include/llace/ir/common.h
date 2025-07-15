#ifndef LLACE_IR_COMMON_H
#define LLACE_IR_COMMON_H

#include <llace/llace.h>
#include <llace/mem.h>
#include <llace/config.h>

#ifdef __cplusplus
extern "C" {
#endif

// references
typedef size_t llace_nameref_t; // offset into name buffer
typedef size_t llace_typeref_t; // index into module types array
typedef size_t llace_globalref_t; // index into module globals array
typedef size_t llace_funcref_t; // index into module functions array
typedef size_t llace_varref_t; // index into function locals array

// value (forward declare)
struct llace_value;
typedef struct llace_value llace_value_t;

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_COMMON_H