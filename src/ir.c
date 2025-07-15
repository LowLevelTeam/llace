#include <llace/ir.h>
#include <llace/detail/common.h>

// ================ IR Main Implementation ================ //

// This file serves as the main entry point for the IR system
// All individual components are implemented in their respective files:
// - ir/type.c - Type system
// - ir/value.c - Value system
// - ir/var.c - Variable system
// - ir/global.c - Global variable system
// - ir/func.c - Function system
// - ir/instr.c - Instruction system
// - ir/cinstr.c - Child instruction system
// - ir/module.c - Module system

// The IR system provides a complete intermediate representation
// for building and manipulating code structures in memory.

// Example usage:
// 1. Initialize a module with llace_module_init()
// 2. Add types with llace_module_add_type()
// 3. Add globals with llace_module_add_global()
// 4. Add functions with llace_module_add_function()
// 5. Build instruction sequences within functions
// 6. Pass the complete module to code generation

// All memory management is handled through the llace_mem_* system
// to ensure efficient allocation and deallocation.