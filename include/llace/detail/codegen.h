/**
 * LLACE Object Writer - Unified Object File Generation Interface
 * 
 * Low Level Assembly & Compilation Engine (LLACE)
 * 
 * This header provides a unified interface for writing machine code to various
 * object file formats including ELF32/64, PE/PE+, MACH-O, DWARF, and raw binary.
 */

#ifndef LLACE_OBJECT_WRITER_H
#define LLACE_OBJECT_WRITER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================================================ Enumerations ================================================ //

/**
 * Object file format enumeration
 */
typedef enum {
  LLACE_FORMAT_ELF32,        /* 32-bit ELF */
  LLACE_FORMAT_ELF64,        /* 64-bit ELF */
  LLACE_FORMAT_PE32,         /* 32-bit Portable Executable */
  LLACE_FORMAT_PE64,         /* 64-bit Portable Executable (PE+) */
  LLACE_FORMAT_MACH_O_32,    /* 32-bit Mach-O */
  LLACE_FORMAT_MACH_O_64,    /* 64-bit Mach-O */
  LLACE_FORMAT_COFF,         /* Common Object File Format */
  LLACE_FORMAT_DWARF,        /* DWARF debug information */
  LLACE_FORMAT_BINARY,       /* Raw binary (no format) */
  LLACE_FORMAT_AUTO          /* Auto-detect based on target */
} llace_format_t;

/**
 * Target architecture enumeration
 */
typedef enum {
  LLACE_ARCH_X86,
  LLACE_ARCH_X86_64,
  LLACE_ARCH_ARM,
  LLACE_ARCH_ARM64,
  LLACE_ARCH_RISCV32,
  LLACE_ARCH_RISCV64,
  LLACE_ARCH_MIPS,
  LLACE_ARCH_MIPS64,
  LLACE_ARCH_SPARC,
  LLACE_ARCH_SPARC64,
  LLACE_ARCH_PPC,
  LLACE_ARCH_PPC64
} llace_arch_t;

/**
 * Endianness specification
 */
typedef enum {
  LLACE_ENDIAN_LITTLE,
  LLACE_ENDIAN_BIG,
  LLACE_ENDIAN_AUTO         /* Use architecture default */
} llace_endian_t;

/**
 * Section types
 */
typedef enum {
  LLACE_SECTION_NULL,        /* Null section */
  LLACE_SECTION_TEXT,        /* Executable code */
  LLACE_SECTION_DATA,        /* Initialized data */
  LLACE_SECTION_BSS,         /* Uninitialized data */
  LLACE_SECTION_RODATA,      /* Read-only data */
  LLACE_SECTION_RELOC,       /* Relocation information */
  LLACE_SECTION_SYMTAB,      /* Symbol table */
  LLACE_SECTION_STRTAB,      /* String table */
  LLACE_SECTION_DEBUG,       /* Debug information */
  LLACE_SECTION_COMMENT,     /* Comment/version info */
  LLACE_SECTION_CUSTOM       /* Custom section type */
} llace_section_type_t;

/**
 * Section flags
 */
typedef enum {
  LLACE_SECTION_FLAG_NONE     = 0x00,
  LLACE_SECTION_FLAG_WRITE    = 0x01,  /* Writable */
  LLACE_SECTION_FLAG_ALLOC    = 0x02,  /* Allocated in memory */
  LLACE_SECTION_FLAG_EXEC     = 0x04,  /* Executable */
  LLACE_SECTION_FLAG_MERGE    = 0x08,  /* Mergeable */
  LLACE_SECTION_FLAG_STRINGS  = 0x10,  /* Contains strings */
  LLACE_SECTION_FLAG_INFO     = 0x20,  /* sh_info contains section index */
  LLACE_SECTION_FLAG_GROUP    = 0x40,  /* Member of section group */
  LLACE_SECTION_FLAG_ORDERED  = 0x80   /* Special ordering requirements */
} llace_section_flags_t;

/**
 * Symbol binding
 */
typedef enum {
  LLACE_SYMBOL_BIND_LOCAL,   /* Local symbol */
  LLACE_SYMBOL_BIND_GLOBAL,  /* Global symbol */
  LLACE_SYMBOL_BIND_WEAK,    /* Weak symbol */
  LLACE_SYMBOL_BIND_GNU_UNIQUE /* GNU unique symbol */
} llace_symbol_bind_t;

