// include/llace/builder/c64.h
#ifndef LLACE_BUILDER_C64_H
#define LLACE_BUILDER_C64_H

// LLACE Builder for C64 (C on 64 bit systems)
// 64 bit is specified because C uses intrinsic types of unspecified sizes
// they are instead specified by operating systems

#include <llace/ir.h>

#ifdef __cplusplus
extern "C" {
#endif

// construct default types for C on 64-bit systems
// constructs following types:
// bool, char, unsigned char, short, unsigned short,
// int, unsigned int, long, unsigned long, long long, unsigned long long
// int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t
// float, double, long double, void
llace_error_t llace_builderc64_types(llace_module_t *module);

// get a default c type by name
// name follows the C standard naming convention
// e.g. "int", "unsigned long", "float", "double"
// no space before no space after
// returns NULL if type is not found
llace_type_t *llace_builderc64_gettype(llace_module_t *module, const char *name);

#ifdef __cplusplus
}
#endif

#endif // LLACE_BUILDER_C64_H