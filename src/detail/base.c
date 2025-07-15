#include <stdio.h>
#include <stdlib.h>

typedef llace_error_t (*llace_handler_t)(llace_module_t*, llace_function_t*, llace_instr_t*);

[[noreturn]] llace_error_t llace_handler_notimpl(llace_module_t *module, llace_function_t *func, llace_instr_t *instr) {
  (void)module;
  (void)func;
  fprintf("Instruction '%s' Not Implemented Yet\n", llace_instr_opstr(instr->opcode));
  exit(1);
}

static llace_handler_t ophandlers[] = {
  llace_handler_notimpl, // LLACE_OP_ADD
  llace_handler_notimpl, // LLACE_OP_SUB
  llace_handler_notimpl, // LLACE_OP_MUL
  llace_handler_notimpl, // LLACE_OP_DIV
  llace_handler_notimpl, // LLACE_OP_MOD
  llace_handler_notimpl, // LLACE_OP_AND
  llace_handler_notimpl, // LLACE_OP_OR
  llace_handler_notimpl, // LLACE_OP_XOR
  llace_handler_notimpl, // LLACE_OP_SHL
  llace_handler_notimpl, // LLACE_OP_SHR
  llace_handler_notimpl, // LLACE_OP_LOAD
  llace_handler_notimpl, // LLACE_OP_STORE
  llace_handler_notimpl, // LLACE_OP_ALLOC
  llace_handler_notimpl, // LLACE_OP_DEALLOC
  llace_handler_notimpl, // LLACE_OP_JMP
  llace_handler_notimpl, // LLACE_OP_BR
  llace_handler_notimpl, // LLACE_OP_CALL
  llace_handler_notimpl // LLACE_OP_RET
};

static llace_error_t llace_codegen_global(llace_config_t *conf, llace_module_t *mod, llace_global_t *glob, FILE *f) {
  // generate data sections from globals
  return LLACE_ERROR_NONE;
}

static llace_error_t llace_codegen_function(llace_config_t *conf, llace_module_t *mod, llace_function_t *func, FILE *f) {
  // generate executable code (normally in .text sections except for special occasions in special object formats)

  return LLACE_ERROR_NONE;
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

