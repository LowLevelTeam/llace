#ifndef LLACE_IR_GLOBAL_H
#define LLACE_IR_GLOBAL_H

#include <llace/llace.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================================ IR ================================================ //

typedef struct llace_global_attributes {
  unsigned _extern : 1; // The global is declared but not defined
  unsigned _public : 1; // The global is visible outside this module
  unsigned _const : 1; // The global will not be changed after initalization
  // unsigned _weak : 1; // The global can be overridden by another definition
} llace_global_attributes_t;

typedef struct llace_global {
  llace_nameref_t name;
  llace_typeref_t type;
  llace_value_t value; // optional llace_value_t

  // global attributes
  union {
    llace_global_attributes_t attr; // bit field
    uint16_t attrval; // raw value for attributes
  };
} llace_global_t;

/** 
* @param global uninitalized llace_global_t structure, if NULL fail with BADARG
*
* @return LLACE_ERROR_NONE
* @return LLACE_ERROR_BADARG
*/
llace_error_t llace_global_init(llace_global_t *global);

/** 
* @param global empty llace_global_t structure, if NULL fail with BADARG
*
* @return LLACE_ERROR_NONE
* @return LLACE_ERROR_BADARG
*/
llace_error_t llace_global_destroy(llace_global_t *global);

// ================================================ Builder ================================================ /

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_GLOBAL_H