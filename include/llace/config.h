#ifndef LLACE_CONFIG_H
#define LLACE_CONFIG_H

#include "llace/llace.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct llace_config {
  // Compilation target
  llace_target_t target; // Target architecture

  // Parameters
  unsigned int debug : 1; // Enable debug mode (library debugging)

  // IO
  const char *output_filename; // Output file name
} llace_config_t;

#ifdef __cplusplus
}
#endif

#endif // LLACE_CONFIG_H