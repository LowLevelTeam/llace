#ifndef LLACE_IR_H
#define LLACE_IR_H

#include <llace/llace.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================ Opcode ================ //

typedef enum llace_opcode {
  // Arithmetic
  LLACE_OP_ADD,
  LLACE_OP_SUB,
  LLACE_OP_MUL,
  LLACE_OP_DIV,
  LLACE_OP_MOD,
  
  // Bitwise
  LLACE_OP_AND,
  LLACE_OP_OR,
  LLACE_OP_XOR,
  LLACE_OP_SHL,
  LLACE_OP_SHR,
  
  // Comparison
  LLACE_OP_EQ,
  LLACE_OP_NE,
  LLACE_OP_LT,
  LLACE_OP_LE,
  LLACE_OP_GT,
  LLACE_OP_GE,
  
  // Memory
  LLACE_OP_LOAD,
  LLACE_OP_STORE,
  LLACE_OP_ALLOCA,
  
  // Control flow
  LLACE_OP_JMP,
  LLACE_OP_BR,
  LLACE_OP_CALL,
  LLACE_OP_RET,
} llace_opcode_t;

// ================ Value ================ //

typedef enum llace_value_kind {
  LLACE_VALUE_CONST, // Constant value
  LLACE_VALUE_INSTRUCTION, // Instruction
} llace_value_kind_t;

typedef struct llace_value_constant {
  llace_type_t *type; // Type of the constant value
  void *data; // Pointer to the constant data (e.g., integer, float)
} llace_value_constant_t;

typedef struct llace_value_instruction {
  llace_opcode_t opcode; // Opcode of the instruction
  unsigned operand_count; // Number of operands
  llace_value_t *operands; // Operands for the instruction
  llace_value_t result; // Result value
} llace_value_instruction_t;

typedef struct llace_value {
  llace_value_kind_t kind; // Value kind
  
  union {
    llace_value_constant_t constant; // Constant value (if kind is LLACE_VALUE_CONST)
    llace_value_instruction_t instruction; // Instruction (if kind is LLACE_VALUE_INSTRUCTION)
  };
} llace_value_t;

// ================ Instruction ================ //

// Instruction structure
typedef struct llace_instruction {
  llace_opcode_t opcode;
  llace_value_t *operands; // Operands for the instruction
  size_t operand_count; // Number of operands
} llace_instruction_t;

// ================ ABIs ================ //

typedef enum llace_abi {
  // If used then no arguments are passed, nothing is returned and function can not return. 
  // Essentially used for jumps.
  LLACE_ABI_NULL, 
  LLACE_ABI_CDECL,
  // introducing more for operating system specific conventions
  // for system calls, specific calling conventions, etc.
} llace_abi_t;

const char *llace_abi_str(llace_abi_t abi);

// ================ Type ================ //

// Type kinds
typedef enum {
  LLACE_TYPE_VOID,
  LLACE_TYPE_INT,
  LLACE_TYPE_UNT,
  LLACE_TYPE_FLOAT,
  LLACE_TYPE_PTR,
  // LLACE_TYPE_VPTR, // Opaque/void pointer
  // LLACE_TYPE_ARRAY,
  // LLACE_TYPE_STRUCT,
  // LLACE_TYPE_FUNCTION,
  // LLACE_TYPE_VARADIC,
} llace_type_kind_t;

