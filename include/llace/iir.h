#ifndef LLACE_IIR_H
#define LLACE_IIR_H

#include <llace/llace.h>
#include <llace/ir.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================ IIR (Interpreted Intermediate Representation) ================ //

/**
 * LLACE IIR - Interpreted execution of LLACE IR
 * 
 * This system provides interpreted execution of LLACE IR for:
 * - Compile-time constant evaluation
 * - Debugging and inspection
 * - Constant folding optimizations
 * - Testing IR correctness
 */

// ================ Interpreter State ================ //

typedef enum {
  LLACE_IIR_STATE_READY,        // Ready to execute
  LLACE_IIR_STATE_RUNNING,      // Currently executing
  LLACE_IIR_STATE_SUSPENDED,    // Suspended (breakpoint/debug)
  LLACE_IIR_STATE_COMPLETED,    // Execution completed
  LLACE_IIR_STATE_ERROR         // Error occurred
} llace_iir_state_t;

typedef enum {
  LLACE_IIR_STEP_INSTRUCTION,   // Step one instruction
  LLACE_IIR_STEP_LINE,          // Step one source line (if debug info available)
  LLACE_IIR_STEP_FUNCTION,      // Step into function
  LLACE_IIR_STEP_OVER,          // Step over function call
  LLACE_IIR_STEP_OUT            // Step out of current function
} llace_iir_step_mode_t;

// ================ Runtime Value System ================ //

typedef enum {
  LLACE_IIR_RTVAL_VOID,         // No value
  LLACE_IIR_RTVAL_INT,          // Integer value
  LLACE_IIR_RTVAL_UNT,          // Unsigned integer value
  LLACE_IIR_RTVAL_FLOAT,        // Floating point value
  LLACE_IIR_RTVAL_PTR,          // Pointer value
  LLACE_IIR_RTVAL_UNDEF         // Undefined value
} llace_iir_rtval_kind_t;

typedef struct {
  llace_iir_rtval_kind_t kind;
  llace_typeref_t type;         // Reference to type in module
  
  union {
    int64_t _int;               // Signed integer value
    uint64_t _unt;              // Unsigned integer value
    double _float;              // Floating point value
    struct {
      void *ptr;                // Pointer value
      size_t offset;            // Offset for pointer arithmetic
    } _ptr;
  };
} llace_iir_rtval_t;

// ================ Execution Context ================ //

typedef struct {
  llace_funcref_t function;     // Current function
  size_t instruction_index;     // Current instruction index
  llace_handle_t locals;        // llace_iir_rtval_t[] - Local variables
  llace_handle_t stack;         // llace_iir_rtval_t[] - Execution stack
  size_t stack_pointer;         // Current stack pointer
} llace_iir_frame_t;

typedef struct {
  llace_module_t *module;       // Module being executed
  llace_config_t *config;       // Configuration
  
  // Execution state
  llace_iir_state_t state;      // Current execution state
  llace_handle_t call_stack;    // llace_iir_frame_t[] - Call stack
  size_t frame_pointer;         // Current frame pointer
  
  // Global state
  llace_handle_t globals;       // llace_iir_rtval_t[] - Global variables
  llace_handle_t heap;          // Simulated heap memory
  
  // Debug state
  llace_handle_t breakpoints;   // size_t[] - Breakpoint instruction indices
  bool debug_enabled;           // Debug mode enabled
  bool trace_enabled;           // Instruction tracing enabled
  
  // Statistics
  size_t instruction_count;     // Total instructions executed
  size_t function_calls;        // Total function calls made
  size_t memory_allocations;    // Total memory allocations
  
  // Error handling
  llace_error_t last_error;     // Last error that occurred
  char error_message[256];      // Error message buffer
} llace_iir_context_t;

// ================ Interpreter Functions ================ //

/**
 * Initialize IIR interpreter context
 * @param ctx Uninitialized context
 * @param module Module to execute
 * @param config Configuration (optional)
 * @return LLACE_ERROR_NONE on success
 */
