# Low Level Assembly & Compilation Engine (LLACE)

## Overview

LLACE is a modern compiler infrastructure and toolchain designed as a lightweight, efficient alternative to LLVM. Built entirely in C, LLACE prioritizes simplicity, speed, and maintainability while providing a complete compilation pipeline from source code to machine code through a sophisticated multi-stage Intermediate Representation (IR) system.

To utilize the LLACE project, make use of the static development library 'libllace-dev' designed to be easily integratable into compilers. Thanks to being written in C, only simple wrappers will be needed to make this work with your own custom language or high-level language of choice.

## Why LLACE?

- **Multi-Stage IR Design**: Three-tier IR system balancing semantic preservation with optimization power
- **Future-Proof Type System**: Abstract bitwidth integers, floating-point formats, and extensible vector types
- **Clean Separation**: Distinct phases for frontend construction, optimization, and target-specific code generation
- **Performance**: Optimized for fast compilation times with efficient memory usage
- **Modularity**: Each compilation stage has clear responsibilities and interfaces
- **Learning-Focused**: Designed for educational purposes and compiler research

## Architecture

LLACE employs a sophisticated three-stage IR architecture that addresses the fundamental tension between semantic preservation and optimization enablement:

```
Source Code → High-Level IR → SSA IR → Target-Specific Codegen → Machine Code
             (Semantic)     (Optimizable)    (Realizable)
```

### Stage 1: High-Level IR (Semantic Preservation)
**Key Principle**: *"What the programmer meant, not how to execute it"*

The High-Level IR maintains source language constructs and programmer intent:

- **Rich Type Abstraction**: Supports arbitrary bitwidths (`i37`, `i127`), abstract floating-point formats (`f23.8`, `f52.11`), and extensible vector types (`vec8<i13>`)
- **Structural Control Flow**: Preserves `while`, `for`, `if-else` as semantic units rather than basic blocks
- **Nested Expressions**: Tree-based expression evaluation with embedded instructions
- **Scoped Symbol Resolution**: Block-based variable scoping with semantic meaning
- **Future-Proofing**: Extensible for new hardware features like tensors, fixed-point arithmetic, and custom SIMD operations

```c
// Example: High-level IR representation
struct hl_while_node {
  hl_expr_t *condition;     // Complex expression tree
  hl_block_t *body;         // Nested statements
  optimization_hints_t hints; // Vectorization potential, etc.
};

struct hl_type {
  type_kind_t kind;
  union {
    struct { size_t bitwidth; } integer;        // i37, i127, etc.
    struct { size_t mantissa, exponent; } float; // f19.8, f52.11, etc.
    struct { hl_type_t *element; size_t count; } vector; // vec8<i13>
  };
};
```

### Stage 2: SSA IR (Optimization-Focused)
**Key Principle**: *"Optimizable representation of programmer intent"*

The SSA IR enables sophisticated optimizations while preserving type semantics:

- **Control Flow Graph**: Explicit basic blocks, phi nodes, and dominance relationships
- **Single Static Assignment**: Every value has a single definition with clear use-def chains
- **Type-Preserving Lowering**: Abstract types flow through but expressions are flattened
- **Target-Independent Optimization**: Dead code elimination, common subexpression elimination, loop optimizations
- **Capability Hints**: Annotations for vectorization potential, library call requirements, and hardware features

```c
// Example: SSA IR with preserved type information
%0 = vector_add vec4<i13> %a, %b  // Single SSA instruction
                                  // Type info preserved for target query
```

### Stage 3: Target-Specific Codegen (Hardware Realization)
**Key Principle**: *"Optimal implementation for specific hardware"*

Target-specific code generators make final implementation decisions:

- **Hardware Capability Queries**: Runtime decisions based on actual target features
- **Type Realization**: Abstract types mapped to concrete hardware representations
- **Instruction Selection**: Complex operations broken down to target instruction sets
- **Architecture-Specific Optimizations**: Peephole optimizations, instruction scheduling
- **Object Format Generation**: ELF, Mach-O, PE, WebAssembly, etc.

```c
// Example: Target-specific decision making
llace_error_t wasm32_gen_instruction(ssa_instruction_t *inst) {
  switch(inst->opcode) {
    case SSA_VECTOR_ADD:
      if (wasm_supports_simd() && is_native_vector_width(inst->result_type)) {
        emit_wasm_v128_add();
      } else {
        emit_scalarized_loop();  // Graceful fallback
      }
      break;
  }
}
```

## Supported Targets

LLACE uses an intuitive target specification system where each architecture maintains its own code generator:

### Native Architectures
- **x86**: 16-bit x86 with full instruction set support
- **i386**: 32-bit x86 with full instruction set support
- **AMD64**: 64-bit x86 with AVX/SSE optimizations
- **ARM32/ARM64**: ARM architectures with NEON support
- **RISC-V 32/64**: Open instruction set architecture
- **MIPS 32/64**: Classic RISC architecture
- **SPARC 32/64**: Sun SPARC processors
- **PowerPC 32/64**: IBM PowerPC architecture

### Virtual Machine Targets
- **WebAssembly (WASM32/64)**: Browser and server deployment
- **UXN Virtual Machine**: Ultra-minimal computing platform
- **Java Virtual Machine (JVM)**: Bytecode generation for JVM languages

### Object Format Support
- **ELF**: Linux, BSD systems (32/64-bit)
- **Mach-O**: macOS, iOS (32/64-bit)
- **PE/PE+**: Windows executables (32/64-bit)
- **COFF/XCOFF**: Common object formats
- **Raw Binary**: Embedded and bare-metal targets

## Benefits of LLACE's Architecture

1. **Future-Proof**: New hardware features only require SSA IR opcode additions and target-specific codegen
2. **Optimization-Friendly**: SSA IR enables all standard optimizations while preserving semantic types
3. **Clean Separation**: Each stage has clear responsibilities and well-defined interfaces
4. **Target Flexibility**: Code generators make final realization decisions based on actual capabilities
5. **Debugging Support**: Can trace execution from source → high-level IR → SSA IR → machine code
6. **Semantic Preservation**: High-level constructs maintained throughout the compilation pipeline

## Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details on:
- Code style guidelines
- Multi-stage IR development
- Target architecture implementation
- Testing procedures
- Submission process

## License

LLACE is released under the UnLicense. See [LICENSE](LICENSE) for details.

## Team

LLACE is developed by the Low Level Team (LLT), a group of engineers focused on creating efficient, modern compiler infrastructure that balances theoretical soundness with practical implementation needs.

## Acknowledgments

While LLACE is an independent project, we acknowledge the pioneering work of LLVM, GCC, and other compiler infrastructures in advancing the field of compilation technology. LLACE builds upon these lessons while exploring new approaches to IR design and optimization.