// Type structure
typedef struct llace_type {
  const char *name; // Type name (for debugging)
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
    llace_type_t *_ptr; // pointee type (types are defined in the context)
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

// allocate type
llace_type_t *llace_typea_create(llace_type_kind_t kind, size_t size, size_t alignment);
llace_type_t *llace_typea_void(void);
llace_type_t *llace_typea_int(unsigned long bit_width);
llace_type_t *llace_typea_unt(unsigned long bit_width);
llace_type_t *llace_typea_float(unsigned long mantissa, unsigned long exponent);
llace_type_t *llace_typea_pointer(llace_type_t *pointee);

// create type
llace_type_t llace_typec_create(llace_type_kind_t kind, size_t size, size_t alignment);
llace_type_t llace_typec_void(void);
llace_type_t llace_typec_int(unsigned long bit_width);
llace_type_t llace_typec_unt(unsigned long bit_width);
llace_type_t llace_typec_float(unsigned long mantissa, unsigned long exponent);
llace_type_t llace_typec_pointer(llace_type_t *pointee);

// initalize type
void llace_typei_create(llace_type_t *type, llace_type_kind_t kind, size_t size, size_t alignment);
void llace_typei_void(llace_type_t *type, void);
void llace_typei_int(llace_type_t *type, unsigned long bit_width);
void llace_typei_unt(llace_type_t *type, unsigned long bit_width);
void llace_typei_float(llace_type_t *type, unsigned long mantissa, unsigned long exponent);
void llace_typei_pointer(llace_type_t *type, llace_type_t *pointee);

// ================ Globals ================ //

typedef struct llace_global {
  const char *name; // symbol name
  llace_type_t *type; // Type of the global variable
  llace_value_t *value; // Initial value (if any) (required for constants)

  // size of the global variable data
  // This is the size of the data in memory, not the size of the type.
  size_t size; // Size in bytes (rounded up to nearest byte)

  // allignment of the global variable data
  // This is the allignment of the data in memory, not the allignment of the type.
  size_t alignment; // Alignment in bytes (rounded up to nearest power of two)

  // Global attributes
  unsigned is_constant : 1; // If true, the value cannot be changed after initialization
  unsigned is_public : 1; // If true, the global is visible outside this module
  unsigned is_extern : 1; // If true, the global is declared but not defined in this module
  // unsigned is_tls : 1; // If true, the global is thread-local storage
  // unsigned is_weak : 1; // If true, the global can be overridden by another definition
  // unsigned is_linkonce : 1; // If true, the global can be merged with other definitions
  // unsigned is_compvalue : 1; // If true, the global is a compiler value (compiler time value)
} llace_global_t;

// ================ Functions ================ //

typedef struct llace_function {
  const char *name; // symbol name
  llace_abi_t abi; // calling convention

  unsigned long return_count; // Number of return values
  llace_type_t *returns; // Return types

  unsigned long parameter_count; // Number of parameters
  llace_type_t *parameters; // Parameter types

  // Function body
  llace_instruction_t *instructions; // First instruction in the function

  // Function attributes
  unsigned _extern : 1; // If true, the function is declared but not defined
  unsigned _public : 1; // If true, the function is visible outside this module
  // unsigned _const : 1; // If true, the function has no side effects and does not modify or inspect any global state
  // unsigned _pure : 1; // If true, the function has no side effects but can inspect global state
  // unsigned _noreturn : 1; // If true, the function does not return to the caller
  // unsigned _vararg : 1; // If true, the function can take a variable number of arguments
  // unsigned _inline : 1; // If true, the function can be inlined by the compiler
  // unsigned _noinline : 1; // If true, the function should not be inlined by the compiler
  // unsigned _alwaysinline : 1; // If true, the function should always be inlined by the compiler
  // unsigned _optimize : 1; // If true, the function should be optimized by the compiler
  // unsigned _nooptimize : 1; // If true, the function should not be optimized by the compiler
  // unsigned _linkonce : 1; // If true, the function can be merged with other definitions
  // unsigned _weak : 1; // If true, the function can be overridden by another definition
} llace_function_t;

// ================ Module ================ //

// Module structure
typedef struct llace_module {
  const char *name; // Module name (typically filename)
  
  // Type definitions
  llace_type_t *types; // type array
  size_t type_count; // number of types

  // Global variables
  llace_global_t *globals; // symbols to data
  size_t global_count;

  // Functions
  llace_function_t *function; // symbols to code
  size_t function_count;
} llace_module_t;

void llace_module_init(llace_module_t *module, const char *name);
void llace_module_destroy(llace_module_t *module);
void llace_module_add_type(llace_module_t *module, llace_type_t *type);
void llace_module_add_global(llace_module_t *module, llace_global_t *global);
void llace_module_add_function(llace_module_t *module, llace_function_t *function);

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_H