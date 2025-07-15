#ifndef LLACE_CONFIG_H
#define LLACE_CONFIG_H

#include "llace/llace.h"

#ifdef __cplusplus
extern "C" {
#endif

// ================ Target Architecture ================ //

typedef enum {
  LLACE_ARCH_X86,        // 32-bit x86
  LLACE_ARCH_X86_64,     // 64-bit x86 (AMD64)
  LLACE_ARCH_ARM32,      // 32-bit ARM
  LLACE_ARCH_ARM64,      // 64-bit ARM (AArch64)
  LLACE_ARCH_RISCV32,    // 32-bit RISC-V
  LLACE_ARCH_RISCV64,    // 64-bit RISC-V
  LLACE_ARCH_MIPS,       // 32-bit MIPS
  LLACE_ARCH_MIPS64,     // 64-bit MIPS
  LLACE_ARCH_SPARC,      // 32-bit SPARC
  LLACE_ARCH_SPARC64,    // 64-bit SPARC
  LLACE_ARCH_PPC,        // 32-bit PowerPC
  LLACE_ARCH_PPC64,      // 64-bit PowerPC
  // Virtual Machine "Architectures"
  LLACE_ARCH_UXN,        // UXN virtual machine
  LLACE_ARCH_WASM32,     // WebAssembly 32-bit
  LLACE_ARCH_WASM64,     // WebAssembly 64-bit
  LLACE_ARCH_JVM,        // Java Virtual Machine
} llace_arch_t;

// ================ Target Operating System ================ //

typedef enum {
  LLACE_OS_NONE,         // Bare metal / no OS
  LLACE_OS_LINUX,        // Linux
  LLACE_OS_WINDOWS,      // Windows
  LLACE_OS_MACOS,        // macOS
  LLACE_OS_FREEBSD,      // FreeBSD
  LLACE_OS_OPENBSD,      // OpenBSD
  LLACE_OS_NETBSD,       // NetBSD
  LLACE_OS_SOLARIS,      // Solaris
  LLACE_OS_AIX,          // IBM AIX
  // Virtual environments
  LLACE_OS_WASI,         // WebAssembly System Interface
  LLACE_OS_BROWSER,      // Browser environment
  LLACE_OS_NODE,         // Node.js environment
} llace_os_t;

// ================ Target Object Format ================ //

typedef enum {
  LLACE_OBJFMT_ELF32,    // 32-bit ELF
  LLACE_OBJFMT_ELF64,    // 64-bit ELF
  LLACE_OBJFMT_PE32,     // 32-bit Portable Executable
  LLACE_OBJFMT_PE64,     // 64-bit Portable Executable (PE+)
  LLACE_OBJFMT_MACH_O_32,// 32-bit Mach-O
  LLACE_OBJFMT_MACH_O_64,// 64-bit Mach-O
  LLACE_OBJFMT_COFF,     // Common Object File Format
  LLACE_OBJFMT_XCOFF,    // Extended COFF (AIX)
  // Virtual machine formats
  LLACE_OBJFMT_WASM,     // WebAssembly module
  LLACE_OBJFMT_CLASSFILE,// Java Class file
  LLACE_OBJFMT_UXN_ROM,  // UXN ROM format
  LLACE_OBJFMT_BYTECODE, // Generic bytecode format
  LLACE_OBJFMT_BINARY,   // Raw binary (no format)
} llace_objfmt_t;

// ================ Target ABI ================ //

typedef enum {
  LLACE_ABI_NONE,        // No specific ABI
  LLACE_ABI_CDECL,       // C calling convention
  LLACE_ABI_STDCALL,     // Standard call
} llace_abi_t;

// ================ Target Endianness ================ //

typedef enum {
  LLACE_ENDIAN_LITTLE,   // Little endian
  LLACE_ENDIAN_BIG,      // Big endian
  LLACE_ENDIAN_AUTO      // Use architecture default
} llace_endian_t;

// ================ Target Triple ================ //

typedef struct {
  llace_arch_t arch;     // Target architecture
  llace_os_t os;         // Target operating system
  llace_objfmt_t format; // Object file format
  llace_abi_t abi;       // Application Binary Interface
  llace_endian_t endian; // Byte order
  
  // Optional vendor/environment info
  const char *vendor;    // Vendor string (e.g., "pc", "apple", "unknown")
  const char *env;       // Environment string (e.g., "gnu", "musl", "msvc")
  const char *version;   // Version string
} llace_target_t;

// ================ Target Utilities ================ //

/**
 * Check if a target is supported by this LLACE build
 * @param target Target to check
 * @return true if supported, false otherwise
 */
bool llace_target_is_supported(const llace_target_t *target);

/**
 * Get string representation of a target
 * @param target Target to stringify
 * @param buffer Buffer to write string to
 * @param buffer_size Size of buffer
 * @return Number of characters written (excluding null terminator)
 */
size_t llace_target_to_string(const llace_target_t *target, char *buffer, size_t buffer_size);

/**
 * Get default target for current host
 * @param target Output target structure
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_target_get_host(llace_target_t *target);

/**
 * Check if target uses virtual machine
 * @param target Target to check
 * @return true if VM target, false for native
 */
bool llace_target_is_vm(const llace_target_t *target);

/**
 * Check if target uses bytecode
 * @param target Target to check
 * @return true if bytecode target, false otherwise
 */
bool llace_target_is_bytecode(const llace_target_t *target);

/**
 * Get pointer size for target in bytes
 * @param target Target to check
 * @return Pointer size in bytes
 */
size_t llace_target_pointer_size(const llace_target_t *target);

/**
 * Get word size for target in bytes
 * @param target Target to check
 * @return Word size in bytes
 */
size_t llace_target_word_size(const llace_target_t *target);

// ================ Configuration Structure ================ //

typedef struct {
  // Target specification
  llace_target_t target;

  // Flags
  unsigned int verbose : 1;                // Verbose output

  // Code generation settings
  unsigned int position_independent : 1;   // Generate position-independent code
  unsigned int shared_library : 1;         // Building shared library
  unsigned int static_library : 1;         // Building static library
  unsigned int generate_debug_info : 1;    // Include debug information
  unsigned int generate_symbol_table : 1;  // Include symbol table
  size_t stack_alignment;                  // Stack alignment requirement
  const char *filename;             // Output file name
} llace_config_t;

// ================ Configuration Functions ================ //

/**
 * Initialize configuration with default values
 * @param config Configuration to initialize
 * @param target Target to configure for (NULL for host default)
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_config_init(llace_config_t *config, const llace_target_t *target);

/**
 * Validate configuration
 * @param config Configuration to validate
 * @return LLACE_ERROR_NONE if valid, error code otherwise
 */
llace_error_t llace_config_validate(const llace_config_t *config);

/**
 * Set output filename
 * @param config Configuration to modify
 * @param filename Output filename
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_config_set_output(llace_config_t *config, const char *filename);

/**
 * Enable debug information
 * @param config Configuration to modify
 * @param enable Whether to enable debug info
 */
void llace_config_set_debug(llace_config_t *config, bool enable);

#ifdef __cplusplus
}
#endif

#endif // LLACE_CONFIG_H