/**
 * Symbol type
 */
typedef enum {
  LLACE_SYMBOL_TYPE_NOTYPE,  /* No type specified */
  LLACE_SYMBOL_TYPE_OBJECT,  /* Data object */
  LLACE_SYMBOL_TYPE_FUNC,    /* Function */
  LLACE_SYMBOL_TYPE_SECTION, /* Section symbol */
  LLACE_SYMBOL_TYPE_FILE,    /* File symbol */
  LLACE_SYMBOL_TYPE_COMMON,  /* Common block */
  LLACE_SYMBOL_TYPE_TLS      /* Thread-local storage */
} llace_symbol_type_t;

/**
 * Symbol visibility
 */
typedef enum {
  LLACE_SYMBOL_VIS_DEFAULT,   /* Default visibility */
  LLACE_SYMBOL_VIS_INTERNAL,  /* Internal visibility */
  LLACE_SYMBOL_VIS_HIDDEN,    /* Hidden visibility */
  LLACE_SYMBOL_VIS_PROTECTED  /* Protected visibility */
} llace_symbol_vis_t;

/**
 * Relocation types (architecture-specific values will be mapped internally)
 */
typedef enum {
  LLACE_RELOC_NONE,
  LLACE_RELOC_8,             /* 8-bit absolute */
  LLACE_RELOC_16,            /* 16-bit absolute */
  LLACE_RELOC_32,            /* 32-bit absolute */
  LLACE_RELOC_64,            /* 64-bit absolute */
  LLACE_RELOC_PC8,           /* 8-bit PC-relative */
  LLACE_RELOC_PC16,          /* 16-bit PC-relative */
  LLACE_RELOC_PC32,          /* 32-bit PC-relative */
  LLACE_RELOC_PC64,          /* 64-bit PC-relative */
  LLACE_RELOC_GOT32,         /* 32-bit GOT entry */
  LLACE_RELOC_PLT32,         /* 32-bit PLT entry */
  LLACE_RELOC_GOTOFF32,      /* 32-bit GOT offset */
  LLACE_RELOC_GOTPC32,       /* 32-bit GOT PC-relative */
  LLACE_RELOC_TLS_GD,        /* TLS General Dynamic */
  LLACE_RELOC_TLS_IE,        /* TLS Initial Exec */
  LLACE_RELOC_TLS_LE,        /* TLS Local Exec */
  LLACE_RELOC_CUSTOM         /* Format-specific custom relocation */
} llace_reloc_type_t;

// ================================================ Handles ================================================ //

/**
 * Opaque object file context handle
 */
typedef struct llace_object {
  // TODO
} llace_object_t;

/**
 * Opaque section handle
 */
typedef struct llace_section {
  // TODO
} llace_section_t;

/**
 * Opaque symbol handle
 */
typedef struct llace_symbol {
  // TODO
} llace_symbol_t;

// ================================================ Context ================================================ //

/**
 * Destroy an object file context and free resources
 * 
 * @param ctx Context to destroy
 */
void llace_context_destroy(llace_object_t *ctx);

// ================================================ Section ================================================ //

/**
 * Create a new section
 * 
 * @param ctx           Context handle
 * @param name          Section name
 * @param type          Section type
 * @param flags         Section flags
 * @param alignment     Section alignment (power of 2)
 * @param section_out   Output section handle
 * @return Error code
 */
llace_error_t llace_section_create(
  llace_object_t *ctx,
  const char *name,
  llace_section_type_t type,
  llace_section_flags_t flags,
  uint32_t alignment,
  llace_section_t **section_out
);

/**
 * Get section by name
 * 
 * @param ctx           Context handle
 * @param name          Section name
 * @param section_out   Output section handle
 * @return Error code
 */
llace_error_t llace_section_get_by_name(
  llace_object_t *ctx,
  const char *name,
  llace_section_t **section_out
);

/**
 * Write data to a section
 * 
 * @param section   Section handle
 * @param data      Data to write
 * @param size      Size of data in bytes
 * @param offset    Offset within section (-1 for append)
 * @return Error code
 */
llace_error_t llace_section_write_data(
  llace_section_t *section,
  const void *data,
  size_t size,
  int64_t offset
);

