#include <llace/ir/func.h>
#include <llace/ir/var.h>
#include <llace/ir/instr.h>
#include <string.h>

// ================ Function Implementation ================ //

llace_error_t llace_function_init(llace_function_t *func) {
  if (!func) return LLACE_ERROR_BADARG;
  
  memset(func, 0, sizeof(llace_function_t));
  func->name = 0;
  func->abi = LLACE_ABI_CDECL;
  func->rets = LLACE_HANDLE_INVALID;
  func->params = LLACE_HANDLE_INVALID;
  func->locals = LLACE_HANDLE_INVALID;
  func->instrs = LLACE_HANDLE_INVALID;
  func->attrval = 0;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_function_destroy(llace_function_t *func) {
  if (!func) return LLACE_ERROR_BADARG;
  
  // Free returns
  if (func->rets != LLACE_HANDLE_INVALID) {
    llace_variable_t *rets = LLACE_MEM_GET_TYPED(llace_variable_t, func->rets);
    size_t count = llace_mem_array_count(func->rets);
    for (size_t i = 0; i < count; i++) {
      llace_variable_destroy(&rets[i]);
    }
    llace_mem_free(func->rets);
  }
  
  // Free parameters
  if (func->params != LLACE_HANDLE_INVALID) {
    llace_variable_t *params = LLACE_MEM_GET_TYPED(llace_variable_t, func->params);
    size_t count = llace_mem_array_count(func->params);
    for (size_t i = 0; i < count; i++) {
      llace_variable_destroy(&params[i]);
    }
    llace_mem_free(func->params);
  }
  
  // Free locals
  if (func->locals != LLACE_HANDLE_INVALID) {
    llace_variable_t *locals = LLACE_MEM_GET_TYPED(llace_variable_t, func->locals);
    size_t count = llace_mem_array_count(func->locals);
    for (size_t i = 0; i < count; i++) {
      llace_variable_destroy(&locals[i]);
    }
    llace_mem_free(func->locals);
  }
  
  // Free instructions
  if (func->instrs != LLACE_HANDLE_INVALID) {
    llace_instr_t *instrs = LLACE_MEM_GET_TYPED(llace_instr_t, func->instrs);
    size_t count = llace_mem_array_count(func->instrs);
    for (size_t i = 0; i < count; i++) {
      llace_instr_destroy(&instrs[i]);
    }
    llace_mem_free(func->instrs);
  }
  
  memset(func, 0, sizeof(llace_function_t));
  
  return LLACE_ERROR_NONE;
}

// ================ Function Builders ================ //

llace_error_t llace_function_set_name(llace_function_t *func, llace_nameref_t name) {
  if (!func) return LLACE_ERROR_BADARG;
  
  func->name = name;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_function_set_abi(llace_function_t *func, llace_abi_t abi) {
  if (!func) return LLACE_ERROR_BADARG;
  
  func->abi = abi;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_function_set_extern(llace_function_t *func, bool is_extern) {
  if (!func) return LLACE_ERROR_BADARG;
  
  func->attr._extern = is_extern ? 1 : 0;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_function_set_public(llace_function_t *func, bool is_public) {
  if (!func) return LLACE_ERROR_BADARG;
  
  func->attr._public = is_public ? 1 : 0;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_function_alloc_returns(llace_function_t *func, size_t count) {
  if (!func) return LLACE_ERROR_BADARG;
  
  // Free existing returns
  if (func->rets != LLACE_HANDLE_INVALID) {
    llace_variable_t *rets = LLACE_MEM_GET_TYPED(llace_variable_t, func->rets);
    size_t old_count = llace_mem_array_count(func->rets);
    for (size_t i = 0; i < old_count; i++) {
      llace_variable_destroy(&rets[i]);
    }
    llace_mem_free(func->rets);
  }
  
  if (count == 0) {
    func->rets = LLACE_HANDLE_INVALID;
    return LLACE_ERROR_NONE;
  }
  
  // Allocate new returns
  llace_error_t err = LLACE_MEM_ALLOC_ARRAY(llace_variable_t, count, &func->rets);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Initialize all returns
  llace_variable_t *rets = LLACE_MEM_GET_TYPED(llace_variable_t, func->rets);
  for (size_t i = 0; i < count; i++) {
    llace_variable_init(&rets[i]);
  }
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_function_alloc_params(llace_function_t *func, size_t count) {
  if (!func) return LLACE_ERROR_BADARG;
  
  // Free existing params
  if (func->params != LLACE_HANDLE_INVALID) {
    llace_variable_t *params = LLACE_MEM_GET_TYPED(llace_variable_t, func->params);
    size_t old_count = llace_mem_array_count(func->params);
    for (size_t i = 0; i < old_count; i++) {
      llace_variable_destroy(&params[i]);
    }
    llace_mem_free(func->params);
  }
  
  if (count == 0) {
    func->params = LLACE_HANDLE_INVALID;
    return LLACE_ERROR_NONE;
  }
  
  // Allocate new params
  llace_error_t err = LLACE_MEM_ALLOC_ARRAY(llace_variable_t, count, &func->params);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Initialize all params
  llace_variable_t *params = LLACE_MEM_GET_TYPED(llace_variable_t, func->params);
  for (size_t i = 0; i < count; i++) {
    llace_variable_init(&params[i]);
  }
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_function_alloc_locals(llace_function_t *func, size_t count) {
  if (!func) return LLACE_ERROR_BADARG;
  
  // Free existing locals
  if (func->locals != LLACE_HANDLE_INVALID) {
    llace_variable_t *locals = LLACE_MEM_GET_TYPED(llace_variable_t, func->locals);
    size_t old_count = llace_mem_array_count(func->locals);
    for (size_t i = 0; i < old_count; i++) {
      llace_variable_destroy(&locals[i]);
    }
    llace_mem_free(func->locals);
  }
  
  if (count == 0) {
    func->locals = LLACE_HANDLE_INVALID;
    return LLACE_ERROR_NONE;
  }
  
  // Allocate new locals
  llace_error_t err = LLACE_MEM_ALLOC_ARRAY(llace_variable_t, count, &func->locals);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Initialize all locals
  llace_variable_t *locals = LLACE_MEM_GET_TYPED(llace_variable_t, func->locals);
  for (size_t i = 0; i < count; i++) {
    llace_variable_init(&locals[i]);
  }
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_function_alloc_instrs(llace_function_t *func, size_t count) {
  if (!func) return LLACE_ERROR_BADARG;
  
  // Free existing instructions
  if (func->instrs != LLACE_HANDLE_INVALID) {
    llace_instr_t *instrs = LLACE_MEM_GET_TYPED(llace_instr_t, func->instrs);
    size_t old_count = llace_mem_array_count(func->instrs);
    for (size_t i = 0; i < old_count; i++) {
      llace_instr_destroy(&instrs[i]);
    }
    llace_mem_free(func->instrs);
  }
  
  if (count == 0) {
    func->instrs = LLACE_HANDLE_INVALID;
    return LLACE_ERROR_NONE;
  }
  
  // Allocate new instructions
  llace_error_t err = LLACE_MEM_ALLOC_ARRAY(llace_instr_t, count, &func->instrs);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Initialize all instructions
  llace_instr_t *instrs = LLACE_MEM_GET_TYPED(llace_instr_t, func->instrs);
  for (size_t i = 0; i < count; i++) {
    llace_instr_create(&instrs[i]);
  }
  
  return LLACE_ERROR_NONE;
}

// ================ Function Accessors ================ //

llace_nameref_t llace_function_get_name(const llace_function_t *func) {
  if (!func) return 0;
  return func->name;
}

llace_abi_t llace_function_get_abi(const llace_function_t *func) {
  if (!func) return LLACE_ABI_CDECL;
  return func->abi;
}

llace_variable_t *llace_function_get_return(const llace_function_t *func, size_t index) {
  if (!func || func->rets == LLACE_HANDLE_INVALID) return NULL;
  
  size_t count = llace_mem_array_count(func->rets);
  if (index >= count) return NULL;
  
  llace_variable_t *rets = LLACE_MEM_GET_TYPED(llace_variable_t, func->rets);
  return &rets[index];
}

llace_variable_t *llace_function_get_param(const llace_function_t *func, size_t index) {
  if (!func || func->params == LLACE_HANDLE_INVALID) return NULL;
  
  size_t count = llace_mem_array_count(func->params);
  if (index >= count) return NULL;
  
  llace_variable_t *params = LLACE_MEM_GET_TYPED(llace_variable_t, func->params);
  return &params[index];
}

llace_variable_t *llace_function_get_local(const llace_function_t *func, size_t index) {
  if (!func || func->locals == LLACE_HANDLE_INVALID) return NULL;
  
  size_t count = llace_mem_array_count(func->locals);
  if (index >= count) return NULL;
  
  llace_variable_t *locals = LLACE_MEM_GET_TYPED(llace_variable_t, func->locals);
  return &locals[index];
}

llace_instr_t *llace_function_get_instr(const llace_function_t *func, size_t index) {
  if (!func || func->instrs == LLACE_HANDLE_INVALID) return NULL;
  
  size_t count = llace_mem_array_count(func->instrs);
  if (index >= count) return NULL;
  
  llace_instr_t *instrs = LLACE_MEM_GET_TYPED(llace_instr_t, func->instrs);
  return &instrs[index];
}

size_t llace_function_return_count(const llace_function_t *func) {
  if (!func || func->rets == LLACE_HANDLE_INVALID) return 0;
  return llace_mem_array_count(func->rets);
}

size_t llace_function_param_count(const llace_function_t *func) {
  if (!func || func->params == LLACE_HANDLE_INVALID) return 0;
  return llace_mem_array_count(func->params);
}

size_t llace_function_local_count(const llace_function_t *func) {
  if (!func || func->locals == LLACE_HANDLE_INVALID) return 0;
  return llace_mem_array_count(func->locals);
}

size_t llace_function_instr_count(const llace_function_t *func) {
  if (!func || func->instrs == LLACE_HANDLE_INVALID) return 0;
  return llace_mem_array_count(func->instrs);
}

bool llace_function_is_extern(const llace_function_t *func) {
  if (!func) return false;
  return func->attr._extern == 1;
}

bool llace_function_is_public(const llace_function_t *func) {
  if (!func) return false;
  return func->attr._public == 1;
}