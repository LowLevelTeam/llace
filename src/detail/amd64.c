#include <stdio.h>


static llace_error_t llace_codegen_value_const() {
  


  
}

static llace_error_t llace_codegen_value(llace_config_t *conf, llace_module_t *mod, llace_value_t *value, FILE *f) {
  switch (value->kind) {
    case LLACE_VALUE_VOID:
    case LLACE_VALUE_CONST:
    case LLACE_VALUE_VARIABLE:
    
    case LLACE_VALUE_GLOBAL:
    
    case LLACE_VALUE_FUNCTION:
    
    case LLACE_VALUE_INSTRUCTION:
    
    case LLACE_VALUE_BLOCK:

  }




}


static llace_error_t llace_codegen_global(llace_config_t *conf, llace_module_t *mod, llace_global_t *glob, FILE *f) {
  
}

static llace_error_t llace_codegen_function(llace_config_t *conf, llace_module_t *mod, llace_function_t *func, FILE *f) {
  



}

llace_error_t llace_codegen(llace_config_t *conf, llace_module_t *mod) {
  FILE *f = fopen(conf->filename, "wb");

  // generate globals
  size_t globcount = llace_mem_array_count(mod->globs);
  llace_global_t *globs = (llace_global_t*)llace_mem_get(mod->globs);
  for (size_t i = 0; i < globcount; ++i) {
    llace_codegen_global(conf, mod, globs[i], f);
  }

  // generate functions
  size_t funccount = llace_mem_array_count(mod->funcs);
  llace_function_t *funcs = (llace_function_t*)llace_mem_get(mod->funcs);
  for (size_t i = 0; i < funccount; ++i) {
    llace_codegen_function(conf, mod, globs[i], f);
  }

  return LLACE_ERROR_NONE;
}