llace_error_t llace_iir_init(llace_iir_context_t *ctx, llace_module_t *module, llace_config_t *config);

/**
 * Destroy IIR interpreter context
 * @param ctx Context to destroy
 */
void llace_iir_destroy(llace_iir_context_t *ctx);

/**
 * Execute a function with given arguments
 * @param ctx Interpreter context
 * @param func_ref Function to execute
 * @param args Array of argument values
 * @param arg_count Number of arguments
 * @param result Output parameter for result (optional)
 * @return LLACE_ERROR_NONE on success
 */
llace_error_t llace_iir_execute_function(llace_iir_context_t *ctx, llace_funcref_t func_ref, 
                                        const llace_iir_rtval_t *args, size_t arg_count,
                                        llace_iir_rtval_t *result);

/**
 * Execute until completion or breakpoint
 * @param ctx Interpreter context
 * @return LLACE_ERROR_NONE on success
 */
llace_error_t llace_iir_run(llace_iir_context_t *ctx);

/**
 * Execute one step
 * @param ctx Interpreter context
 * @param mode Step mode
 * @return LLACE_ERROR_NONE on success
 */
llace_error_t llace_iir_step(llace_iir_context_t *ctx, llace_iir_step_mode_t mode);

/**
 * Suspend execution
 * @param ctx Interpreter context
 * @return LLACE_ERROR_NONE on success
 */
llace_error_t llace_iir_suspend(llace_iir_context_t *ctx);

/**
 * Resume execution
 * @param ctx Interpreter context
 * @return LLACE_ERROR_NONE on success
 */
llace_error_t llace_iir_resume(llace_iir_context_t *ctx);

/**
 * Reset interpreter state
 * @param ctx Interpreter context
 * @return LLACE_ERROR_NONE on success
 */
llace_error_t llace_iir_reset(llace_iir_context_t *ctx);

// ================ Runtime Value Functions ================ //

/**
 * Create void runtime value
 * @param val Output value
 */
void llace_iir_rtval_void(llace_iir_rtval_t *val);

/**
 * Create integer runtime value
 * @param val Output value
 * @param type Type reference
 * @param value Integer value
 */
void llace_iir_rtval_int(llace_iir_rtval_t *val, llace_typeref_t type, int64_t value);

/**
 * Create unsigned integer runtime value
 * @param val Output value
 * @param type Type reference
 * @param value Unsigned integer value
 */
void llace_iir_rtval_unt(llace_iir_rtval_t *val, llace_typeref_t type, uint64_t value);

/**
 * Create floating point runtime value
 * @param val Output value
 * @param type Type reference
 * @param value Floating point value
 */
void llace_iir_rtval_float(llace_iir_rtval_t *val, llace_typeref_t type, double value);

/**
 * Create pointer runtime value
 * @param val Output value
 * @param type Type reference
 * @param ptr Pointer value
 * @param offset Offset value
 */
void llace_iir_rtval_ptr(llace_iir_rtval_t *val, llace_typeref_t type, void *ptr, size_t offset);

/**
 * Create undefined runtime value
 * @param val Output value
 * @param type Type reference
 */
void llace_iir_rtval_undef(llace_iir_rtval_t *val, llace_typeref_t type);

/**
 * Convert constant IR value to runtime value
 * @param ctx Interpreter context
 * @param ir_val IR value to convert
 * @param rt_val Output runtime value
 * @return LLACE_ERROR_NONE on success
 */
llace_error_t llace_iir_rtval_from_const(llace_iir_context_t *ctx, const llace_value_t *ir_val, llace_iir_rtval_t *rt_val);

/**
 * Convert runtime value to string for debugging
 * @param val Runtime value
 * @param buffer Output buffer
 * @param buffer_size Size of output buffer
 * @return Number of characters written
 */
size_t llace_iir_rtval_to_string(const llace_iir_rtval_t *val, char *buffer, size_t buffer_size);

