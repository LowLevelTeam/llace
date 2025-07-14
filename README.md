# Low Level Assembly & Compilation Engine (LLACE)

## Overview

LLACE is a modern compiler infrastructure and toolchain designed as a lightweight, efficient alternative to LLVM. Built entirely in C, LLACE prioritizes simplicity, speed, and maintainability while providing a complete compilation pipeline from source code to machine code.

To utilize the LLACE project make use of the static development library 'libllace-dev' made to be easily integratable into compilers. Thanks to being written in C only simple wrappers will be needed to make this work with your own custom language or the hgih level language you wish to use.

## Why LLACE?

- **Simplicity**: Clean C codebase with minimal abstractions
- **Performance**: Optimized for fast compilation times
- **Efficiency**: Smaller memory footprint and binary size
- **Modularity**: Clear separation between compilation phases
- **Learning-Focused**: Designed for educational purposes and experimentation

## Architecture

LLACE is built around a streamlined Intermediate Representation (IR) system with three core phases:

```
[Configuration] -> [Building] -> [Optimizing] -> [Generation]
    (optional)
```

### Configuration (Optional)
Custom configuration scripts allow fine-grained control over the compilation process without relying on complex command-line arguments. This provides flexibility for advanced users while keeping the default interface simple.

### Building
The building phase transforms source code into LLACE's high-level IR. This stage focuses on minimal abstractions, making the IR both human-readable and efficient to process.

### Optimizing
Multiple optimization passes can be applied based on compilation flags or custom configurations. The modular design allows users to select specific optimizations or create custom optimization pipelines.

### Generation
The final code generation phase uses a target-specific architecture where each platform has its own dedicated code generator. This approach ensures optimal code quality while maintaining clear separation between architectures.

#### Target System
Unlike traditional target triplets, LLACE uses a more intuitive target specification system. Each architecture maintains its own code generator responsible for:

- Machine code generation
- Object file formatting (ELF, Mach-O, PE, etc.)
- Architecture-specific optimizations

**Shared Components**: Common object format implementations are located in `src/detail/` to avoid duplication across architectures (i386, amd64, ARM32, ARM64).

**Virtual Machine Targets**: Support for VM targets (UXN, WebAssembly, JVM) with specialized generators that don't require traditional object formats.

## Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details on:
- Code style guidelines
- Testing procedures
- Submission process

## License

LLACE is released under the UnLicense. See [LICENSE](LICENSE) for details.

## Team

LLACE is developed by the Low Level Team (LLT), a group of engineers focused on creating efficient, modern compiler infrastructure.

## Acknowledgments

While LLACE is an independent project, we acknowledge the pioneering work of the LLVM project in advancing compiler infrastructure design.