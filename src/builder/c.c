#include <llace/builder/c64.h>
#include <llace/ir.h>
#include <llace/detail/common.h>
#include <string.h>

// ================ Type Names ================ //
// C@bool
// C@char
// C@unsigned char
// C@short
// C@unsigned short
// C@int
// C@unsigned int
// C@long
// C@unsigned long
// C@long long
// C@unsigned long long
// C@float
// C@double
// C@long double
// C@void
// C@int8_t
// C@uint8_t
// C@int16_t
// C@uint16_t
// C@int32_t
// C@uint32_t
// C@int64_t
// C@uint64_t

// ================ Type Definitions ================ //

static llace_error_t add_int_type(llace_module_t *module, const char *name, unsigned bitwidth) {
  llace_error_t err;
  llace_type_t type;
  llace_nameref_t name_ref;

  err = llace_type_init(&type);
  if (err != LLACE_ERROR_NONE) return err;

  err = llace_type_int(&type, bitwidth);
  if (err != LLACE_ERROR_NONE) return err;
  
  err = llace_module_add_name(module, name, &name_ref);
  if (err != LLACE_ERROR_NONE) return err;
  type.name = name_ref;
  
  llace_typeref_t type_ref;
  err = llace_module_add_type(module, &type, &type_ref);
  if (err != LLACE_ERROR_NONE) return err;
  
  return LLACE_ERROR_NONE;
}
static llace_error_t add_uint_type(llace_module_t *module, const char *name, unsigned bitwidth) {
  llace_error_t err;
  llace_type_t type;
  llace_nameref_t name_ref;

  err = llace_type_init(&type);
  if (err != LLACE_ERROR_NONE) return err;

  err = llace_type_uint(&type, bitwidth);
  if (err != LLACE_ERROR_NONE) return err;
  
  err = llace_module_add_name(module, name, &name_ref);
  if (err != LLACE_ERROR_NONE) return err;
  type.name = name_ref;
  
  llace_typeref_t type_ref;
  err = llace_module_add_type(module, &type, &type_ref);
  if (err != LLACE_ERROR_NONE) return err;
  
  return LLACE_ERROR_NONE;
}
static llace_error_t add_float_type(llace_module_t *module, const char *name, unsigned mant, unsigned exp) {
  llace_error_t err;
  llace_type_t type;
  llace_nameref_t name_ref;

  err = llace_type_init(&type);
  if (err != LLACE_ERROR_NONE) return err;

  err = llace_type_float(&type, mant, exp);
  if (err != LLACE_ERROR_NONE) return err;
  
  err = llace_module_add_name(module, name, &name_ref);
  if (err != LLACE_ERROR_NONE) return err;
  type.name = name_ref;

  llace_typeref_t type_ref;
  err = llace_module_add_type(module, &type, &type_ref);
  if (err != LLACE_ERROR_NONE) return err;
  
  return LLACE_ERROR_NONE;
}
static llace_error_t add_void_type(llace_module_t *module, const char *name) {
  llace_error_t err;
  llace_type_t type;
  llace_nameref_t name_ref;

  err = llace_type_init(&type);
  if (err != LLACE_ERROR_NONE) return err;

  err = llace_type_void(&type);
  if (err != LLACE_ERROR_NONE) return err;

  err = llace_module_add_name(module, name, &name_ref);
  if (err != LLACE_ERROR_NONE) return err;
  type.name = name_ref;

  llace_typeref_t type_ref;
  err = llace_module_add_type(module, &type, &type_ref);
  if (err != LLACE_ERROR_NONE) return err;
  
  return LLACE_ERROR_NONE;
}

// ================ Public Functions ================ //

