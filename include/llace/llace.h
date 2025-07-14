#ifndef LLACE_H
#define LLACE_H

#include "llace/detail/common.h"

/**
* detail/ contains implementation details used only by source files. 
* It is not intended to be included by users of the LLACE library.
*
* The root include folders contains the public API of LLACE.
*
* TODO Structure:
*   Todos are used to track the progress of the implementation.
*   each todo has a suffix to give an indication at what timepoint it will be implemented.
*   The suffixes are:
*   - FAR: This TODO is for a future implementation probably in the next major release.
*   - BUG: This TODO is for a bug fix that needs to be addressed.
*   - CLOSE: This TODO is for a close to completion feature that will be implemented in the next minor release.
*/


#ifdef __cplusplus
extern "C" {
#endif

// ================ Version ================ //

#define LLACE_VERSION_MAJOR 0
#define LLACE_VERSION_MINOR 1
#define LLACE_VERSION_PATCH 0

const char *llace_version_str(void);

// ================ Target ================ //

typedef enum {
  LLACE_TARGET_AMD64_GNU_ELF64, // GNU/Linux x86_64 ELF64 (GNU compatible ELF format)
} llace_target_t;

bool llace_target_is_supported(llace_target_t target);
const char *llace_target_str(llace_target_t target);

// ================ Error ================ //

typedef enum {
  LLACE_ERROR_NONE = 0,
  LLACE_ERROR_NOMEM,
  LLACE_ERROR_INVLMOD,
  LLACE_ERROR_INVLFUNC,
  LLACE_ERROR_INVLTYPE,
  LLACE_ERROR_IO,
  LLACE_ERROR_BADTARGET,
} llace_error_t;

const char *llace_error_str(llace_error_t error);

#ifdef __cplusplus
}
#endif

#endif // LLACE_H