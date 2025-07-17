#ifndef LLACE_IR_TYPE_H
#define LLACE_IR_TYPE_H

#include <llace/ir/type.h>

#ifdef __cplusplus
extern "C" {
#endif


// constructs the names in the type hash table for C types
// These are the currently constructed types. More will come as this IR develops.
//  bool, char, unsigned char, short, unsigned short,
//  int, unsigned int, long, unsigned long, long long, unsigned long long
//  int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t
//  float, double, long double, void
llace_error_t llace_builder_type_c(llace_builder_t *builder);


// Future expansions will exist for each language to give the types of each language
// llace_error_t llace_builder_type_cxx(llace_builder_t *builder);
// llace_error_t llace_builder_type_rust(llace_builder_t *builder);
// llace_error_t llace_builder_type_python(llace_builder_t *builder);

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_TYPE_H