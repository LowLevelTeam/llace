#ifndef LLACE_IR_VAR_H
#define LLACE_IR_VAR_H

#include <llace/ir/common.h>
#include <llace/ir/value.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================================ IR ================================================ //

typedef struct llace_variable_attributes {
  unsigned _const : 1; // The variable is not modified after initalization
  unsigned _volatile : 1; // The variable is not optimized
  unsigned _cexpr : 1; // The variable is utilized at compile time
} llace_variable_attributes_t;

typedef struct llace_variable {
  llace_nameref_t name;
  llace_typeref_t type;
  llace_value_t value; // optional llace_value_t

  // variable attributes
  union {
    llace_variable_attributes_t attr; // bit field
    uint16_t attrval; // raw value for attributes
  };
} llace_variable_t;


llace_error_t llace_variable_init(llace_variable_t *variable);
llace_error_t llace_variable_destroy(llace_variable_t *variable);

// ================================================ Builder ================================================ /

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_VAR_H