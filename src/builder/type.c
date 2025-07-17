#include <llace/builder/c64.h>
#include <llace/ir.h>
#include <llace/detail/common.h>
#include <string.h>

llace_error_t llace_builder_type_c(llace_builder_t *builder) {
  if (!module) return LLACE_ERROR_BADARG;

  llace_error_t err;

  if (llace_target_word_size(&module->conf->target) == 32) {
    // 32 bit compatible C integer intrinsics
  } else if (llace_target_word_size(&module->conf->target) == 64) {
    // 64 bit compatible C integer intrinsics
  } else {
    return LLACE_ERROR_BADARG;
  }
  
  // Fixed-width integer types
  // Floats
  // Void

  return LLACE_ERROR_NONE;
}
