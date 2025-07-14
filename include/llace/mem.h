#ifndef LLACE_MEM_H
#define LLACE_MEM_H

#include <llace/llace.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ================ Memory Handle System ================ //

/**
 * Handle-based memory allocation system for LLACE IR compiler.
 * 
 * This system uses handles instead of raw pointers to allow for memory
 * compaction and relocation without breaking references. All allocations
 * are tracked and can be moved during defragmentation.
 */

typedef uint32_t llace_handle_t;
#define LLACE_HANDLE_INVALID 0

// ================ Memory Manager ================ //

/**
 * Initialize the global memory manager with an initial pool size.
 * Must be called before any other memory operations.
 * 
 * @param initial_size Initial size of the memory pool in bytes
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_mem_init(size_t initial_size);

/**
 * Cleanup and free all memory managed by the system.
 * Invalidates all existing handles.
 */
void llace_mem_cleanup(void);

/**
 * Compact memory by moving allocations to eliminate fragmentation.
 * This operation may change the physical addresses of allocations but
 * handles remain valid.
 * 
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_mem_compact(void);

/**
 * Expand the memory pool if needed to accommodate more allocations.
 * 
 * @param additional_size Additional bytes to add to the pool
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_mem_expand(size_t additional_size);

// ================ Allocation Functions ================ //

/**
 * Allocate a block of memory.
 * 
 * @param size Size in bytes to allocate
 * @param handle Output parameter for the allocated handle
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_mem_alloc(size_t size, llace_handle_t *handle);

/**
 * Allocate an array of elements.
 * This is optimized for array allocations common in IR operations.
 * 
 * @param element_size Size of each array element
 * @param count Number of elements
 * @param handle Output parameter for the allocated handle
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_mem_alloc_array(size_t element_size, size_t count, llace_handle_t *handle);

/**
 * Reallocate a memory block to a new size.
 * May move the allocation to a new location.
 * 
 * @param handle Handle to the existing allocation
 * @param new_size New size in bytes
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_mem_realloc(llace_handle_t handle, size_t new_size);

/**
 * Reallocate an array to a new element count.
 * 
 * @param handle Handle to the existing array allocation
 * @param new_count New number of elements
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_mem_realloc_array(llace_handle_t handle, size_t new_count);

/**
 * Free a memory allocation.
 * 
 * @param handle Handle to the allocation to free
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_mem_free(llace_handle_t handle);

// ================ Access Functions ================ //

/**
 * Get a pointer to the memory referenced by a handle.
 * 
 * WARNING: This pointer is only valid until the next call to llace_mem_compact()
 * or any operation that may trigger compaction. For long-term storage,
 * always use handles.
 * 
 * @param handle Handle to the allocation
 * @return Pointer to the memory, or NULL if handle is invalid
 */
void *llace_mem_get(llace_handle_t handle);

/**
 * Get the size of an allocation.
 * 
 * @param handle Handle to the allocation
 * @return Size in bytes, or 0 if handle is invalid
 */
size_t llace_mem_size(llace_handle_t handle);

/**
 * Get the element count for an array allocation.
 * 
 * @param handle Handle to the array allocation
 * @return Number of elements, or 0 if handle is invalid or not an array
 */
size_t llace_mem_array_count(llace_handle_t handle);

/**
 * Get the element size for an array allocation.
 * 
 * @param handle Handle to the array allocation
 * @return Size of each element, or 0 if handle is invalid or not an array
 */
size_t llace_mem_array_element_size(llace_handle_t handle);

/**
 * Check if a handle is valid.
 * 
 * @param handle Handle to check
 * @return true if handle is valid, false otherwise
 */
bool llace_mem_is_valid(llace_handle_t handle);

/**
 * Check if a handle refers to an array allocation.
 * 
 * @param handle Handle to check
 * @return true if handle refers to an array, false otherwise
 */
bool llace_mem_is_array(llace_handle_t handle);

// ================ Memory Statistics ================ //

typedef struct {
  size_t total_size;              // Total size of memory pool
  size_t used_size;               // Currently allocated bytes
  size_t free_size;               // Available bytes
  size_t fragmentation_percent;   // Fragmentation as percentage (0-100)
  size_t allocation_count;        // Number of active allocations
  size_t array_count;             // Number of active array allocations
  size_t compaction_count;        // Number of compactions performed
  size_t expansion_count;         // Number of pool expansions
} llace_mem_stats_t;

/**
 * Get memory usage statistics.
 * 
 * @param stats Output parameter for statistics
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_mem_get_stats(llace_mem_stats_t *stats);

/**
 * Print memory statistics to stdout (debug function).
 */
void llace_mem_print_stats(void);

// ================ Configuration ================ //

typedef struct {
  size_t min_allocation_size;     // Minimum allocation size (for alignment)
  size_t compaction_threshold;    // Fragmentation % that triggers auto-compaction
  bool auto_expand;               // Automatically expand pool when needed
  bool auto_compact;              // Automatically compact when threshold reached
} llace_mem_config_t;

/**
 * Configure memory manager behavior.
 * 
 * @param config Configuration parameters
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_mem_configure(const llace_mem_config_t *config);

/**
 * Get current memory manager configuration.
 * 
 * @param config Output parameter for current configuration
 * @return LLACE_ERROR_NONE on success, error code on failure
 */
llace_error_t llace_mem_get_config(llace_mem_config_t *config);

// ================ Convenience Macros ================ //

/**
 * Type-safe array allocation macro.
 * 
 * @param type Element type
 * @param count Number of elements
 * @param handle_ptr Pointer to handle variable
 */
#define LLACE_MEM_ALLOC_ARRAY(type, count, handle_ptr) \
  llace_mem_alloc_array(sizeof(type), (count), (handle_ptr))

/**
 * Type-safe array reallocation macro.
 * 
 * @param handle Handle to existing array
 * @param type Element type
 * @param new_count New number of elements
 */
#define LLACE_MEM_REALLOC_ARRAY(handle, type, new_count) \
  llace_mem_realloc((handle), sizeof(type) * (new_count))

/**
 * Type-safe pointer retrieval macro.
 * 
 * @param type Element type
 * @param handle Handle to allocation
 */
#define LLACE_MEM_GET_TYPED(type, handle) \
  ((type *)llace_mem_get(handle))

#ifdef NDEBUG

/**
 * Get array element at index (bounds checking in debug builds).
 * 
 * @param type Element type
 * @param handle Handle to array allocation
 * @param index Array index
 */
#define LLACE_MEM_ARRAY_GET(type, handle, index) \
  (((type *)llace_mem_get(handle)) + (index))
#else
#define LLACE_MEM_ARRAY_GET(type, handle, index) \
  llace_mem_array_get_checked(handle, sizeof(type), index)

/**
 * Bounds-checked array element access (debug only).
 * 
 * @param handle Handle to array allocation
 * @param element_size Size of each element
 * @param index Array index
 * @return Pointer to element, or NULL if out of bounds
 */
void *llace_mem_array_get_checked(llace_handle_t handle, size_t element_size, size_t index);
#endif

#ifdef __cplusplus
}
#endif

#endif // LLACE_MEM_H