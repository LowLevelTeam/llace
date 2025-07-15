#include <llace/iir.h>
#include <llace/detail/common.h>
#include <llace/mem.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// ================ Internal Constants ================ //

#define LLACE_IIR_DEFAULT_STACK_SIZE 1024
#define LLACE_IIR_DEFAULT_HEAP_SIZE (64 * 1024)
#define LLACE_IIR_MAX_CALL_DEPTH 256

// ================ Internal Helper Functions ================ //

static llace_error_t ensure_globals_initialized(llace_iir_context_t *ctx) {
  if (ctx->globals == LLACE_HANDLE_INVALID) {
    // Get global count from module
    size_t global_count = llace_mem_array_count(ctx->module->globs);
    
    llace_error_t err = LLACE_MEM_ALLOC_ARRAY(llace_iir_rtval_t, global_count, &ctx->globals);
    if (err != LLACE_ERROR_NONE) {
      return err;
    }
    
    // Initialize globals to undefined
    llace_iir_rtval_t *globals = LLACE_MEM_GET_TYPED(llace_iir_rtval_t, ctx->globals);
    for (size_t i = 0; i < global_count; i++) {
      llace_iir_rtval_undef(&globals[i], 0); // Type will be set when accessed
    }
  }
  return LLACE_ERROR_NONE;
}

static llace_error_t push_frame(llace_iir_context_t *ctx, llace_funcref_t func_ref) {
  if (ctx->frame_pointer >= LLACE_IIR_MAX_CALL_DEPTH) {
    strcpy(ctx->error_message, "Call stack overflow");
    return LLACE_ERROR_OVERFLOW;
  }
  
  // Get call stack
  llace_iir_frame_t *frames = LLACE_MEM_GET_TYPED(llace_iir_frame_t, ctx->call_stack);
  
  // Get function
  llace_function_t *func = llace_module_get_function(ctx->module, func_ref);
  if (!func) {
    strcpy(ctx->error_message, "Invalid function reference");
    return LLACE_ERROR_INVLFUNC;
  }
  
  // Initialize new frame
  llace_iir_frame_t *frame = &frames[ctx->frame_pointer++];
  frame->function = func_ref;
  frame->instruction_index = 0;
  frame->stack_pointer = 0;
  
  // Allocate locals for function
  size_t local_count = llace_mem_array_count(func->locals);
  if (local_count > 0) {
    llace_error_t err = LLACE_MEM_ALLOC_ARRAY(llace_iir_rtval_t, local_count, &frame->locals);
    if (err != LLACE_ERROR_NONE) {
      ctx->frame_pointer--;
      return err;
    }
    
    // Initialize locals to undefined
    llace_iir_rtval_t *locals = LLACE_MEM_GET_TYPED(llace_iir_rtval_t, frame->locals);
    for (size_t i = 0; i < local_count; i++) {
      llace_iir_rtval_undef(&locals[i], 0);
    }
  } else {
    frame->locals = LLACE_HANDLE_INVALID;
  }
  
  // Allocate stack for frame
  llace_error_t err = LLACE_MEM_ALLOC_ARRAY(llace_iir_rtval_t, LLACE_IIR_DEFAULT_STACK_SIZE, &frame->stack);
  if (err != LLACE_ERROR_NONE) {
    if (frame->locals != LLACE_HANDLE_INVALID) {
      llace_mem_free(frame->locals);
    }
    ctx->frame_pointer--;
    return err;
  }
  
  ctx->function_calls++;
  return LLACE_ERROR_NONE;
}

static llace_error_t pop_frame(llace_iir_context_t *ctx) {
  if (ctx->frame_pointer == 0) {
    return LLACE_ERROR_BADARG;
  }
  
  llace_iir_frame_t *frames = LLACE_MEM_GET_TYPED(llace_iir_frame_t, ctx->call_stack);
  llace_iir_frame_t *frame = &frames[--ctx->frame_pointer];
  
  // Free frame resources
  if (frame->locals != LLACE_HANDLE_INVALID) {
    llace_mem_free(frame->locals);
  }
  if (frame->stack != LLACE_HANDLE_INVALID) {
    llace_mem_free(frame->stack);
  }
  
  return LLACE_ERROR_NONE;
}

