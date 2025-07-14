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
*   - UNKN: This TODO is for features that are just not implemented right now
*/

#ifdef __cplusplus
extern "C" {
#endif

// ================ Error ================ //

typedef enum {
  LLACE_ERROR_NONE = 0,
  LLACE_ERROR_NOMEM, // Out of Memory
  LLACE_ERROR_INVLMOD, // Invalid Module
  LLACE_ERROR_INVLFUNC, // Invalid Function
  LLACE_ERROR_INVLTYPE, // Invalid Type
  LLACE_ERROR_IO, // IO Failed
  LLACE_ERROR_INVLFMT, // Invalid Format
  LLACE_ERROR_INVLARCH, // Invalid Architecture
  LLACE_ERROR_INVLSECT, // Invalid Section
  LLACE_ERROR_INVLSYM, // Invalid Symbol
  LLACE_ERROR_INVLREL, // Invalid Relocation
  LLACE_ERROR_SECT404, // Section Not Found
  LLACE_ERROR_SYM404, // Sybol Not Found
  LLACE_ERROR_SYMDUP, // Symbol Duplicate
  LLACE_ERROR_UNRESSYM, // Unmresolved Symbol
  LLACE_ERROR_BADALLIGN, // Bad Allignment
  LLACE_ERROR_OVERFLOW // Overflow
} llace_error_t;

const char *llace_error_str(llace_error_t error);

// ================ Memory Management ================ //

/**
 * Initialize the LLACE library including memory management.
 * This must be called before using any other LLACE functions.
 * 
 * @param initial_memory_size Initial size for the memory pool (0 for default)
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_init(size_t initial_memory_size);

/**
 * Cleanup and shutdown the LLACE library.
 * This will free all allocated memory and invalidate all handles.
 */
void llace_cleanup(void);

/**
 * Check if the LLACE library has been initialized.
 * 
 * @return true if initialized, false otherwise
 */
bool llace_is_initialized(void);

// ================ Library Information ================ //

typedef struct {
  size_t memory_pool_size;
  size_t memory_used;
  size_t active_allocations;
  size_t compactions_performed;
  const char *version;
  const char *build_date;
  const char *build_config;
} llace_info_t;

/**
 * Get information about the LLACE library instance.
 * 
 * @param info Output parameter for library information
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_get_info(llace_info_t *info);

/**
 * Print library information to stdout.
 */
void llace_print_info(void);

#ifdef __cplusplus
}
#endif

#endif // LLACE_H