#include <stdlib.h>
#include <string.h>
#include <llace/mem.h>
#include <llace/log.h>

// ================ Item Allocation ================ //

// llace_item_t llace_mem_new(size_t size);
// void llace_mem_free(llace_item_t *item);
// void *llace_mem_get(llace_item_t);

// ================ Array Allocation ================ //

llace_array_t llace_mem_newarray(size_t element_size, size_t element_capacity) {
  llace_array_t arr;
  
  // CHECK: Potential integer overflow
  size_t total_size = element_size * element_capacity;
  if (element_capacity > 0 && total_size / element_capacity != element_size) {
    LLACE_LOG_FATAL("Integer overflow in allocation: %zu * %zu", element_size, element_capacity);
  }
  
  arr.data = malloc(total_size);
  arr.element_size = element_size;
  arr.element_capacity = element_capacity;
  arr.element_count = 0;

  if (total_size != 0 && arr.data == NULL) {
    LLACE_LOG_FATAL("Failed to allocate requested size '%zu: size * %zu: capacity = %zu'", element_size, element_capacity, total_size);
  }

  return arr;
}

void llace_mem_freearray(llace_array_t *arr) {
  if (arr->data) {
    free(arr->data);
    arr->element_capacity = 0;
    arr->element_count = 0;
    arr->data = NULL;
  }
}

void llace_mem_reserve(llace_array_t *arr, size_t element_capacity) {
  if (arr == NULL) { LLACE_LOG_FATAL("You passed a NULL array? Really?"); }
  
  if (element_capacity <= arr->element_capacity) {
    return;
  }

  // CHECK: Potential integer overflow
  size_t total_size = arr->element_size * element_capacity;
  if (element_capacity > 0 && total_size / element_capacity != arr->element_size) {
    LLACE_LOG_FATAL("Integer overflow in reallocation: %zu * %zu", arr->element_size, element_capacity);
  }

  void *new_data = realloc(arr->data, total_size);
  if (new_data == NULL && total_size != 0) {
    LLACE_LOG_FATAL("Failed to allocate requested size '%zu: size * %zu: capacity = %zu'", arr->element_size, element_capacity, total_size);
  }
  
  arr->data = new_data;
  arr->element_capacity = element_capacity;
}

void llace_mem_array_push(llace_array_t *arr, const void *data) {
  if (arr == NULL) { LLACE_LOG_FATAL("You passed a NULL array? Really?"); }
  if (data == NULL) { LLACE_LOG_FATAL("Data is NULL"); }

  size_t new_capacity = arr->element_capacity == 0 ? 1 : arr->element_capacity * 2;
  
  if (arr->element_capacity <= arr->element_count) {
    llace_mem_reserve(arr, new_capacity);
  }

  void *dest = ((char*)arr->data) + (arr->element_count * arr->element_size);
  memcpy(dest, data, arr->element_size);
  ++arr->element_count;
}

void llace_mem_array_pusha(llace_array_t *arr, const void *data, size_t count) {
  if (arr == NULL) { LLACE_LOG_FATAL("You passed a NULL array? Really?"); }
  if (data == NULL) { LLACE_LOG_FATAL("Data is NULL"); }

  if (count == 0) return;

  if (arr->element_capacity < arr->element_count + count) {
    size_t needed_capacity = arr->element_count + count;
    size_t double_capacity = arr->element_capacity == 0 ? 1 : arr->element_capacity * 2;
    size_t new_capacity = needed_capacity > double_capacity ? needed_capacity : double_capacity;
    llace_mem_reserve(arr, new_capacity);
  }

  void *dest = ((char*)arr->data) + (arr->element_count * arr->element_size);
  memcpy(dest, data, arr->element_size * count);
  
  arr->element_count += count;
}

void *llace_mem_array_get(const llace_array_t *arr, size_t index) {
  if (!arr) { LLACE_LOG_FATAL("You passed a NULL array? Really?"); }

  if (index >= arr->element_count) {
    LLACE_LOG_DEBUG("Array index out of bounds: %zu >= %zu", index, arr->element_count);
    return NULL;
  }
  
  return ((char*)arr->data) + (index * arr->element_size);
}

void *llace_mem_array_back(const llace_array_t *arr) {
  if (arr == NULL) { LLACE_LOG_FATAL("You passed a NULL array? Really?"); }
  
  if (arr->element_count == 0) {
    LLACE_LOG_FATAL("Cannot get back element of empty array");
  }
  
  return llace_mem_array_get(arr, arr->element_count - 1);
}

void *llace_mem_array_front(const llace_array_t *arr) {
  if (arr == NULL) { LLACE_LOG_FATAL("You passed a NULL array? Really?"); }
  
  if (arr->element_count == 0) {
    LLACE_LOG_FATAL("Cannot get front element of empty array");
  }
  
  return llace_mem_array_get(arr, 0);
}