llace_error_t llace_builderc_types(llace_module_t *module) {
  if (!module) return LLACE_ERROR_BADARG;

  llace_error_t err;

  if (llace_target_word_size(&module->conf->target) == 32) {
    err = add_uint_type(module, "C@bool", 1);
    if (err != LLACE_ERROR_NONE) return err;
    
    // char (8-bit signed)
    err = add_int_type(module, "C@char", 8);
    if (err != LLACE_ERROR_NONE) return err;
    
    // unsigned char (8-bit unsigned)
    err = add_uint_type(module, "C@unsigned char", 8);
    if (err != LLACE_ERROR_NONE) return err;
    
    // short (16-bit signed)
    err = add_int_type(module, "C@short", 8);
    if (err != LLACE_ERROR_NONE) return err;
    
    // unsigned short (16-bit unsigned)
    err = add_uint_type(module, "C@unsigned short", 8);
    if (err != LLACE_ERROR_NONE) return err;
    
    // int (32-bit signed)
    err = add_int_type(module, "C@int", 16);
    if (err != LLACE_ERROR_NONE) return err;
    
    // unsigned int (32-bit unsigned)
    err = add_uint_type(module, "C@unsigned int", 16);
    if (err != LLACE_ERROR_NONE) return err;
    
    // long (64-bit signed on 64-bit systems)
    err = add_int_type(module, "C@long", 32);
    if (err != LLACE_ERROR_NONE) return err;
    
    // unsigned long (64-bit unsigned on 64-bit systems)
    err = add_uint_type(module, "C@unsigned long", 32);
    if (err != LLACE_ERROR_NONE) return err;
    
    // long long (64-bit signed)
    err = add_int_type(module, "C@long long", 32);
    if (err != LLACE_ERROR_NONE) return err;
    
    // unsigned long long (64-bit unsigned)    
    err = add_uint_type(module, "C@unsigned long long", 32);
    if (err != LLACE_ERROR_NONE) return err;
  } else if (llace_target_word_size(&module->conf->target) == 64) {
    err = add_uint_type(module, "C@bool", 1);
    if (err != LLACE_ERROR_NONE) return err;
    
    // char (8-bit signed)
    err = add_int_type(module, "C@char", 8);
    if (err != LLACE_ERROR_NONE) return err;
    
    // unsigned char (8-bit unsigned)
    err = add_uint_type(module, "C@unsigned char", 8);
    if (err != LLACE_ERROR_NONE) return err;
    
    // short (16-bit signed)
    err = add_int_type(module, "C@short", 16);
    if (err != LLACE_ERROR_NONE) return err;
    
    // unsigned short (16-bit unsigned)
    err = add_uint_type(module, "C@unsigned short", 16);
    if (err != LLACE_ERROR_NONE) return err;
    
    // int (32-bit signed)
    err = add_int_type(module, "C@int", 32);
    if (err != LLACE_ERROR_NONE) return err;
    
    // unsigned int (32-bit unsigned)
    err = add_uint_type(module, "C@unsigned int", 32);
    if (err != LLACE_ERROR_NONE) return err;
    
    // long (64-bit signed on 64-bit systems)
    err = add_int_type(module, "C@long", 64);
    if (err != LLACE_ERROR_NONE) return err;
    
    // unsigned long (64-bit unsigned on 64-bit systems)
    err = add_uint_type(module, "C@unsigned long", 64);
    if (err != LLACE_ERROR_NONE) return err;
    
    // long long (64-bit signed)
    err = add_int_type(module, "C@long long", 64);
    if (err != LLACE_ERROR_NONE) return err;
    
    // unsigned long long (64-bit unsigned)    
    err = add_uint_type(module, "C@unsigned long long", 64);
    if (err != LLACE_ERROR_NONE) return err;
  } else {
    return LLACE_ERROR_BADARG;
  }
  
  // Fixed-width integer types
  // int8_t
  err = add_int_type(module, "C@int8_t", 8);
  if (err != LLACE_ERROR_NONE) return err;
  
  // uint8_t
  err = add_uint_type(module, "C@uint8_t", 8);
  if (err != LLACE_ERROR_NONE) return err;
  
  // int16_t
  err = add_int_type(module, "C@int16_t", 16);
  if (err != LLACE_ERROR_NONE) return err;
  
  // uint16_t
  err = add_uint_type(module, "C@uint16_t", 16);
  if (err != LLACE_ERROR_NONE) return err;
  
  // int32_t
  err = add_int_type(module, "C@int32_t", 32);
  if (err != LLACE_ERROR_NONE) return err;
  
  // uint32_t
  err = add_uint_type(module, "C@uint32_t", 32);
  if (err != LLACE_ERROR_NONE) return err;
  
  // int64_t
  err = add_int_type(module, "C@int64_t", 64);
  if (err != LLACE_ERROR_NONE) return err;
  
  // uint64_t
  err = add_uint_type(module, "C@uint64_t", 64);
  if (err != LLACE_ERROR_NONE) return err;
  
  err = add_float_type(module, "C@float", 23, 8); // IEEE 754 Single
  if (err != LLACE_ERROR_NONE) return err;
  
  err = add_float_type(module, "C@double", 52, 11); // IEEE 754 double
  if (err != LLACE_ERROR_NONE) return err;
  
  err = add_float_type(module, "C@long double", 52, 11); // IEEE 754 double
  if (err != LLACE_ERROR_NONE) return err;

  err = add_void_type(module, "C@void");
  if (err != LLACE_ERROR_NONE) return err;
  
  return LLACE_ERROR_NONE;
}

llace_type_t *llace_builderc64_gettype(llace_module_t *module, const char *name) {
  if (!module || !name) return NULL;
  
  // Search through all types in the module
  size_t type_count = llace_module_type_count(module);
  for (size_t i = 0; i < type_count; i++) {
    llace_type_t *type = llace_module_get_type(module, i);
    if (type && type->name && strcmp(type->name, name) == 0) {
      return type;
    }
  }
  
  return NULL;
}

llace_typeref_t llace_builderc64_gettype_ref(llace_module_t *module, const char *name) {
  if (!module || !name) return SIZE_MAX;
  
  // Search through all types in the module
  size_t type_count = llace_module_type_count(module);
  for (size_t i = 0; i < type_count; i++) {
    llace_type_t *type = llace_module_get_type(module, i);
    if (type && type->name && strcmp(type->name, name) == 0) {
      return i;
    }
  }
  
  return SIZE_MAX; // Not found
}