#include <llace/ir/instr.h>
#include <llace/ir/value.h>
#include <string.h>

// ================ Instruction Implementation ================ //

void llace_instr_create(llace_instr_t *instr) {
  if (!instr) return;
  
  memset(instr, 0, sizeof(llace_instr_t));
  instr->opcode = LLACE_OP_ADD; // Default to safe opcode
  instr->params = LLACE_HANDLE_INVALID;
}

void llace_instr_destroy(llace_instr_t *instr) {
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
  
  memset(instr, 0, sizeof(llace_instr_t));
}

const char *llace_instr_opstr(llace_opcode_t op) {
  switch (op) {
    case LLACE_OP_ADD: return "add";
    case LLACE_OP_SUB: return "sub";
    case LLACE_OP_MUL: return "mul";
    case LLACE_OP_DIV: return "div";
    case LLACE_OP_MOD: return "mod";
    case LLACE_OP_AND: return "and";
    case LLACE_OP_OR: return "or";
    case LLACE_OP_XOR: return "xor";
    case LLACE_OP_SHL: return "shl";
    case LLACE_OP_SHR: return "shr";
    case LLACE_OP_LOAD: return "load";
    case LLACE_OP_STORE: return "store";
    case LLACE_OP_ALLOC: return "alloc";
    case LLACE_OP_DEALLOC: return "dealloc";
    case LLACE_OP_JMP: return "jmp";
    case LLACE_OP_BR: return "br";
    case LLACE_OP_CALL: return "call";
    case LLACE_OP_RET: return "ret";
    default: return "unknown";
  }
}

// ================ Instruction Builders ================ //

llace_error_t llace_instr_set_opcode(llace_instr_t *instr, llace_opcode_t opcode) {
  if (!instr) return LLACE_ERROR_BADARG;
  
  instr->opcode = opcode;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_instr_alloc_params(llace_instr_t *instr, size_t count) {
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

llace_error_t llace_instr_set_param(llace_instr_t *instr, size_t index, const llace_value_t *value) {
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

// ================ Instruction Accessors ================ //

llace_opcode_t llace_instr_get_opcode(const llace_instr_t *instr) {
  if (!instr) return LLACE_OP_ADD;
  return instr->opcode;
}

size_t llace_instr_param_count(const llace_instr_t *instr) {
  if (!instr || instr->params == LLACE_HANDLE_INVALID) return 0;
  return llace_mem_array_count(instr->params);
}

llace_value_t *llace_instr_get_param(const llace_instr_t *instr, size_t index) {
  if (!instr || instr->params == LLACE_HANDLE_INVALID) return NULL;
  
  size_t count = llace_mem_array_count(instr->params);
  if (index >= count) return NULL;
  
  llace_value_t *params = LLACE_MEM_GET_TYPED(llace_value_t, instr->params);
  return &params[index];
}

llace_handle_t llace_instr_get_params(const llace_instr_t *instr) {
  if (!instr) return LLACE_HANDLE_INVALID;
  return instr->params;
}