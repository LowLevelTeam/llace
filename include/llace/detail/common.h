#ifndef LLACE_COMMON_H
#define LLACE_COMMON_H

#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// TODO macro for unimplemented functionality
#define LLACE_TODO(msg) do { \
  fprintf(stderr, "TODO: '%s' not implemented at %s:%d\n", msg, __FILE__, __LINE__); \
  abort(); \
} while(0);
#define LLACE_TODOV(fmt, ...) do { \
  fprintf(stderr, "TODO: '"fmt"' not implemented at %s:%d\n", __VA_ARGS__, __FILE__, __LINE__); \
  abort(); \
} while(0);

#ifdef __cplusplus
}
#endif

#endif // LLACE_COMMON_H