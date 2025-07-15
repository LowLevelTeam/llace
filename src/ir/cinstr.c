#include <llace/ir/cinstr.h>
#include <llace/detail/common.h>
#include <llace/mem.h>
#include <string.h>

#include <llace/ir/value.h>

// ================ Child Instruction Implementation ================ //

void llace_cinstr_create(llace_cinstr_t *instr) {
  if (!instr) return;
  
  memset(instr, 0, sizeof(llace_cinstr_t));
  instr->opcode = LLACE_COP_ADD; // Default to safe opcode
  instr->params = LLACE_HANDLE_INVALID;
}

void llace_cinstr_destroy(llace_cinstr_t *instr) {
  if (!instr) return;
  
  // Free parameters
  if (instr->params != LLACE_HANDLE_INVALID) {
    llace_value_t *params = LLACE_MEM_GET_TYPED(llace_value_t, instr->params);
    size_t count = llace_mem_array_count(instr->params);
    for (size_t i = 0; i < count; i++) {
      llace_value_destroy(&params[i]);
    }
    llace_mem_free(instr->params);
  }
  
  memset(instr, 0, sizeof(llace_cinstr_t));
}

const char *llace_cinstr_opstr(llace_copcode_t op) {
  switch (op) {
    case LLACE_COP_ADD: return "add";
    case LLACE_COP_SUB: return "sub";
    case LLACE_COP_MUL: return "mul";
    case LLACE_COP_DIV: return "div";
    case LLACE_COP_MOD: return "mod";
    case LLACE_COP_AND: return "and";
    case LLACE_COP_OR: return "or";
    case LLACE_COP_XOR: return "xor";
    case LLACE_COP_NOT: return "not";
    case LLACE_COP_SHL: return "shl";
    case LLACE_COP_SHR: return "shr";
    case LLACE_COP_EQ: return "eq";
    case LLACE_COP_NE: return "ne";
    case LLACE_COP_LT: return "lt";
    case LLACE_COP_LE: return "le";
    case LLACE_COP_GT: return "gt";
    case LLACE_COP_GE: return "ge";
    case LLACE_COP_CALL: return "call";
    default: return "unknown";
  }
}

// ================ Child Instruction Builders ================ //

llace_error_t llace_cinstr_set_opcode(llace_cinstr_t *instr, llace_copcode_t opcode) {
  if (!instr) return LLACE_ERROR_BADARG;
  
  instr->opcode = opcode;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_cinstr_alloc_params(llace_cinstr_t *instr, size_t count) {
  if (!instr) return LLACE_ERROR_BADARG;
  
  // Free existing params
  if (instr->params != LLACE_HANDLE_INVALID) {
    llace_value_t *params = LLACE_MEM_GET_TYPED(llace_value_t, instr->params);
    size_t old_count = llace_mem_array_count(instr->params);
    for (size_t i = 0; i < old_count; i++) {
      llace_value_destroy(&params[i]);
    }
    llace_mem_free(instr->params);
  }
  
  if (count == 0) {
    instr->params = LLACE_HANDLE_INVALID;
    return LLACE_ERROR_NONE;
  }
  
  // Allocate new params
  llace_error_t err = LLACE_MEM_ALLOC_ARRAY(llace_value_t, count, &instr->params);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Initialize all params
  llace_value_t *params = LLACE_MEM_GET_TYPED(llace_value_t, instr->params);
  for (size_t i = 0; i < count; i++) {
    llace_value_init(&params[i]);
  }
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_cinstr_set_param(llace_cinstr_t *instr, size_t index, const llace_value_t *value) {
  if (!instr || !value || instr->params == LLACE_HANDLE_INVALID) {
    return LLACE_ERROR_BADARG;
  }
  
  size_t count = llace_mem_array_count(instr->params);
  if (index >= count) return LLACE_ERROR_BADARG;
  
  llace_value_t *params = LLACE_MEM_GET_TYPED(llace_value_t, instr->params);
  
  // Destroy existing value
  llace_value_destroy(&params[index]);
  
  // Copy new value
  params[index] = *value;
  
  return LLACE_ERROR_NONE;
}

// ================ Child Instruction Accessors ================ //

llace_copcode_t llace_cinstr_get_opcode(const llace_cinstr_t *instr) {
  if (!instr) return LLACE_COP_ADD;
  return instr->opcode;
}

size_t llace_cinstr_param_count(const llace_cinstr_t *instr) {
  if (!instr || instr->params == LLACE_HANDLE_INVALID) return 0;
  return llace_mem_array_count(instr->params);
}

llace_value_t *llace_cinstr_get_param(const llace_cinstr_t *instr, size_t index) {
  if (!instr || instr->params == LLACE_HANDLE_INVALID) return NULL;
  
  size_t count = llace_mem_array_count(instr->params);
  if (index >= count) return NULL;
  
  llace_value_t *params = LLACE_MEM_GET_TYPED(llace_value_t, instr->params);
  return &params[index];
}

llace_handle_t llace_cinstr_get_params(const llace_cinstr_t *instr) {
  if (!instr) return LLACE_HANDLE_INVALID;
  return instr->params;
}