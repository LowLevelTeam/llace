#include <llace/ir/value.h>
#include <llace/ir/cinstr.h>
#include <llace/ir/instr.h>
#include <llace/detail/common.h>
#include <llace/mem.h>
#include <string.h>

llace_error_t llace_value_init(llace_value_t *val) {
  if (!val) return LLACE_ERROR_BADARG;
  
  memset(val, 0, sizeof(llace_value_t));
  val->kind = LLACE_VALUE_VOID;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_destroy(llace_value_t *val) {
  if (!val) return LLACE_ERROR_BADARG;
  
  switch (val->kind) {
    case LLACE_VALUE_VOID:
    case LLACE_VALUE_VARIABLE:
    case LLACE_VALUE_GLOBAL:
    case LLACE_VALUE_FUNCTION:
      // These don't own any resources
      break;
      
    case LLACE_VALUE_CONST:
      // Free constant data handle
      if (val->_const.data != LLACE_HANDLE_INVALID) {
        llace_mem_free(val->_const.data);
      }
      break;
      
    case LLACE_VALUE_INSTRUCTION:
      // Destroy child instruction
      llace_cinstr_destroy(&val->_instr);
      break;
      
    case LLACE_VALUE_BLOCK:
      // Free instruction block
      if (val->_block != LLACE_HANDLE_INVALID) {
        llace_mem_free(val->_block);
      }
      break;
  }
  
  memset(val, 0, sizeof(llace_value_t));
  
  return LLACE_ERROR_NONE;
}

// ================ Value Builders ================ //

llace_error_t llace_value_void(llace_value_t *val) {
  if (!val) return LLACE_ERROR_BADARG;
  
  llace_value_destroy(val);
  val->kind = LLACE_VALUE_VOID;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_const(llace_value_t *val, llace_typeref_t type, const void *data, size_t data_size) {
  if (!val || !data || data_size == 0) return LLACE_ERROR_BADARG;
  
  llace_value_destroy(val);
  val->kind = LLACE_VALUE_CONST;
  val->_const.type = type;
  
  // Allocate memory handle and copy data
  llace_error_t err = llace_mem_alloc(data_size, &val->_const.data);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  void *dest = llace_mem_get(val->_const.data);
  memcpy(dest, data, data_size);
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_const_int(llace_value_t *val, llace_typeref_t type, int64_t value) {
  return llace_value_const(val, type, &value, sizeof(value));
}

llace_error_t llace_value_const_uint(llace_value_t *val, llace_typeref_t type, uint64_t value) {
  return llace_value_const(val, type, &value, sizeof(value));
}

llace_error_t llace_value_const_float(llace_value_t *val, llace_typeref_t type, double value) {
  return llace_value_const(val, type, &value, sizeof(value));
}

llace_error_t llace_value_const_ptr(llace_value_t *val, llace_typeref_t type, void *ptr) {
  return llace_value_const(val, type, &ptr, sizeof(ptr));
}

llace_error_t llace_value_variable(llace_value_t *val, llace_varref_t var_ref) {
  if (!val) return LLACE_ERROR_BADARG;
  
  llace_value_destroy(val);
  val->kind = LLACE_VALUE_VARIABLE;
  val->_var = var_ref;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_global(llace_value_t *val, llace_globalref_t global_ref) {
  if (!val) return LLACE_ERROR_BADARG;
  
  llace_value_destroy(val);
  val->kind = LLACE_VALUE_GLOBAL;
  val->_glob = global_ref;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_function(llace_value_t *val, llace_funcref_t func_ref) {
  if (!val) return LLACE_ERROR_BADARG;
  
  llace_value_destroy(val);
  val->kind = LLACE_VALUE_FUNCTION;
  val->_func = func_ref;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_instruction(llace_value_t *val, const llace_cinstr_t *instr) {
  if (!val || !instr) return LLACE_ERROR_BADARG;
  
  llace_value_destroy(val);
  val->kind = LLACE_VALUE_INSTRUCTION;
  
  // Initialize and copy instruction
  llace_cinstr_create(&val->_instr);
  val->_instr.opcode = instr->opcode;
  
  // Copy parameters if they exist
  if (instr->params != LLACE_HANDLE_INVALID) {
    size_t param_count = llace_mem_array_count(instr->params);
    if (param_count > 0) {
      llace_error_t err = LLACE_MEM_ALLOC_ARRAY(llace_value_t, param_count, &val->_instr.params);
      if (err != LLACE_ERROR_NONE) {
        return err;
      }
      
      llace_value_t *src_params = LLACE_MEM_GET_TYPED(llace_value_t, instr->params);
      llace_value_t *dst_params = LLACE_MEM_GET_TYPED(llace_value_t, val->_instr.params);
      
      for (size_t i = 0; i < param_count; i++) {
        llace_value_init(&dst_params[i]);
        // Simple copy - values are handled by reference
        dst_params[i] = src_params[i];
      }
    }
  }
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_block(llace_value_t *val, size_t instruction_count) {
  if (!val) return LLACE_ERROR_BADARG;
  
  llace_value_destroy(val);
  val->kind = LLACE_VALUE_BLOCK;
  
  if (instruction_count > 0) {
    llace_error_t err = LLACE_MEM_ALLOC_ARRAY(llace_instr_t, instruction_count, &val->_block);
    if (err != LLACE_ERROR_NONE) {
      return err;
    }
    
    // Initialize all instructions
    llace_instr_t *instrs = LLACE_MEM_GET_TYPED(llace_instr_t, val->_block);
    for (size_t i = 0; i < instruction_count; i++) {
      llace_instr_create(&instrs[i]);
    }
  } else {
    val->_block = LLACE_HANDLE_INVALID;
  }
  
  return LLACE_ERROR_NONE;
}

// ================ Value Accessors ================ //

llace_error_t llace_value_get_const_data(const llace_value_t *val, void **data, llace_typeref_t *type) {
  if (!val || !data || val->kind != LLACE_VALUE_CONST) {
    return LLACE_ERROR_BADARG;
  }
  
  *data = llace_mem_get(val->_const.data);
  if (type) {
    *type = val->_const.type;
  }
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_get_variable_ref(const llace_value_t *val, llace_varref_t *var_ref) {
  if (!val || !var_ref || val->kind != LLACE_VALUE_VARIABLE) {
    return LLACE_ERROR_BADARG;
  }
  
  *var_ref = val->_var;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_get_global_ref(const llace_value_t *val, llace_globalref_t *global_ref) {
  if (!val || !global_ref || val->kind != LLACE_VALUE_GLOBAL) {
    return LLACE_ERROR_BADARG;
  }
  
  *global_ref = val->_glob;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_get_function_ref(const llace_value_t *val, llace_funcref_t *func_ref) {
  if (!val || !func_ref || val->kind != LLACE_VALUE_FUNCTION) {
    return LLACE_ERROR_BADARG;
  }
  
  *func_ref = val->_func;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_get_instruction(const llace_value_t *val, llace_cinstr_t **instr) {
  if (!val || !instr || val->kind != LLACE_VALUE_INSTRUCTION) {
    return LLACE_ERROR_BADARG;
  }
  
  *instr = (llace_cinstr_t *)&val->_instr;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_value_get_block(const llace_value_t *val, llace_handle_t *block) {
  if (!val || !block || val->kind != LLACE_VALUE_BLOCK) {
    return LLACE_ERROR_BADARG;
  }
  
  *block = val->_block;
  return LLACE_ERROR_NONE;
}