static llace_error_t execute_instruction(llace_iir_context_t *ctx, const llace_instr_t *instr) {
  llace_iir_frame_t *frame = llace_iir_get_current_frame(ctx);
  if (!frame) {
    return LLACE_ERROR_BADARG;
  }
  
  llace_iir_rtval_t *stack = LLACE_MEM_GET_TYPED(llace_iir_rtval_t, frame->stack);
  llace_value_t *params = LLACE_MEM_GET_TYPED(llace_value_t, instr->params);
  size_t param_count = llace_mem_array_count(instr->params);
  
  switch (instr->opcode) {
    case LLACE_OP_ADD: {
      if (param_count < 2) return LLACE_ERROR_BADARG;
      
      // Get operands (simplified - should handle different value types)
      llace_iir_rtval_t a = stack[frame->stack_pointer - 2];
      llace_iir_rtval_t b = stack[frame->stack_pointer - 1];
      
      // Perform addition based on types
      if (a.kind == LLACE_IIR_RTVAL_INT && b.kind == LLACE_IIR_RTVAL_INT) {
        llace_iir_rtval_int(&stack[frame->stack_pointer - 2], a.type, a._int + b._int);
      } else if (a.kind == LLACE_IIR_RTVAL_FLOAT && b.kind == LLACE_IIR_RTVAL_FLOAT) {
        llace_iir_rtval_float(&stack[frame->stack_pointer - 2], a.type, a._float + b._float);
      } else {
        strcpy(ctx->error_message, "Type mismatch in ADD operation");
        return LLACE_ERROR_INVLTYPE;
      }
      
      frame->stack_pointer--;
      break;
    }
    
    case LLACE_OP_SUB: {
      if (param_count < 2) return LLACE_ERROR_BADARG;
      
      llace_iir_rtval_t a = stack[frame->stack_pointer - 2];
      llace_iir_rtval_t b = stack[frame->stack_pointer - 1];
      
      if (a.kind == LLACE_IIR_RTVAL_INT && b.kind == LLACE_IIR_RTVAL_INT) {
        llace_iir_rtval_int(&stack[frame->stack_pointer - 2], a.type, a._int - b._int);
      } else if (a.kind == LLACE_IIR_RTVAL_FLOAT && b.kind == LLACE_IIR_RTVAL_FLOAT) {
        llace_iir_rtval_float(&stack[frame->stack_pointer - 2], a.type, a._float - b._float);
      } else {
        strcpy(ctx->error_message, "Type mismatch in SUB operation");
        return LLACE_ERROR_INVLTYPE;
      }
      
      frame->stack_pointer--;
      break;
    }
    
    case LLACE_OP_MUL: {
      if (param_count < 2) return LLACE_ERROR_BADARG;
      
      llace_iir_rtval_t a = stack[frame->stack_pointer - 2];
      llace_iir_rtval_t b = stack[frame->stack_pointer - 1];
      
      if (a.kind == LLACE_IIR_RTVAL_INT && b.kind == LLACE_IIR_RTVAL_INT) {
        llace_iir_rtval_int(&stack[frame->stack_pointer - 2], a.type, a._int * b._int);
      } else if (a.kind == LLACE_IIR_RTVAL_FLOAT && b.kind == LLACE_IIR_RTVAL_FLOAT) {
        llace_iir_rtval_float(&stack[frame->stack_pointer - 2], a.type, a._float * b._float);
      } else {
        strcpy(ctx->error_message, "Type mismatch in MUL operation");
        return LLACE_ERROR_INVLTYPE;
      }
      
      frame->stack_pointer--;
      break;
    }
    
    case LLACE_OP_DIV: {
      if (param_count < 2) return LLACE_ERROR_BADARG;
      
      llace_iir_rtval_t a = stack[frame->stack_pointer - 2];
      llace_iir_rtval_t b = stack[frame->stack_pointer - 1];
      
      if (a.kind == LLACE_IIR_RTVAL_INT && b.kind == LLACE_IIR_RTVAL_INT) {
        if (b._int == 0) {
          strcpy(ctx->error_message, "Division by zero");
          return LLACE_ERROR_BADARG;
        }
        llace_iir_rtval_int(&stack[frame->stack_pointer - 2], a.type, a._int / b._int);
      } else if (a.kind == LLACE_IIR_RTVAL_FLOAT && b.kind == LLACE_IIR_RTVAL_FLOAT) {
        if (b._float == 0.0) {
          strcpy(ctx->error_message, "Division by zero");
          return LLACE_ERROR_BADARG;
        }
        llace_iir_rtval_float(&stack[frame->stack_pointer - 2], a.type, a._float / b._float);
      } else {
        strcpy(ctx->error_message, "Type mismatch in DIV operation");
        return LLACE_ERROR_INVLTYPE;
      }
      
      frame->stack_pointer--;
      break;
    }
    
    case LLACE_OP_LOAD: {
      // TODO: Implement memory load
      LLACE_TODO("LOAD instruction not implemented");
      break;
    }
    
    case LLACE_OP_STORE: {
      // TODO: Implement memory store
      LLACE_TODO("STORE instruction not implemented");
      break;
    }
    
    case LLACE_OP_CALL: {
      // TODO: Implement function calls
      LLACE_TODO("CALL instruction not implemented");
      break;
    }
    
    case LLACE_OP_RET: {
      // Return from function
      if (param_count > 0) {
        // TODO: Handle return value
      }
      return pop_frame(ctx);
    }
    
    case LLACE_OP_JMP: {
      // TODO: Implement unconditional jump
      LLACE_TODO("JMP instruction not implemented");
      break;
    }
    
    case LLACE_OP_BR: {
      // TODO: Implement conditional branch
      LLACE_TODO("BR instruction not implemented");
      break;
    }
    
    default:
      snprintf(ctx->error_message, sizeof(ctx->error_message), 
               "Unknown instruction opcode: %d", instr->opcode);
      return LLACE_ERROR_BADARG;
  }
  
  ctx->instruction_count++;
  return LLACE_ERROR_NONE;
}

