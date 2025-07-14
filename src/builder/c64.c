// src/builder/c64.c
#include <llace/builder/c64.h>
#include <llace/ir.h>
#include <string.h>
#include <assert.h>

// ================ Type Registry ================ //

const char *typemap[] = {
  "bool",
  "char",
  "unsigned char",
  "short",
  "unsigned short",
  "int",
  "unsigned int",
  "long",
  "unsigned long",
  "long long",
  "unsigned long long",
  "float",
  "double",
  "long dobule",
  "void"
};

// ================ Public Functions ================ //

llace_error_t llace_builderc64_types(llace_module_t *module) {
  assert(module != NULL);
  
  if (module->types != NULL || module->type_count > 0) {
    // If types already exist, we assume they are already initialized
    return;
  }
  
  llace_type_t type;
  llace_error_t error;

  // Boolean type (typically 1 byte)
  llace_typei_int(&type, 1); // 1-bit boolean
  bool_type->name = "bool";
  bool_type->size = 1;
  bool_type->alignment = 1;
  error = llace_module_add_type(module, bool_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  // Character types (8-bit)
  llace_typei_int(&type, 8);
  char_type->name = "char";
  char_type->size = 1;
  char_type->alignment = 1;
  error = llace_module_add_type(module, char_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  llace_typei_unt(&type, 8);
  uchar_type->name = "unsigned char";
  uchar_type->size = 1;
  uchar_type->alignment = 1;
  error = llace_module_add_type(module, uchar_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  // Short types (16-bit)
  llace_typei_int(&type, 16);
  short_type->name = "short";
  short_type->size = 2;
  short_type->alignment = 2;
  error = llace_module_add_type(module, short_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  llace_typei_unt(&type, 16);
  ushort_type->name = "unsigned short";
  ushort_type->size = 2;
  ushort_type->alignment = 2;
  error = llace_module_add_type(module, ushort_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  // Integer types (32-bit)
  llace_typei_int(&type, 32);
  int_type->name = "int";
  int_type->size = 4;
  int_type->alignment = 4;
  error = llace_module_add_type(module, int_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  llace_typei_unt(&type, 32);
  uint_type->name = "unsigned int";
  uint_type->size = 4;
  uint_type->alignment = 4;
  error = llace_module_add_type(module, uint_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  // Long types (64-bit on 64-bit systems)
  llace_typei_int(&type, 64);
  long_type->name = "long";
  long_type->size = 8;
  long_type->alignment = 8;
  error = llace_module_add_type(module, long_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  llace_typei_unt(&type, 64);
  ulong_type->name = "unsigned long";
  ulong_type->size = 8;
  ulong_type->alignment = 8;
  error = llace_module_add_type(module, ulong_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  // Long long types (64-bit)
  llace_typei_int(&type, 64);
  longlong_type->name = "long long";
  longlong_type->size = 8;
  longlong_type->alignment = 8;
  error = llace_module_add_type(module, longlong_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  llace_typei_unt(&type, 64);
  ulonglong_type->name = "unsigned long long";
  ulonglong_type->size = 8;
  ulonglong_type->alignment = 8;
  error = llace_module_add_type(module, ulonglong_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  // Floating-point types
  // IEEE 754 single precision: 23-bit mantissa, 8-bit exponent (sign bit implicit)
  llace_typei_float(&type, 23, 8);
  float_type->name = "float";
  float_type->size = 4;
  float_type->alignment = 4;
  error = llace_module_add_type(module, float_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  // IEEE 754 double precision: 52-bit mantissa, 11-bit exponent (sign bit implicit)
  llace_typei_float(&type, 52, 11);
  double_type->name = "double";
  double_type->size = 8;
  double_type->alignment = 8;
  error = llace_module_add_type(module, double_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  // x86-64 extended precision: 64-bit mantissa, 15-bit exponent (sign bit implicit)
  llace_typei_float(&type, 64, 15);
  longdouble_type->name = "long double";
  longdouble_type->size = 16; // 80-bit padded to 128-bit
  longdouble_type->alignment = 16;
  error = llace_module_add_type(module, longdouble_type);
  if (error != LLACE_ERROR_NONE) return error;
  
  // Void type
  llace_typei_void(&type);
  void_type->name = "void";
  error = llace_module_add_type(module, void_type);
  if (error != LLACE_ERROR_NONE) return error;
}


llace_type_t *llace_builderc64_gettype(llace_module_t *module, const char *name) {
  assert(module != NULL);
  assert(name != NULL);
  
  for (size_t i = 0; i < sizeof(typemap) / sizeof(typemap[0]); i++) {
    if (strcmp(name, typemap[i]) == 0) {
      return llace_module_get_type(module, i);
    }
  }

  return NULL; // Type not found
}