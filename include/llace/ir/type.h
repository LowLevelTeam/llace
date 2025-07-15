#ifndef LLACE_IR_TYPE_H
#define LLACE_IR_TYPE_H

#include <llace/ir/common.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================================ IR ================================================ //

// Type kinds
typedef enum {
  LLACE_TYPE_VOID,
  LLACE_TYPE_INT,
  LLACE_TYPE_UNT,
  LLACE_TYPE_FLOAT,
  LLACE_TYPE_PTR,
  LLACE_TYPE_VPTR, // Opaque/void pointer
  // LLACE_TYPE_ARRAY,
  // LLACE_TYPE_STRUCT,
  // LLACE_TYPE_FUNCTION,
  // LLACE_TYPE_VARADIC,
} llace_type_kind_t;

// Type structure
typedef struct llace_type {
  llace_nameref_t name; // Type name (for debugging)
  llace_type_kind_t kind;
  size_t size; // size in bytes (rounded up to nearest byte)
  size_t alignment; // allignment in bytes (rounded up to nearest power of two)

  union {
    // void _void;
    unsigned long _int; // value is bit width
    unsigned long _unt; // value is bit width
    struct {
      // values are bit widths
      unsigned long mantissa; // also referred to as significand
      unsigned long exponent;
    } _float;
    struct { 
      llace_typeref_t type;
      size_t depth; // depth of pointer indirection
    } _ptr; // pointee type (types are defined in the context)
    // void _vptr; // opaque pointer type

    // Implementations Undecided but an example is given below
    // TODO(FAR): Implement Multi-Value and Composite Types
    // struct {
    //  llace_type_t **element_type;
    //  unsigned long count; // number of elements
    // } _array;
    // struct {
    //  llace_type_t **element_type;
    //  unsigned long count; // number of elements
    // } _struct;
    // struct {
    //  unsigned long paramcount; // number of parameters
    //  unsigned long returncount; // number of returns
    //  llace_type_t **params;
    //  llace_type_t **returns;
    //  llace_abi_t abi; // calling convention
    // } _function;
  };
} llace_type_t;

/** 
* @param type uninitalized llace_type_t structure, if NULL fail with BADARG
*
* @return LLACE_ERROR_NONE
* @return LLACE_ERROR_BADARG
*/
llace_error_t llace_type_init(llace_type_t *type);

/** 
* @param type empty llace_type_t structure, if NULL fail with BADARG
*
* @return LLACE_ERROR_NONE
* @return LLACE_ERROR_BADARG
*/
llace_error_t llace_type_destroy(llace_type_t *type);

// ================================================ Builder ================================================ //

llace_error_t llace_type_void(llace_type_t *type);
llace_error_t llace_type_int(llace_type_t *type, unsigned long bits);
llace_error_t llace_type_uint(llace_type_t *type, unsigned long bits);
llace_error_t llace_type_float(llace_type_t *type, unsigned long mantissa, unsigned long exponent);
llace_error_t llace_type_ptr(llace_type_t *type, llace_typeref_t pointee_type, size_t depth);

// name and typeref can be NULL
llace_error_t llace_type_add_int(llace_module_t *module, llace_typeref_t *typeref, const char *name, unsigned bitwidth);
llace_error_t llace_type_add_uint(llace_module_t *module, llace_typeref_t *typeref, const char *name, unsigned bitwidth);
llace_error_t llace_type_add_float(llace_module_t *module, llace_typeref_t *typeref, const char *name, unsigned mantissa, unsigned exponent);
llace_error_t llace_type_add_void(llace_module_t *module, llace_typeref_t *typeref, const char *name);
llace_error_t llace_type_add_ptr(llace_module_t *module, llace_typeref_t *typeref, const char *name, llace_typeref_t pointee_type, size_t depth);

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_TYPE_H