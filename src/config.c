#include "llace/config.h"

const char *llace_arch_str(llace_arch_t arch) {
  switch (arch) {
  case LLACE_ARCH_X86:      return "x86";
  case LLACE_ARCH_I386:     return "i386";
  case LLACE_ARCH_AMD64:    return "amd64";
  case LLACE_ARCH_ARM32:    return "ARM32";
  case LLACE_ARCH_ARM64:    return "ARM64";
  case LLACE_ARCH_RISCV32:  return "RISC-V32";
  case LLACE_ARCH_RISCV64:  return "RISC-V64";
  case LLACE_ARCH_MIPS:     return "MIPS32";
  case LLACE_ARCH_MIPS64:   return "MIPS64";
  case LLACE_ARCH_SPARC:    return "SPARC32";
  case LLACE_ARCH_SPARC64:  return "SPARC64";
  case LLACE_ARCH_PPC:      return "PowerPC32";
  case LLACE_ARCH_PPC64:    return "PowerPC64";
  case LLACE_ARCH_UXN:      return "UXN virtual machine";
  case LLACE_ARCH_WASM32:   return "WASM32";
  case LLACE_ARCH_WASM64:   return "WASM64";
  case LLACE_ARCH_JVM:      return "JVM";
  default:                  return "INVALID";
  };
}

const char *llace_os_str(llace_os_t os) {
  switch (os) {
  case LLACE_OS_NONE:     return "Bare metal / no OS";
  case LLACE_OS_LINUX:    return "Linux";
  case LLACE_OS_WINDOWS:  return "Windows";
  case LLACE_OS_MACOS:    return "macOS";
  case LLACE_OS_FREEBSD:  return "FreeBSD";
  case LLACE_OS_OPENBSD:  return "OpenBSD";
  case LLACE_OS_NETBSD:   return "NetBSD";
  case LLACE_OS_SOLARIS:  return "Solaris";
  case LLACE_OS_AIX:      return "IBM AIX";
  case LLACE_OS_WASI:     return "WebAssembly System Interface";
  case LLACE_OS_BROWSER:  return "Browser environment";
  case LLACE_OS_NODE:     return "Node.js environment";
  default:                return "INVALID";
  };
}

const char *llace_objfmt_str(llace_objfmt_t fmt) {
  switch (fmt) {
  case LLACE_OBJFMT_BINARY:    return "Raw binary (no format)";
  case LLACE_OBJFMT_ELF32:     return "32-bit ELF";
  case LLACE_OBJFMT_ELF64:     return "64-bit ELF";
  case LLACE_OBJFMT_PE32:      return "32-bit Portable Executable";
  case LLACE_OBJFMT_PE64:      return "64-bit Portable Executable (PE+)";
  case LLACE_OBJFMT_MACH_O_32: return "32-bit Mach-O";
  case LLACE_OBJFMT_MACH_O_64: return "64-bit Mach-O";
  case LLACE_OBJFMT_COFF:      return "Common Object File Format";
  case LLACE_OBJFMT_XCOFF:     return "Extended COFF (AIX)";
  case LLACE_OBJFMT_WASM:      return "WebAssembly module";
  case LLACE_OBJFMT_CLASSFILE: return "Java Class file";
  case LLACE_OBJFMT_UXN_ROM:   return "UXN ROM format";
  case LLACE_OBJFMT_BYTECODE:  return "Generic bytecode format";
  default: return "INVALID";
  };
}

const char *llace_abi_str(llace_abi_t abi) {
  switch (abi) {
  case LLACE_ABI_NONE:    return "No specific ABI";
  case LLACE_ABI_CDECL:   return "C calling convention";
  case LLACE_ABI_STDCALL: return "Standard call";
  default: return "INVALID";
  };
}

const char *llace_endian_str(llace_endian_t endian) {
  switch (endian) {
  case LLACE_ENDIAN_LITTLE: return "Little endian";
  case LLACE_ENDIAN_BIG:    return "Big endian";
  case LLACE_ENDIAN_AUTO:   return "Use architecture default";
  default: return "INVALID";
  };
}

void llace_target_log(const llace_target_t *target) {
  size_t addrwidth = llace_target_pointer_size(target);
  size_t wordwidth = llace_target_word_size(target);

  LLACE_LOG_INFO("%s - (%s | %s | %s ) - (%s | %s ) - %s - (word(%zu) | addr(%zu))",
    llace_os_str(target->os),
    llace_arch_str(target->arch),
    llace_objfmt_str(target->format),
    llace_endian_str(target->endian),
    target->vendor, target->env, target->version,
    wordwidth, addrwidth,
    llace_target_is_supported(target) ? "Supported ✅" : "Not Supported ❌"
  );

}

bool llace_target_is_supported(const llace_target_t *target) {
  if (target->endian == LLACE_ENDIAN_LITTLE) {
    if (target->os == LLACE_OS_NONE) {
      if (target->format == LLACE_OBJFMT_BINARY) {
        if (target->arch == LLACE_ARCH_AMD64) {
          return true;
        }
      }
    }
  }

  return false;
}

void llace_target_get_host(llace_target_t *target) {
  target->arch = LLACE_ARCH_AMD64;
  target->os = LLACE_OS_NONE;
  target->format = LLACE_OBJFMT_BINARY;
  target->endian = LLACE_ENDIAN_LITTLE;

  target->vendor = "pc";
  target->env = "gnu";
  target->version = "1.0.0";
}

size_t llace_target_pointer_size(const llace_target_t *target) {
  if (target->arch == LLACE_ARCH_AMD64) {
    return 64; // it may be technically 48?
  }
  return 0;
}
size_t llace_target_word_size(const llace_target_t *target) {
  if (target->arch == LLACE_ARCH_AMD64) {
    return 64; // it may be technically 48?
  }
  return 0;
}

llace_error_t llace_config_init(llace_config_t *config) {
  if (!config) {
    return LLACE_ERROR_BADARG;
  }

  llace_target_get_host(&config->target);

  config->verbose = 0;
  config->debug = 0;

  config->position_independent = 0;
  config->shared_library = 0;
  config->static_library = 0;
  config->generate_debug_info = 0;
  config->generate_symbol_table = 0;
  config->filename = "output.o";

  return LLACE_ERROR_NONE;
}

bool llace_config_valid(const llace_config_t *config) {
  if (!llace_target_is_supported(&config->target)) {
    return false;
  }

  // can be none, or one but not both
  if (config->shared_library && config->static_library) {
    return false;
  }

  if (config->filename == NULL) {
    return false;
  }

  return true;
}