/**
 * Reserve space in a section (for BSS-like sections)
 * 
 * @param section   Section handle
 * @param size      Size to reserve in bytes
 * @return Error code
 */
llace_error_t llace_section_reserve_space(
  llace_section_t *section,
  size_t size
);

/**
 * Get current size of a section
 * 
 * @param section   Section handle
 * @param size_out  Output size
 * @return Error code
 */
llace_error_t llace_section_get_size(
  llace_section_t *section,
  size_t *size_out
);

/**
 * Set section virtual address (for formats that support it)
 * 
 * @param section   Section handle
 * @param address   Virtual address
 * @return Error code
 */
llace_error_t llace_section_set_virtual_address(
  llace_section_t *section,
  uint64_t address
);

// ================================================ Symbol ================================================ //

/**
 * Add a symbol to the object file
 * 
 * @param ctx         Context handle
 * @param name        Symbol name
 * @param section     Section containing symbol (NULL for undefined)
 * @param offset      Offset within section
 * @param size        Symbol size (0 if unknown)
 * @param bind        Symbol binding
 * @param type        Symbol type
 * @param vis         Symbol visibility
 * @param symbol_out  Output symbol handle
 * @return Error code
 */
llace_error_t llace_symbol_add(
  llace_object_t *ctx,
  const char *name,
  llace_section_t *section,
  uint64_t offset,
  uint64_t size,
  llace_symbol_bind_t bind,
  llace_symbol_type_t type,
  llace_symbol_vis_t vis,
  llace_symbol_t **symbol_out
);

/**
 * Add an undefined external symbol
 * 
 * @param ctx         Context handle
 * @param name        Symbol name
 * @param symbol_out  Output symbol handle
 * @return Error code
 */
llace_error_t llace_symbol_add_external(
  llace_object_t *ctx,
  const char *name,
  llace_symbol_t **symbol_out
);

/**
 * Get symbol by name
 * 
 * @param ctx         Context handle
 * @param name        Symbol name
 * @param symbol_out  Output symbol handle
 * @return Error code
 */
llace_error_t llace_symbol_get_by_name(
  llace_object_t *ctx,
  const char *name,
  llace_symbol_t **symbol_out
);

// ================================================ Relocation ================================================ //

/**
 * Add a relocation entry
 * 
 * @param section   Section containing the relocation
 * @param offset    Offset within section to relocate
 * @param symbol    Symbol being referenced
 * @param type      Relocation type
 * @param addend    Addend value (for RELA-style relocations)
 * @return Error code
 */
llace_error_t llace_relocation_add(
  llace_section_t *section,
  uint64_t offset,
  llace_symbol_t *symbol,
  llace_reloc_type_t type,
  int64_t addend
);

/**
 * Add a relocation with custom type (format-specific)
 * 
 * @param section     Section containing the relocation
 * @param offset      Offset within section to relocate
 * @param symbol      Symbol being referenced
 * @param custom_type Format-specific relocation type
 * @param addend      Addend value
 * @return Error code
 */
llace_error_t llace_relocation_add_custom(
  llace_section_t *section,
  uint64_t offset,
  llace_symbol_t *symbol,
  uint32_t custom_type,
  int64_t addend
);

// ================================================ Debug ================================================ //

/**
 * Add source line mapping information
 * 
 * @param ctx       Context handle
 * @param filename  Source filename
 * @param line      Line number
 * @param column    Column number
 * @param address   Code address
 * @return Error code
 */
llace_error_t llace_debug_add_line_info(
  llace_object_t *ctx,
  const char *filename,
  uint32_t line,
  uint32_t column,
  uint64_t address
);

// ================================================ Output ================================================ //

/**
 * Finalize and write the object file
 * 
 * @param ctx Context handle
 * @return Error code
 */
llace_error_t llace_object_write(llace_module_t *_module);

// ================================================ Utility ================================================ //

/**
 * Check if format supports feature
 * 
 * @param format  Object file format
 * @param feature Feature name (e.g., "relocations", "debug", "entry_point")
 * @return true if supported, false otherwise
 */
bool llace_format_supports_feature(llace_format_t format, const char *feature);

#ifdef __cplusplus
}
#endif

#endif /* LLACE_OBJECT_WRITER_H */