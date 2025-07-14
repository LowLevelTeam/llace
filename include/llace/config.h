#ifndef LLACE_CONFIG_H
#define LLACE_CONFIG_H

#include "llace/llace.h"

#ifdef __cplusplus
extern "C" {
#endif

// ================ Target Machine Code ================ //

typedef enum {
  LLACE_TARGET_AMD64, // x86_64 | x86-64
} llace_target_mc_t;

bool llace_targetmc_is_supported(llace_target_mc_t target);
const char *llace_targetmc_str(llace_target_mc_t target);

// ================ Target Object ================ //

typedef enum {
  LLACE_TARGET_ELF64,
} llace_target_obj_t;

bool llace_targetobj_is_supported(llace_target_obj_t target);
const char *llace_targetobj_str(llace_target_obj_t target);

// ================ Config ================ //

typedef struct llace_config {
  // Output Generation
  llace_target_mc_t mc; // Target Machine Code
  llace_target_obj_t obj; // Target Object
  const char *filename; // Output file name

  // Flags
  unsigned int debug : 1; // Enable library debug mode
} llace_config_t;

#ifdef __cplusplus
}
#endif

#endif // LLACE_CONFIG_H