static bool is_breakpoint(llace_iir_context_t *ctx, size_t instruction_index) {
  if (ctx->breakpoints == LLACE_HANDLE_INVALID) {
    return false;
  }
  
  size_t *breakpoints = LLACE_MEM_GET_TYPED(size_t, ctx->breakpoints);
  size_t bp_count = llace_mem_array_count(ctx->breakpoints);
  
  for (size_t i = 0; i < bp_count; i++) {
    if (breakpoints[i] == instruction_index) {
      return true;
    }
  }
  
  return false;
}

// ================ Public API Implementation ================ //

llace_error_t llace_iir_init(llace_iir_context_t *ctx, llace_module_t *module, llace_config_t *config) {
  if (!ctx || !module) {
    return LLACE_ERROR_BADARG;
  }
  
  memset(ctx, 0, sizeof(llace_iir_context_t));
  
  ctx->module = module;
  ctx->config = config;
  ctx->state = LLACE_IIR_STATE_READY;
  ctx->globals = LLACE_HANDLE_INVALID;
  ctx->heap = LLACE_HANDLE_INVALID;
  ctx->breakpoints = LLACE_HANDLE_INVALID;
  ctx->debug_enabled = false;
  ctx->trace_enabled = false;
  
  // Allocate call stack
  llace_error_t err = LLACE_MEM_ALLOC_ARRAY(llace_iir_frame_t, LLACE_IIR_MAX_CALL_DEPTH, &ctx->call_stack);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Initialize heap
  err = llace_mem_alloc(LLACE_IIR_DEFAULT_HEAP_SIZE, &ctx->heap);
  if (err != LLACE_ERROR_NONE) {
    llace_mem_free(ctx->call_stack);
    return err;
  }
  
  return LLACE_ERROR_NONE;
}

