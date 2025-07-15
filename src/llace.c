#include <llace/llace.h>
#include <llace/mem.h>
#include <string.h>
#include <stdio.h>

// ================ Error Handling ================ //

const char *llace_error_str(llace_error_t error) {
  switch (error) {
    case LLACE_ERROR_NONE: return "No Error";
    case LLACE_ERROR_NOMEM: return "Out of Memory";
    case LLACE_ERROR_BADARG: return "Bad Argument";
    case LLACE_ERROR_INVLMOD: return "Invalid Module";
    case LLACE_ERROR_INVLFUNC: return "Invalid Function";
    case LLACE_ERROR_INVLTYPE: return "Invalid Type";
    case LLACE_ERROR_IO: return "IO Failed";
    case LLACE_ERROR_INVLFMT: return "Invalid Format";
    case LLACE_ERROR_INVLARCH: return "Invalid Architecture";
    case LLACE_ERROR_INVLSECT: return "Invalid Section";
    case LLACE_ERROR_INVLSYM: return "Invalid Symbol";
    case LLACE_ERROR_INVLREL: return "Invalid Relocation";
    case LLACE_ERROR_SECT404: return "Section Not Found";
    case LLACE_ERROR_SYM404: return "Sybol Not Found";
    case LLACE_ERROR_SYMDUP: return "Symbol Duplicate";
    case LLACE_ERROR_UNRESSYM: return "Unmresolved Symbol";
    case LLACE_ERROR_BADALLIGN: return "Bad Allignment";
    case LLACE_ERROR_OVERFLOW: return "Overflow";
    default: return "Unknown Error";
  }
}
