#ifndef LLACE_MEM_H
#define LLACE_MEM_H

#include <llace/llace.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// This system is mainly to keep memory operations easily updateable

// ================ Item Allocation ================ //

typedef struct llace_item {
  void *data;
} llace_item_t;

// Create a new memory item with specified size
#define llace_mem_new(size) (llace_item_t){ .data = malloc(size) }

// Free memory item data
#define llace_mem_free(item) if ((item).data) { free((item).data); (item).data = NULL; }

// Get pointer to item data
#define llace_mem_get(item) ((item).data)

// ================ Array Allocation ================ //

typedef struct llace_array {
  void *data;
  size_t element_size;
  size_t element_count;
  size_t element_capacity;
} llace_array_t;

llace_array_t llace_mem_newarray(size_t element_size, size_t element_capacity);
void llace_mem_freearray(llace_array_t *arr);
void llace_mem_reserve(llace_array_t *arr, size_t element_capacity);
void llace_mem_array_push(llace_array_t *arr, const void *data);
void llace_mem_array_pusha(llace_array_t *arr, const void *data, size_t count);
void *llace_mem_array_get(llace_array_t *arr, size_t index); // item at array index
void *llace_mem_array_back(llace_array_t *arr); // end of array
void *llace_mem_array_front(llace_array_t *arr); // beginning of array

// ================ Interface Macros ================ //

// Allocate memory for a specific type
#define LLACE_NEW(type) llace_mem_new(sizeof(type))

// Free memory item
#define LLACE_FREE(item) llace_mem_free(item)

// Get typed pointer to item data
#define LLACE_GET(type, item) ((type *)((item).data))

// ================ Array Helper Macros ================ //

// Get raw data pointer from array
#define LLACE_ARRAY_RAW(array) ((array).data)

// Get current number of elements in array
#define LLACE_ARRAY_COUNT(array) ((array).element_count)

// Get maximum capacity of array
#define LLACE_ARRAY_CAPACITY(array) ((array).element_capacity)

// Get size of individual elements in array
#define LLACE_ARRAY_ELEMENT_SIZE(array) ((array).element_size)

// Create new array for specific type with given capacity
#define LLACE_NEW_ARRAY(type, capacity) llace_mem_newarray(sizeof(type), capacity)

// Free array and its data
#define LLACE_FREE_ARRAY(array) llace_mem_freearray(&(array))

// Push value to array (by value)
#define LLACE_ARRAY_PUSH(array, value) do { typeof((value)) val = (value); llace_mem_array_push(&(array), &val); } while (0);

// Push value to array (by pointer)
#define LLACE_ARRAY_PUSHP(array, value_ptr) llace_mem_array_push(&(array), (value_ptr))

// Push multiple values to array
#define LLACE_ARRAY_PUSHA(array, value_ptr, count) llace_mem_array_pusha(&(array), (value_ptr), (count))

// Get typed pointer to element at index
#define LLACE_ARRAY_GET(type, array, index) ((type *)llace_mem_array_get(&(array), (index)))

// Get typed pointer to first element
#define LLACE_ARRAY_FRONT(type, array) ((type *)llace_mem_array_front(&(array)))

// Get typed pointer to last element
#define LLACE_ARRAY_BACK(type, array) ((type *)llace_mem_array_back(&(array)))

// Iterate over array elements
#define LLACE_ARRAY_FOREACH(type, var_name, array) \
  for (size_t _llace_i = 0; _llace_i < LLACE_ARRAY_COUNT(array); ++_llace_i) \
    for (type *var_name = LLACE_ARRAY_GET(type, array, _llace_i); \
         var_name != NULL; \
         var_name = NULL)

// Check if array is empty
#define LLACE_ARRAY_IS_EMPTY(array) (LLACE_ARRAY_COUNT(array) == 0)

// Check if array is full
#define LLACE_ARRAY_IS_FULL(array) (LLACE_ARRAY_COUNT(array) >= LLACE_ARRAY_CAPACITY(array))

#ifdef __cplusplus
}
#endif

#endif // LLACE_MEM_H