void llace_iir_destroy(llace_iir_context_t *ctx) {
  if (!ctx) return;
  
  // Free all frames
  while (ctx->frame_pointer > 0) {
    pop_frame(ctx);
  }
  
  // Free resources
  if (ctx->call_stack != LLACE_HANDLE_INVALID) {
    llace_mem_free(ctx->call_stack);
  }
  if (ctx->globals != LLACE_HANDLE_INVALID) {
    llace_mem_free(ctx->globals);
  }
  if (ctx->heap != LLACE_HANDLE_INVALID) {
    llace_mem_free(ctx->heap);
  }
  if (ctx->breakpoints != LLACE_HANDLE_INVALID) {
    llace_mem_free(ctx->breakpoints);
  }
  
  memset(ctx, 0, sizeof(llace_iir_context_t));
}

llace_error_t llace_iir_execute_function(llace_iir_context_t *ctx, llace_funcref_t func_ref,
                                        const llace_iir_rtval_t *args, size_t arg_count,
                                        llace_iir_rtval_t *result) {
  if (!ctx || ctx->state != LLACE_IIR_STATE_READY) {
    return LLACE_ERROR_BADARG;
  }
  
  // Ensure globals are initialized
  llace_error_t err = ensure_globals_initialized(ctx);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Push initial frame
  err = push_frame(ctx, func_ref);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // TODO: Set up function arguments
  // For now, just run the function
  
  ctx->state = LLACE_IIR_STATE_RUNNING;
  err = llace_iir_run(ctx);
  
  if (result) {
    llace_iir_rtval_void(result);
  }
  
  return err;
}

