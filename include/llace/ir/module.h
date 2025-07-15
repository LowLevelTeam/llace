#ifndef LLACE_IR_MODULE_H
#define LLACE_IR_MODULE_H

#include <llace/llace.h>
#include <llace/mem.h> // llace_handle_t

#ifdef __cplusplus
extern "C" {
#endif

// ================================================ IR ================================================ //

// Module structure
typedef struct llace_module {
  llace_config_t *conf;
  llace_nameref_t name; // Module name (typically filename)
  llace_handle_t nametab; // const char *
  llace_handle_t types; // llace_type_t
  llace_handle_t globs; // llace_global_t
  llace_handle_t funcs; // llace_function_t
} llace_module_t;

/** 
* @param module empty llace_module_t structure, if NULL fail with BADARG
* @param name character pointer, if NULL no name is added, name can be added later manually
* @param namelen length of character pointer, if 0 strlen is used internally
*
* @return LLACE_ERROR_NONE
* @return LLACE_ERROR_NOMEM
* @return LLACE_ERROR_BADARG
*/
llace_error_t llace_module_init(llace_module_t *module, const char *name, size_t namelen);

/** 
* @param module initalized llace_module_t structure, if NULL fail with BADARG
*
* @return LLACE_ERROR_NONE
* @return LLACE_ERROR_BADARG
*
* Loops through and destroys all items then frees all handles
*/
llace_error_t llace_module_destroy(llace_module_t *module);

// ================================================ Builder ================================================ //

/** 
* @param module initalized llace_module_t structure, if NULL fail with BADARG
* @param name character pointer, if NULL fail with BADARG
* @param ref pointer to uninitalized sizeof(llace_nameref_t) space
*
* @return LLACE_ERROR_NONE
* @return LLACE_ERROR_NOMEM
* @return LLACE_ERROR_BADARG
*/
llace_error_t llace_module_add_name(llace_module_t *module, const char *name, llace_nameref_t *ref);

/** 
* @param module initalized llace_module_t structure, if NULL fail with BADARG
* @param type initalized llace_type_t structure, if NULL fail with BADARG
* @param ref pointer to uninitalized sizeof(llace_typeref_t) space
*
* @return LLACE_ERROR_NONE
* @return LLACE_ERROR_NOMEM
* @return LLACE_ERROR_BADARG
*/
llace_error_t llace_module_add_type(llace_module_t *module, const llace_type_t *type, llace_typeref_t *ref);

/** 
* @param module initalized llace_module_t structure, if NULL fail with BADARG
* @param global initalized llace_global_t structure, if NULL fail with BADARG
* @param ref pointer to uninitalized sizeof(llace_globalref_t) space
*
* @return LLACE_ERROR_NONE
* @return LLACE_ERROR_NOMEM
* @return LLACE_ERROR_BADARG
*/
llace_error_t llace_module_add_global(llace_module_t *module, const llace_global_t *global, llace_globalref_t *ref);

/** 
* @param module initalized llace_module_t structure, if NULL fail with BADARG
* @param function initalized llace_function_t structure, if NULL fail with BADARG
* @param ref pointer to uninitalized sizeof(llace_funcref_t) space
*
* @return LLACE_ERROR_NONE
* @return LLACE_ERROR_NOMEM
* @return LLACE_ERROR_BADARG
*/
llace_error_t llace_module_add_function(llace_module_t *module, const llace_function_t *function, llace_funcref_t *ref);

/** 
* @param module initalized llace_module_t structure, if NULL fail with BADARG
* @param ref valid llace_nameref_t
*
* @return valid character pointer to c string if LLACE_ERROR_NONE 
* @return NULL pointer if LLACE_ERROR_BADARG
*/
char *llace_module_get_name(llace_module_t *module, llace_nameref_t ref);

/** 
* @param module initalized llace_module_t structure, if NULL fail with BADARG
* @param ref valid llace_typeref_t
*
* @return valid llace_type_t pointer if LLACE_ERROR_NONE
* @return NULL pointer if LLACE_ERROR_BADARG
*/
llace_type_t *llace_module_get_type(llace_module_t *module, llace_typeref_t ref);

/** 
* @param module initalized llace_module_t structure, if NULL fail with BADARG
* @param ref valid llace_globalref_t
*
* @return valid llace_global_t pointer if LLACE_ERROR_NONE
* @return NULL pointer if LLACE_ERROR_BADARG
*/
llace_global_t *llace_module_get_global(llace_module_t *module, llace_globalref_t ref);

/** 
* @param module initalized llace_module_t structure, if NULL fail with BADARG
* @param ref valid llace_funcref_t
*
* @return valid llace_function_t pointer if LLACE_ERROR_NONE
* @return NULL pointer if LLACE_ERROR_BADARG
*/
llace_function_t *llace_module_get_function(llace_module_t *module, llace_funcref_t ref);

#ifdef __cplusplus
}
#endif

#endif // LLACE_IR_MODULE_H