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
  LLACE_TYPE_ARRAY,
  LLACE_TYPE_STRUCT,
  LLACE_TYPE_UNION,
  LLACE_TYPE_FUNCTION,
  LLACE_TYPE_VARADIC
} llace_type_kind_t;

// Type structure
typedef struct llace_type {
  llace_nameref_t name; // Type name (for debugging)
  llace_type_kind_t kind;
  size_t size; // size in bytes (rounded up to nearest byte)
  size_t alignment; // allignment in bytes (rounded up to nearest power of two)

  union {
    size_t _int; // LLACE_TYPE_INT // value is bit width
    size_t _unt; // LLACE_TYPE_UNT // value is bit width
    struct {
      // values are bit widths
      size_t mantissa; // also referred to as significand
      size_t exponent;
    } _float; // LLACE_TYPE_FLOAT
    struct { 
      llace_typeref_t type;
      size_t depth; // depth of pointer indirection
    } _ptr; // LLACE_TYPE_PTR // pointee type (types are defined in the context)
    struct {
     llace_typeref_t element;
     size_t count;
    } _array; // LLACE_TYPE_ARRAY
    struct {
      llace_array_t members; // llace_typeref_t[]
    } _composite; // LLACE_TYPE_STRUCT | LLACE_TYPE_UNION
    struct {
     llace_array_t params; // llace_typeref_t[]
     llace_array_t returns; // llace_typeref_t[]
     llace_abi_t abi; // calling convention
    } _func; // LLACE_TYPE_FUNCTION

    // void _void; // LLACE_TYPE_VOID
    // void _vptr; // LLACE_TYPE_VPTR // opaque pointer type
    // void _varadic; // LLACE_TYPE_VARADIC
  };
} llace_type_t;

llace_error_t llace_type_init(llace_type_t *type);
llace_error_t llace_type_destroy(llace_type_t *type);

// ================================================ Builder ================================================ //

// Utilizes struct based initalization by far the best option for C.
// utilizing unions all the variations exist in one easy to manipulate structure.
// then the structure can be passed into the llace_build_type function.

typedef struct llace_type_builder {
  llace_module_t *_module;
  const char *name; 
  size_t namelen;
  llace_type_kind_t kind;

  union {
    size_t _int; // LLACE_TYPE_INT
    size_t _unt; // LLACE_TYPE_UNT
    struct { size_t mantissa; size_t exponent; } _float; // LLACE_TYPE_FLOAT
    struct { llace_typeref_t type; size_t depth; } _ptr; // LLACE_TYPE_PTR
    struct { llace_typeref_t element; size_t count; } _array; // LLACE_TYPE_ARRAY
    struct { llace_array_t members; size_t count; } _composite; // LLACE_TYPE_STRUCT | LLACE_TYPE_UNION
    struct { llace_array_t params; llace_array_t returns; llace_abi_t abi; } _func; // LLACE_TYPE_FUNCTION
  };
} llace_type_builder_t;

llace_error_t llace_build_type(llace_type_builder_t *builder, llace_type_t *dest);

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_TYPE_H