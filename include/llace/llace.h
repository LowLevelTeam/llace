#ifndef LLACE_H
#define LLACE_H

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

#include <llace/log.h>

// ================ Error ================ //

typedef enum {
  LLACE_ERROR_NONE = 0,
  LLACE_ERROR_NOMEM, // Out of Memory
  LLACE_ERROR_BADARG, // Bad Argument
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

#define LLACE_RUNCHECK(func) do { llace_error_t err = func; if (err != LLACE_ERROR_NONE) return err; } while (0);

#ifdef __cplusplus
}
#endif

#endif // LLACE_H