// ================ Debugging Functions ================ //

/**
 * Enable debug mode
 * @param ctx Interpreter context
 * @param enable Whether to enable debug mode
 */
void llace_iir_set_debug(llace_iir_context_t *ctx, bool enable);

/**
 * Enable instruction tracing
 * @param ctx Interpreter context
 * @param enable Whether to enable tracing
 */
void llace_iir_set_trace(llace_iir_context_t *ctx, bool enable);

/**
 * Add breakpoint at instruction index
 * @param ctx Interpreter context
 * @param instruction_index Instruction index
 * @return LLACE_ERROR_NONE on success
 */
llace_error_t llace_iir_add_breakpoint(llace_iir_context_t *ctx, size_t instruction_index);

/**
 * Remove breakpoint at instruction index
 * @param ctx Interpreter context
 * @param instruction_index Instruction index
 * @return LLACE_ERROR_NONE on success
 */
llace_error_t llace_iir_remove_breakpoint(llace_iir_context_t *ctx, size_t instruction_index);

/**
 * Clear all breakpoints
 * @param ctx Interpreter context
 */
void llace_iir_clear_breakpoints(llace_iir_context_t *ctx);

/**
 * Get current execution frame
 * @param ctx Interpreter context
 * @return Current frame or NULL if not executing
 */
llace_iir_frame_t *llace_iir_get_current_frame(llace_iir_context_t *ctx);

/**
 * Get call stack depth
 * @param ctx Interpreter context
 * @return Call stack depth
 */
size_t llace_iir_get_call_depth(llace_iir_context_t *ctx);

/**
 * Get frame at depth
 * @param ctx Interpreter context
 * @param depth Frame depth (0 = current)
 * @return Frame or NULL if invalid depth
 */
llace_iir_frame_t *llace_iir_get_frame(llace_iir_context_t *ctx, size_t depth);

/**
 * Print execution context for debugging
 * @param ctx Interpreter context
 */
void llace_iir_print_context(llace_iir_context_t *ctx);

/**
 * Print call stack for debugging
 * @param ctx Interpreter context
 */
void llace_iir_print_call_stack(llace_iir_context_t *ctx);

// ================ Statistics Functions ================ //

typedef struct {
  size_t instruction_count;     // Total instructions executed
  size_t function_calls;        // Total function calls
  size_t memory_allocations;    // Total memory allocations
  size_t call_stack_depth;      // Current call stack depth
  size_t max_call_stack_depth;  // Maximum call stack depth reached
} llace_iir_stats_t;

/**
 * Get interpreter statistics
 * @param ctx Interpreter context
 * @param stats Output statistics
 * @return LLACE_ERROR_NONE on success
 */
llace_error_t llace_iir_get_stats(llace_iir_context_t *ctx, llace_iir_stats_t *stats);

/**
 * Reset interpreter statistics
 * @param ctx Interpreter context
 */
void llace_iir_reset_stats(llace_iir_context_t *ctx);

/**
 * Print interpreter statistics
 * @param ctx Interpreter context
 */
void llace_iir_print_stats(llace_iir_context_t *ctx);

// ================ Utility Functions ================ //

/**
 * Check if value is compile-time constant
 * @param ctx Interpreter context
 * @param val Value to check
 * @return true if constant, false otherwise
 */
bool llace_iir_is_constant(llace_iir_context_t *ctx, const llace_value_t *val);

/**
 * Evaluate constant expression
 * @param ctx Interpreter context
 * @param expr Expression to evaluate
 * @param result Output result
 * @return LLACE_ERROR_NONE on success
 */
llace_error_t llace_iir_eval_const(llace_iir_context_t *ctx, const llace_value_t *expr, llace_iir_rtval_t *result);

/**
 * Get state string for debugging
 * @param state Interpreter state
 * @return String representation
 */
const char *llace_iir_state_str(llace_iir_state_t state);

#ifdef __cplusplus
}
#endif

#endif // LLACE_IIR_H