llace_error_t llace_iir_run(llace_iir_context_t *ctx) {
  if (!ctx || ctx->state != LLACE_IIR_STATE_RUNNING) {
    return LLACE_ERROR_BADARG;
  }
  
  while (ctx->state == LLACE_IIR_STATE_RUNNING && ctx->frame_pointer > 0) {
    llace_iir_frame_t *frame = llace_iir_get_current_frame(ctx);
    if (!frame) break;
    
    llace_function_t *func = llace_module_get_function(ctx->module, frame->function);
    if (!func) {
      ctx->state = LLACE_IIR_STATE_ERROR;
      strcpy(ctx->error_message, "Invalid function in frame");
      return LLACE_ERROR_INVLFUNC;
    }
    
    // Check for breakpoint
    if (ctx->debug_enabled && is_breakpoint(ctx, frame->instruction_index)) {
      ctx->state = LLACE_IIR_STATE_SUSPENDED;
      break;
    }
    
    // Get instructions
    llace_instr_t *instrs = LLACE_MEM_GET_TYPED(llace_instr_t, func->instrs);
    size_t instr_count = llace_mem_array_count(func->instrs);
    
    if (frame->instruction_index >= instr_count) {
      // Function completed
      pop_frame(ctx);
      continue;
    }
    
    llace_instr_t *instr = &instrs[frame->instruction_index];
    
    // Trace if enabled
    if (ctx->trace_enabled) {
      printf("IIR: [%zu:%zu] %s\n", frame->function, frame->instruction_index, 
             llace_instr_opstr(instr->opcode));
    }
    
    // Execute instruction
    llace_error_t err = execute_instruction(ctx, instr);
    if (err != LLACE_ERROR_NONE) {
      ctx->state = LLACE_IIR_STATE_ERROR;
      ctx->last_error = err;
      return err;
    }
    
    frame->instruction_index++;
  }
  
  if (ctx->frame_pointer == 0) {
    ctx->state = LLACE_IIR_STATE_COMPLETED;
  }
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_iir_step(llace_iir_context_t *ctx, llace_iir_step_mode_t mode) {
  if (!ctx || (ctx->state != LLACE_IIR_STATE_RUNNING && ctx->state != LLACE_IIR_STATE_SUSPENDED)) {
    return LLACE_ERROR_BADARG;
  }
  
  ctx->state = LLACE_IIR_STATE_RUNNING;
  
  switch (mode) {
    case LLACE_IIR_STEP_INSTRUCTION: {
      // Execute one instruction
      llace_iir_frame_t *frame = llace_iir_get_current_frame(ctx);
      if (!frame) return LLACE_ERROR_BADARG;
      
      llace_function_t *func = llace_module_get_function(ctx->module, frame->function);
      if (!func) return LLACE_ERROR_INVLFUNC;
      
      llace_instr_t *instrs = LLACE_MEM_GET_TYPED(llace_instr_t, func->instrs);
      size_t instr_count = llace_mem_array_count(func->instrs);
      
      if (frame->instruction_index < instr_count) {
        llace_error_t err = execute_instruction(ctx, &instrs[frame->instruction_index]);
        if (err != LLACE_ERROR_NONE) {
          ctx->state = LLACE_IIR_STATE_ERROR;
          return err;
        }
        frame->instruction_index++;
      }
      
      ctx->state = LLACE_IIR_STATE_SUSPENDED;
      break;
    }
    
    default:
      // TODO: Implement other step modes
      LLACE_TODO("Step modes other than INSTRUCTION not implemented");
      break;
  }
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_iir_suspend(llace_iir_context_t *ctx) {
  if (!ctx || ctx->state != LLACE_IIR_STATE_RUNNING) {
    return LLACE_ERROR_BADARG;
  }
  
  ctx->state = LLACE_IIR_STATE_SUSPENDED;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_iir_resume(llace_iir_context_t *ctx) {
  if (!ctx || ctx->state != LLACE_IIR_STATE_SUSPENDED) {
    return LLACE_ERROR_BADARG;
  }
  
  ctx->state = LLACE_IIR_STATE_RUNNING;
  return llace_iir_run(ctx);
}

llace_error_t llace_iir_reset(llace_iir_context_t *ctx) {
  if (!ctx) {
    return LLACE_ERROR_BADARG;
  }
  
  // Free all frames
  while (ctx->frame_pointer > 0) {
    pop_frame(ctx);
  }
  
  ctx->state = LLACE_IIR_STATE_READY;
  ctx->instruction_count = 0;
  ctx->function_calls = 0;
  ctx->memory_allocations = 0;
  ctx->last_error = LLACE_ERROR_NONE;
  memset(ctx->error_message, 0, sizeof(ctx->error_message));
  
  return LLACE_ERROR_NONE;
}

// ================ Runtime Value Functions ================ //

void llace_iir_rtval_void(llace_iir_rtval_t *val) {
  if (!val) return;
  memset(val, 0, sizeof(llace_iir_rtval_t));
  val->kind = LLACE_IIR_RTVAL_VOID;
}

void llace_iir_rtval_int(llace_iir_rtval_t *val, llace_typeref_t type, int64_t value) {
  if (!val) return;
  memset(val, 0, sizeof(llace_iir_rtval_t));
  val->kind = LLACE_IIR_RTVAL_INT;
  val->type = type;
  val->_int = value;
}

void llace_iir_rtval_unt(llace_iir_rtval_t *val, llace_typeref_t type, uint64_t value) {
  if (!val) return;
  memset(val, 0, sizeof(llace_iir_rtval_t));
  val->kind = LLACE_IIR_RTVAL_UNT;
  val->type = type;
  val->_unt = value;
}

void llace_iir_rtval_float(llace_iir_rtval_t *val, llace_typeref_t type, double value) {
  if (!val) return;
  memset(val, 0, sizeof(llace_iir_rtval_t));
  val->kind = LLACE_IIR_RTVAL_FLOAT;
  val->type = type;
  val->_float = value;
}

void llace_iir_rtval_ptr(llace_iir_rtval_t *val, llace_typeref_t type, void *ptr, size_t offset) {
  if (!val) return;
  memset(val, 0, sizeof(llace_iir_rtval_t));
  val->kind = LLACE_IIR_RTVAL_PTR;
  val->type = type;
  val->_ptr.ptr = ptr;
  val->_ptr.offset = offset;
}

void llace_iir_rtval_undef(llace_iir_rtval_t *val, llace_typeref_t type) {
  if (!val) return;
  memset(val, 0, sizeof(llace_iir_rtval_t));
  val->kind = LLACE_IIR_RTVAL_UNDEF;
  val->type = type;
}

llace_error_t llace_iir_rtval_from_const(llace_iir_context_t *ctx, const llace_value_t *ir_val, llace_iir_rtval_t *rt_val) {
  if (!ctx || !ir_val || !rt_val) {
    return LLACE_ERROR_BADARG;
  }
  
  if (ir_val->kind != LLACE_VALUE_CONST) {
    return LLACE_ERROR_INVLTYPE;
  }
  
  llace_type_t *type = llace_module_get_type(ctx->module, ir_val->_const.type);
  if (!type) {
    return LLACE_ERROR_INVLTYPE;
  }
  
  switch (type->kind) {
    case LLACE_TYPE_VOID:
      llace_iir_rtval_void(rt_val);
      break;
      
    case LLACE_TYPE_INT: {
      int64_t value = 0;
      memcpy(&value, ir_val->_const.data, type->size);
      llace_iir_rtval_int(rt_val, ir_val->_const.type, value);
      break;
    }
    
    case LLACE_TYPE_UNT: {
      uint64_t value = 0;
      memcpy(&value, ir_val->_const.data, type->size);
      llace_iir_rtval_unt(rt_val, ir_val->_const.type, value);
      break;
    }
    
    case LLACE_TYPE_FLOAT: {
      double value = 0.0;
      if (type->size == 4) {
        float f;
        memcpy(&f, ir_val->_const.data, 4);
        value = (double)f;
      } else {
        memcpy(&value, ir_val->_const.data, type->size);
      }
      llace_iir_rtval_float(rt_val, ir_val->_const.type, value);
      break;
    }
    
    case LLACE_TYPE_PTR: {
      void *ptr;
      memcpy(&ptr, ir_val->_const.data, sizeof(void*));
      llace_iir_rtval_ptr(rt_val, ir_val->_const.type, ptr, 0);
      break;
    }
    
    default:
      return LLACE_ERROR_INVLTYPE;
  }
  
  return LLACE_ERROR_NONE;
}

size_t llace_iir_rtval_to_string(const llace_iir_rtval_t *val, char *buffer, size_t buffer_size) {
  if (!val || !buffer || buffer_size == 0) {
    return 0;
  }
  
  switch (val->kind) {
    case LLACE_IIR_RTVAL_VOID:
      return snprintf(buffer, buffer_size, "void");
    case LLACE_IIR_RTVAL_INT:
      return snprintf(buffer, buffer_size, "%lld", (long long)val->_int);
    case LLACE_IIR_RTVAL_UNT:
      return snprintf(buffer, buffer_size, "%llu", (unsigned long long)val->_unt);
    case LLACE_IIR_RTVAL_FLOAT:
      return snprintf(buffer, buffer_size, "%f", val->_float);
    case LLACE_IIR_RTVAL_PTR:
      return snprintf(buffer, buffer_size, "%p+%zu", val->_ptr.ptr, val->_ptr.offset);
    case LLACE_IIR_RTVAL_UNDEF:
      return snprintf(buffer, buffer_size, "undef");
    default:
      return snprintf(buffer, buffer_size, "unknown");
  }
}

// ================ Debugging Functions ================ //

void llace_iir_set_debug(llace_iir_context_t *ctx, bool enable) {
  if (ctx) {
    ctx->debug_enabled = enable;
  }
}

void llace_iir_set_trace(llace_iir_context_t *ctx, bool enable) {
  if (ctx) {
    ctx->trace_enabled = enable;
  }
}

llace_error_t llace_iir_add_breakpoint(llace_iir_context_t *ctx, size_t instruction_index) {
  if (!ctx) {
    return LLACE_ERROR_BADARG;
  }
  
  if (ctx->breakpoints == LLACE_HANDLE_INVALID) {
    // Create breakpoint array
    llace_error_t err = LLACE_MEM_ALLOC_ARRAY(size_t, 1, &ctx->breakpoints);
    if (err != LLACE_ERROR_NONE) {
      return err;
    }
    
    size_t *breakpoints = LLACE_MEM_GET_TYPED(size_t, ctx->breakpoints);
    breakpoints[0] = instruction_index;
  } else {
    // Check if breakpoint already exists
    size_t *breakpoints = LLACE_MEM_GET_TYPED(size_t, ctx->breakpoints);
    size_t bp_count = llace_mem_array_count(ctx->breakpoints);
    
    for (size_t i = 0; i < bp_count; i++) {
      if (breakpoints[i] == instruction_index) {
        return LLACE_ERROR_NONE; // Already exists
      }
    }
    
    // Add new breakpoint
    llace_error_t err = llace_mem_realloc_array(ctx->breakpoints, bp_count + 1);
    if (err != LLACE_ERROR_NONE) {
      return err;
    }
    
    breakpoints = LLACE_MEM_GET_TYPED(size_t, ctx->breakpoints);
    breakpoints[bp_count] = instruction_index;
  }
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_iir_remove_breakpoint(llace_iir_context_t *ctx, size_t instruction_index) {
  if (!ctx || ctx->breakpoints == LLACE_HANDLE_INVALID) {
    return LLACE_ERROR_BADARG;
  }
  
  size_t *breakpoints = LLACE_MEM_GET_TYPED(size_t, ctx->breakpoints);
  size_t bp_count = llace_mem_array_count(ctx->breakpoints);
  
  // Find and remove breakpoint
  for (size_t i = 0; i < bp_count; i++) {
    if (breakpoints[i] == instruction_index) {
      // Move remaining breakpoints down
      for (size_t j = i; j < bp_count - 1; j++) {
        breakpoints[j] = breakpoints[j + 1];
      }
      
      // Resize array
      if (bp_count > 1) {
        llace_mem_realloc_array(ctx->breakpoints, bp_count - 1);
      } else {
        // No more breakpoints
        llace_mem_free(ctx->breakpoints);
        ctx->breakpoints = LLACE_HANDLE_INVALID;
      }
      
      return LLACE_ERROR_NONE;
    }
  }
  
  return LLACE_ERROR_BADARG; // Not found
}

void llace_iir_clear_breakpoints(llace_iir_context_t *ctx) {
  if (ctx && ctx->breakpoints != LLACE_HANDLE_INVALID) {
    llace_mem_free(ctx->breakpoints);
    ctx->breakpoints = LLACE_HANDLE_INVALID;
  }
}

llace_iir_frame_t *llace_iir_get_current_frame(llace_iir_context_t *ctx) {
  if (!ctx || ctx->frame_pointer == 0) {
    return NULL;
  }
  
  llace_iir_frame_t *frames = LLACE_MEM_GET_TYPED(llace_iir_frame_t, ctx->call_stack);
  return &frames[ctx->frame_pointer - 1];
}

size_t llace_iir_get_call_depth(llace_iir_context_t *ctx) {
  return ctx ? ctx->frame_pointer : 0;
}

llace_iir_frame_t *llace_iir_get_frame(llace_iir_context_t *ctx, size_t depth) {
  if (!ctx || depth >= ctx->frame_pointer) {
    return NULL;
  }
  
  llace_iir_frame_t *frames = LLACE_MEM_GET_TYPED(llace_iir_frame_t, ctx->call_stack);
  return &frames[ctx->frame_pointer - 1 - depth];
}

void llace_iir_print_context(llace_iir_context_t *ctx) {
  if (!ctx) {
    printf("IIR Context: NULL\n");
    return;
  }
  
  printf("IIR Context:\n");
  printf("  State: %s\n", llace_iir_state_str(ctx->state));
  printf("  Call Depth: %zu\n", ctx->frame_pointer);
  printf("  Instructions: %zu\n", ctx->instruction_count);
  printf("  Function Calls: %zu\n", ctx->function_calls);
  printf("  Debug: %s\n", ctx->debug_enabled ? "enabled" : "disabled");
  printf("  Trace: %s\n", ctx->trace_enabled ? "enabled" : "disabled");
  
  if (ctx->last_error != LLACE_ERROR_NONE) {
    printf("  Last Error: %s - %s\n", llace_error_str(ctx->last_error), ctx->error_message);
  }
}

void llace_iir_print_call_stack(llace_iir_context_t *ctx) {
  if (!ctx) {
    printf("Call Stack: NULL context\n");
    return;
  }
  
  printf("Call Stack (%zu frames):\n", ctx->frame_pointer);
  
  for (size_t i = 0; i < ctx->frame_pointer; i++) {
    llace_iir_frame_t *frame = llace_iir_get_frame(ctx, i);
    if (frame) {
      llace_function_t *func = llace_module_get_function(ctx->module, frame->function);
      char *func_name = func ? llace_module_get_name(ctx->module, func->name) : "unknown";
      
      printf("  [%zu] %s @ instruction %zu\n", i, func_name, frame->instruction_index);
    }
  }
}

// ================ Statistics Functions ================ //

llace_error_t llace_iir_get_stats(llace_iir_context_t *ctx, llace_iir_stats_t *stats) {
  if (!ctx || !stats) {
    return LLACE_ERROR_BADARG;
  }
  
  stats->instruction_count = ctx->instruction_count;
  stats->function_calls = ctx->function_calls;
  stats->memory_allocations = ctx->memory_allocations;
  stats->call_stack_depth = ctx->frame_pointer;
  stats->max_call_stack_depth = ctx->frame_pointer; // TODO: Track max depth
  
  return LLACE_ERROR_NONE;
}

void llace_iir_reset_stats(llace_iir_context_t *ctx) {
  if (ctx) {
    ctx->instruction_count = 0;
    ctx->function_calls = 0;
    ctx->memory_allocations = 0;
  }
}

void llace_iir_print_stats(llace_iir_context_t *ctx) {
  if (!ctx) {
    printf("IIR Stats: NULL context\n");
    return;
  }
  
  printf("IIR Statistics:\n");
  printf("  Instructions Executed: %zu\n", ctx->instruction_count);
  printf("  Function Calls: %zu\n", ctx->function_calls);
  printf("  Memory Allocations: %zu\n", ctx->memory_allocations);
  printf("  Current Call Depth: %zu\n", ctx->frame_pointer);
}

// ================ Utility Functions ================ //

bool llace_iir_is_constant(llace_iir_context_t *ctx, const llace_value_t *val) {
  if (!ctx || !val) {
    return false;
  }
  
  return val->kind == LLACE_VALUE_CONST;
}

llace_error_t llace_iir_eval_const(llace_iir_context_t *ctx, const llace_value_t *expr, llace_iir_rtval_t *result) {
  if (!ctx || !expr || !result) {
    return LLACE_ERROR_BADARG;
  }
  
  if (expr->kind == LLACE_VALUE_CONST) {
    return llace_iir_rtval_from_const(ctx, expr, result);
  }
  
  // TODO: Implement constant expression evaluation
  LLACE_TODO("Constant expression evaluation not implemented");
  return LLACE_ERROR_BADARG;
}

const char *llace_iir_state_str(llace_iir_state_t state) {
  switch (state) {
    case LLACE_IIR_STATE_READY: return "ready";
    case LLACE_IIR_STATE_RUNNING: return "running";
    case LLACE_IIR_STATE_SUSPENDED: return "suspended";
    case LLACE_IIR_STATE_COMPLETED: return "completed";
    case LLACE_IIR_STATE_ERROR: return "error";
    default: return "unknown";
  }
}