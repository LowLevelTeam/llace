#include <llace/ir/type.h>
#include <llace/ir/module.h>
#include <llace/detail/common.h>
#include <string.h>


llace_error_t llace_type_init(llace_type_t *type) {
  if (!type) return LLACE_ERROR_BADARG;
  
  memset(type, 0, sizeof(llace_type_t));
  type->name = 0;
  type->kind = LLACE_TYPE_VOID;
  type->size = 0;
  type->alignment = 1;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_type_destroy(llace_type_t *type) {
  if (!type) return LLACE_ERROR_BADARG;
  memset(type, 0, sizeof(llace_type_t));
  return LLACE_ERROR_NONE;
}

// ================ Type Builders ================ //

llace_error_t llace_type_void(llace_type_t *type) {
  if (!type) return LLACE_ERROR_BADARG;
  
  type->kind = LLACE_TYPE_VOID;
  type->size = 0;
  type->alignment = 1;
  
  return LLACE_ERROR_NONE;
}
llace_error_t llace_type_int(llace_type_t *type, unsigned long bits) {
  if (!type || bits == 0) return LLACE_ERROR_BADARG;
  
  type->kind = LLACE_TYPE_INT;
  type->_int = bits;
  uint64_t size = (bits + 7) / 8;
  type->size = size;

  if (size == 0) type->alignment = 1;
  else if (size == 1) type->alignment = 1;
  else {
    if ((size & (size - 1)) == 0) type->alignment = size; // If already a power of 2, use size as alignment
    else type->alignment = 1ULL << (64 - __builtin_clzll(size)); // Count leading zeros and calculate next power of 2
  }
  
  return LLACE_ERROR_NONE;
}
llace_error_t llace_type_uint(llace_type_t *type, unsigned long bits) {
  if (!type || bits == 0) return LLACE_ERROR_BADARG;
  
  type->kind = LLACE_TYPE_UNT;
  type->_unt = bits;
  uint64_t size = (bits + 7) / 8;
  type->size = size;

  if (size == 0) type->alignment = 1;
  else if (size == 1) type->alignment = 1;
  else {
    if ((size & (size - 1)) == 0) type->alignment = size; // If already a power of 2, use size as alignment
    else type->alignment = 1ULL << (64 - __builtin_clzll(size)); // Count leading zeros and calculate next power of 2
  }
  
  return LLACE_ERROR_NONE;
}
llace_error_t llace_type_float(llace_type_t *type, unsigned long mantissa, unsigned long exponent) {
  if (!type || mantissa == 0 || exponent == 0) return LLACE_ERROR_BADARG;
  
  type->kind = LLACE_TYPE_FLOAT;
  type->_float.mantissa = mantissa;
  type->_float.exponent = exponent;
  
  uint64_t size = (mantissa + exponent + 8) / 8; // (mantissa + exponenet + 1 + 7) / 8

  if (size == 0) type->alignment = 1;
  else if (size == 1) type->alignment = 1;
  else {
    if ((size & (size - 1)) == 0) type->alignment = size; // If already a power of 2, use size as alignment
    else type->alignment = 1ULL << (64 - __builtin_clzll(size)); // Count leading zeros and calculate next power of 2
  }
  
  return LLACE_ERROR_NONE;
}
llace_error_t llace_type_ptr(llace_type_t *type, llace_typeref_t pointee_type, size_t depth) {
  if (!type || depth == 0) return LLACE_ERROR_BADARG;
  
  type->kind = LLACE_TYPE_PTR;
  type->_ptr.type = pointee_type;
  type->_ptr.depth = depth;
  type->size = sizeof(void*);
  type->alignment = sizeof(void*);
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_type_add_void(llace_module_t *module, llace_typeref_t *typeref, const char *name) {
  if (!module) return LLACE_ERROR_BADARG;
  llace_error_t err;
  llace_type_t type;

  err = llace_type_init(&type);
  if (err != LLACE_ERROR_NONE) return err;

  err = llace_type_void(&type);
  if (err != LLACE_ERROR_NONE) return err;

  if (name) {
    llace_nameref_t name_ref;
    err = llace_module_add_name(module, name, &name_ref);
    if (err != LLACE_ERROR_NONE) return err;
    type.name = name_ref;
  }

  llace_typeref_t local_typeref;
  err = llace_module_add_type(module, &type, &local_typeref);
  if (err != LLACE_ERROR_NONE) return err;

  if (typeref) {
    *typeref = local_typeref;
  }
  
  return LLACE_ERROR_NONE;
}
llace_error_t llace_type_add_int(llace_module_t *module, llace_typeref_t *typeref, const char *name, unsigned bitwidth) {
  if (!module) return LLACE_ERROR_BADARG;
  llace_error_t err;
  llace_type_t type;

  err = llace_type_init(&type);
  if (err != LLACE_ERROR_NONE) return err;

  err = llace_type_int(&type, bitwidth);
  if (err != LLACE_ERROR_NONE) return err;

  if (name) {
    llace_nameref_t name_ref;
    err = llace_module_add_name(module, name, &name_ref);
    if (err != LLACE_ERROR_NONE) return err;
    type.name = name_ref;
  }

  llace_typeref_t local_typeref;
  err = llace_module_add_type(module, &type, &local_typeref);
  if (err != LLACE_ERROR_NONE) return err;

  if (typeref) {
    *typeref = local_typeref;
  }
  
  return LLACE_ERROR_NONE;
}
llace_error_t llace_type_add_uint(llace_module_t *module, llace_typeref_t *typeref, const char *name, unsigned bitwidth) {
  if (!module) return LLACE_ERROR_BADARG;
  llace_error_t err;
  llace_type_t type;

  err = llace_type_init(&type);
  if (err != LLACE_ERROR_NONE) return err;

  err = llace_type_uint(&type, bitwidth);
  if (err != LLACE_ERROR_NONE) return err;

  if (name) {
    llace_nameref_t name_ref;
    err = llace_module_add_name(module, name, &name_ref);
    if (err != LLACE_ERROR_NONE) return err;
    type.name = name_ref;
  }

  llace_typeref_t local_typeref;
  err = llace_module_add_type(module, &type, &local_typeref);
  if (err != LLACE_ERROR_NONE) return err;

  if (typeref) {
    *typeref = local_typeref;
  }
  
  return LLACE_ERROR_NONE;
}
llace_error_t llace_type_add_float(llace_module_t *module, llace_typeref_t *typeref, const char *name, unsigned mantissa, unsigned exponent) {
  if (!module) return LLACE_ERROR_BADARG;
  llace_error_t err;
  llace_type_t type;

  err = llace_type_init(&type);
  if (err != LLACE_ERROR_NONE) return err;

  err = llace_type_float(&type, mantissa, exponent);
  if (err != LLACE_ERROR_NONE) return err;

  if (name) {
    llace_nameref_t name_ref;
    err = llace_module_add_name(module, name, &name_ref);
    if (err != LLACE_ERROR_NONE) return err;
    type.name = name_ref;
  }

  llace_typeref_t local_typeref;
  err = llace_module_add_type(module, &type, &local_typeref);
  if (err != LLACE_ERROR_NONE) return err;

  if (typeref) {
    *typeref = local_typeref;
  }
  
  return LLACE_ERROR_NONE;
}
llace_error_t llace_type_add_ptr(llace_module_t *module, llace_typeref_t *typeref, const char *name, llace_typeref_t pointee_type, size_t depth) {
  if (!module) return LLACE_ERROR_BADARG;
  llace_error_t err;
  llace_type_t type;

  err = llace_type_init(&type);
  if (err != LLACE_ERROR_NONE) return err;

  err = llace_type_ptr(&type, pointee_type, depth);
  if (err != LLACE_ERROR_NONE) return err;

  if (name) {
    llace_nameref_t name_ref;
    err = llace_module_add_name(module, name, &name_ref);
    if (err != LLACE_ERROR_NONE) return err;
    type.name = name_ref;
  }

  llace_typeref_t local_typeref;
  err = llace_module_add_type(module, &type, &local_typeref);
  if (err != LLACE_ERROR_NONE) return err;

  if (typeref) {
    *typeref = local_typeref;
  }
  
  return LLACE_ERROR_NONE;
}
