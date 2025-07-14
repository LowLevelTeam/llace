#include <llace/mem.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

// ================ Internal Constants ================ //

#define LLACE_MEM_DEFAULT_POOL_SIZE (1024 * 1024)  // 1MB default
#define LLACE_MEM_DEFAULT_ALIGNMENT 8
#define LLACE_MEM_HANDLE_TABLE_INITIAL_SIZE 1024
#define LLACE_MEM_MIN_BLOCK_SIZE 16
#define LLACE_MEM_MAGIC 0xDEADBEEF

// ================ Internal Data Structures ================ //

typedef struct llace_mem_block {
  size_t size;                    // Size of the allocation
  size_t offset;                  // Offset from pool start
  bool is_free;                   // Whether block is free
  bool is_array;                  // Whether this is an array allocation
  size_t element_size;            // For arrays: size of each element
  size_t element_count;           // For arrays: number of elements
  uint32_t magic;                 // Magic number for validation
  struct llace_mem_block *next;   // Next block in free list
} llace_mem_block_t;

typedef struct {
  llace_mem_block_t *block;       // Pointer to memory block
  uint32_t generation;            // Generation counter for ABA prevention
} llace_handle_entry_t;

typedef struct {
  // Memory pool
  void *pool;                     // Main memory pool
  size_t pool_size;               // Total pool size
  size_t pool_used;               // Currently used bytes

  // Handle table
  llace_handle_entry_t *handle_table;
  size_t handle_table_size;
  size_t handle_table_capacity;
  llace_handle_t next_handle;

  // Free list
  llace_mem_block_t *free_list;
  
  // Block tracking
  llace_mem_block_t *blocks;      // Array of all blocks
  size_t block_count;
  size_t block_capacity;

  // Statistics
  size_t allocation_count;
  size_t array_count;
  size_t compaction_count;
  size_t expansion_count;

  // Configuration
  llace_mem_config_t config;

  // State
  bool initialized;
} llace_mem_manager_t;

// ================ Global Manager Instance ================ //

static llace_mem_manager_t g_mem_manager = {0};

// ================ Internal Helper Functions ================ //

static size_t align_size(size_t size) {
  size_t alignment = g_mem_manager.config.min_allocation_size;
  return (size + alignment - 1) & ~(alignment - 1);
}

static llace_error_t expand_handle_table(void) {
  size_t new_capacity = g_mem_manager.handle_table_capacity * 2;
  llace_handle_entry_t *new_table = realloc(g_mem_manager.handle_table, 
                                            new_capacity * sizeof(llace_handle_entry_t));
  if (!new_table) {
    return LLACE_ERROR_NOMEM;
  }

  // Clear new entries
  memset(new_table + g_mem_manager.handle_table_capacity, 0, 
         (new_capacity - g_mem_manager.handle_table_capacity) * sizeof(llace_handle_entry_t));

  g_mem_manager.handle_table = new_table;
  g_mem_manager.handle_table_capacity = new_capacity;
  return LLACE_ERROR_NONE;
}

static llace_error_t expand_block_array(void) {
  size_t new_capacity = g_mem_manager.block_capacity * 2;
  llace_mem_block_t *new_blocks = realloc(g_mem_manager.blocks,
                                          new_capacity * sizeof(llace_mem_block_t));
  if (!new_blocks) {
    return LLACE_ERROR_NOMEM;
  }

  g_mem_manager.blocks = new_blocks;
  g_mem_manager.block_capacity = new_capacity;
  return LLACE_ERROR_NONE;
}

static llace_handle_t allocate_handle(llace_mem_block_t *block) {
  // Find free handle slot
  for (size_t i = 1; i < g_mem_manager.handle_table_capacity; i++) {
    if (g_mem_manager.handle_table[i].block == NULL) {
      g_mem_manager.handle_table[i].block = block;
      g_mem_manager.handle_table[i].generation++;
      g_mem_manager.handle_table_size++;
      return (llace_handle_t)i;
    }
  }

  // Need to expand handle table
  llace_error_t err = expand_handle_table();
  if (err != LLACE_ERROR_NONE) {
    return LLACE_HANDLE_INVALID;
  }

  // Try again with expanded table
  size_t i = g_mem_manager.handle_table_size + 1;
  g_mem_manager.handle_table[i].block = block;
  g_mem_manager.handle_table[i].generation = 1;
  g_mem_manager.handle_table_size++;
  return (llace_handle_t)i;
}

static void free_handle(llace_handle_t handle) {
  if (handle == LLACE_HANDLE_INVALID || handle >= g_mem_manager.handle_table_capacity) {
    return;
  }

  g_mem_manager.handle_table[handle].block = NULL;
  g_mem_manager.handle_table[handle].generation++;
  g_mem_manager.handle_table_size--;
}

static llace_mem_block_t *get_block_from_handle(llace_handle_t handle) {
  if (handle == LLACE_HANDLE_INVALID || handle >= g_mem_manager.handle_table_capacity) {
    return NULL;
  }

  return g_mem_manager.handle_table[handle].block;
}

static llace_mem_block_t *find_free_block(size_t size) {
  llace_mem_block_t **current = &g_mem_manager.free_list;
  
  while (*current) {
    if ((*current)->size >= size) {
      llace_mem_block_t *block = *current;
      *current = block->next;  // Remove from free list
      return block;
    }
    current = &(*current)->next;
  }
  
  return NULL;
}

static void add_to_free_list(llace_mem_block_t *block) {
  block->is_free = true;
  block->next = g_mem_manager.free_list;
  g_mem_manager.free_list = block;
}

static llace_mem_block_t *create_new_block(size_t size) {
  if (g_mem_manager.block_count >= g_mem_manager.block_capacity) {
    llace_error_t err = expand_block_array();
    if (err != LLACE_ERROR_NONE) {
      return NULL;
    }
  }

  size_t aligned_size = align_size(size);
  
  // Check if we have space in the pool
  if (g_mem_manager.pool_used + aligned_size > g_mem_manager.pool_size) {
    if (g_mem_manager.config.auto_expand) {
      size_t expand_size = aligned_size > g_mem_manager.pool_size ? 
                          aligned_size * 2 : g_mem_manager.pool_size;
      if (llace_mem_expand(expand_size) != LLACE_ERROR_NONE) {
        return NULL;
      }
    } else {
      return NULL;
    }
  }

  llace_mem_block_t *block = &g_mem_manager.blocks[g_mem_manager.block_count++];
  block->size = aligned_size;
  block->offset = g_mem_manager.pool_used;
  block->is_free = false;
  block->is_array = false;
  block->element_size = 0;
  block->element_count = 0;
  block->magic = LLACE_MEM_MAGIC;
  block->next = NULL;

  g_mem_manager.pool_used += aligned_size;
  return block;
}

static size_t calculate_fragmentation(void) {
  if (g_mem_manager.pool_size == 0) return 0;
  
  size_t total_free = 0;
  size_t largest_free = 0;
  
  llace_mem_block_t *current = g_mem_manager.free_list;
  while (current) {
    total_free += current->size;
    if (current->size > largest_free) {
      largest_free = current->size;
    }
    current = current->next;
  }
  
  if (total_free == 0) return 0;
  
  // Fragmentation = (total_free - largest_free) / total_free * 100
  return ((total_free - largest_free) * 100) / total_free;
}

static void compact_memory_internal(void) {
  // Simple compaction: move all allocated blocks to the beginning
  size_t new_offset = 0;
  
  for (size_t i = 0; i < g_mem_manager.block_count; i++) {
    llace_mem_block_t *block = &g_mem_manager.blocks[i];
    
    if (!block->is_free && block->magic == LLACE_MEM_MAGIC) {
      if (block->offset != new_offset) {
        // Move the data
        void *src = (char *)g_mem_manager.pool + block->offset;
        void *dst = (char *)g_mem_manager.pool + new_offset;
        memmove(dst, src, block->size);
        block->offset = new_offset;
      }
      new_offset += block->size;
    }
  }
  
  // Update pool usage
  g_mem_manager.pool_used = new_offset;
  
  // Rebuild free list
  g_mem_manager.free_list = NULL;
  for (size_t i = 0; i < g_mem_manager.block_count; i++) {
    llace_mem_block_t *block = &g_mem_manager.blocks[i];
    if (block->is_free) {
      add_to_free_list(block);
    }
  }
  
  g_mem_manager.compaction_count++;
}

// ================ Public API Implementation ================ //

llace_error_t llace_mem_init(size_t initial_size) {
  if (g_mem_manager.initialized) {
    llace_mem_cleanup();
  }

  size_t pool_size = initial_size > 0 ? initial_size : LLACE_MEM_DEFAULT_POOL_SIZE;
  
  // Allocate memory pool
  g_mem_manager.pool = malloc(pool_size);
  if (!g_mem_manager.pool) {
    return LLACE_ERROR_NOMEM;
  }

  // Allocate handle table
  g_mem_manager.handle_table = calloc(LLACE_MEM_HANDLE_TABLE_INITIAL_SIZE, 
                                      sizeof(llace_handle_entry_t));
  if (!g_mem_manager.handle_table) {
    free(g_mem_manager.pool);
    return LLACE_ERROR_NOMEM;
  }

  // Allocate block array
  g_mem_manager.blocks = malloc(LLACE_MEM_HANDLE_TABLE_INITIAL_SIZE * sizeof(llace_mem_block_t));
  if (!g_mem_manager.blocks) {
    free(g_mem_manager.pool);
    free(g_mem_manager.handle_table);
    return LLACE_ERROR_NOMEM;
  }

  // Initialize manager
  g_mem_manager.pool_size = pool_size;
  g_mem_manager.pool_used = 0;
  g_mem_manager.handle_table_capacity = LLACE_MEM_HANDLE_TABLE_INITIAL_SIZE;
  g_mem_manager.handle_table_size = 0;
  g_mem_manager.next_handle = 1;
  g_mem_manager.free_list = NULL;
  g_mem_manager.block_count = 0;
  g_mem_manager.block_capacity = LLACE_MEM_HANDLE_TABLE_INITIAL_SIZE;
  g_mem_manager.allocation_count = 0;
  g_mem_manager.array_count = 0;
  g_mem_manager.compaction_count = 0;
  g_mem_manager.expansion_count = 0;

  // Set default configuration
  g_mem_manager.config.min_allocation_size = LLACE_MEM_DEFAULT_ALIGNMENT;
  g_mem_manager.config.compaction_threshold = 50;  // 50% fragmentation
  g_mem_manager.config.auto_expand = true;
  g_mem_manager.config.auto_compact = true;

  g_mem_manager.initialized = true;
  return LLACE_ERROR_NONE;
}

void llace_mem_cleanup(void) {
  if (!g_mem_manager.initialized) {
    return;
  }

  free(g_mem_manager.pool);
  free(g_mem_manager.handle_table);
  free(g_mem_manager.blocks);
  
  memset(&g_mem_manager, 0, sizeof(g_mem_manager));
}

llace_error_t llace_mem_compact(void) {
  if (!g_mem_manager.initialized) {
    return LLACE_ERROR_INVLMOD;
  }

  compact_memory_internal();
  return LLACE_ERROR_NONE;
}

llace_error_t llace_mem_expand(size_t additional_size) {
  if (!g_mem_manager.initialized) {
    return LLACE_ERROR_INVLMOD;
  }

  size_t new_size = g_mem_manager.pool_size + additional_size;
  void *new_pool = realloc(g_mem_manager.pool, new_size);
  if (!new_pool) {
    return LLACE_ERROR_NOMEM;
  }

  g_mem_manager.pool = new_pool;
  g_mem_manager.pool_size = new_size;
  g_mem_manager.expansion_count++;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_mem_alloc(size_t size, llace_handle_t *handle) {
  if (!g_mem_manager.initialized || !handle || size == 0) {
    return LLACE_ERROR_INVLMOD;
  }

  *handle = LLACE_HANDLE_INVALID;

  // Try to find a free block first
  llace_mem_block_t *block = find_free_block(size);
  if (!block) {
    // Create new block
    block = create_new_block(size);
    if (!block) {
      return LLACE_ERROR_NOMEM;
    }
  } else {
    block->is_free = false;
    block->next = NULL;
  }

  // Allocate handle
  llace_handle_t new_handle = allocate_handle(block);
  if (new_handle == LLACE_HANDLE_INVALID) {
    add_to_free_list(block);
    return LLACE_ERROR_NOMEM;
  }

  *handle = new_handle;
  g_mem_manager.allocation_count++;

  // Check if auto-compaction is needed
  if (g_mem_manager.config.auto_compact) {
    size_t fragmentation = calculate_fragmentation();
    if (fragmentation > g_mem_manager.config.compaction_threshold) {
      compact_memory_internal();
    }
  }

  return LLACE_ERROR_NONE;
}

llace_error_t llace_mem_alloc_array(size_t element_size, size_t count, llace_handle_t *handle) {
  if (!g_mem_manager.initialized || !handle || element_size == 0 || count == 0) {
    return LLACE_ERROR_INVLMOD;
  }

  size_t total_size = element_size * count;
  llace_error_t err = llace_mem_alloc(total_size, handle);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }

  // Mark as array and set metadata
  llace_mem_block_t *block = get_block_from_handle(*handle);
  if (block) {
    block->is_array = true;
    block->element_size = element_size;
    block->element_count = count;
    g_mem_manager.array_count++;
  }

  return LLACE_ERROR_NONE;
}

llace_error_t llace_mem_realloc(llace_handle_t handle, size_t new_size) {
  if (!g_mem_manager.initialized || handle == LLACE_HANDLE_INVALID || new_size == 0) {
    return LLACE_ERROR_INVLMOD;
  }

  llace_mem_block_t *block = get_block_from_handle(handle);
  if (!block || block->is_free || block->magic != LLACE_MEM_MAGIC) {
    return LLACE_ERROR_INVLMOD;
  }

  size_t aligned_size = align_size(new_size);

  // If new size fits in current block, just update size
  if (aligned_size <= block->size) {
    if (block->is_array) {
      // Update array count for smaller size
      block->element_count = new_size / block->element_size;
    }
    return LLACE_ERROR_NONE;
  }

  // Need to allocate new block
  llace_handle_t new_handle;
  llace_error_t err = llace_mem_alloc(new_size, &new_handle);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }

  // Copy data from old to new
  void *old_ptr = llace_mem_get(handle);
  void *new_ptr = llace_mem_get(new_handle);
  if (old_ptr && new_ptr) {
    size_t copy_size = block->size < new_size ? block->size : new_size;
    memcpy(new_ptr, old_ptr, copy_size);
  }

  // Update handle to point to new block
  llace_mem_block_t *new_block = get_block_from_handle(new_handle);
  if (new_block && block->is_array) {
    new_block->is_array = true;
    new_block->element_size = block->element_size;
    new_block->element_count = new_size / block->element_size;
  }

  // Update handle table
  g_mem_manager.handle_table[handle].block = new_block;
  g_mem_manager.handle_table[handle].generation++;

  // Free old block
  add_to_free_list(block);
  free_handle(new_handle);  // Free the temporary handle

  return LLACE_ERROR_NONE;
}

llace_error_t llace_mem_realloc_array(llace_handle_t handle, size_t new_count) {
  llace_mem_block_t *block = get_block_from_handle(handle);
  if (!block || !block->is_array || block->magic != LLACE_MEM_MAGIC) {
    return LLACE_ERROR_INVLMOD;
  }

  size_t new_size = block->element_size * new_count;
  return llace_mem_realloc(handle, new_size);
}

llace_error_t llace_mem_free(llace_handle_t handle) {
  if (!g_mem_manager.initialized || handle == LLACE_HANDLE_INVALID) {
    return LLACE_ERROR_INVLMOD;
  }

  llace_mem_block_t *block = get_block_from_handle(handle);
  if (!block || block->is_free || block->magic != LLACE_MEM_MAGIC) {
    return LLACE_ERROR_INVLMOD;
  }

  if (block->is_array) {
    g_mem_manager.array_count--;
  }
  g_mem_manager.allocation_count--;

  add_to_free_list(block);
  free_handle(handle);

  return LLACE_ERROR_NONE;
}

void *llace_mem_get(llace_handle_t handle) {
  if (!g_mem_manager.initialized || handle == LLACE_HANDLE_INVALID) {
    return NULL;
  }

  llace_mem_block_t *block = get_block_from_handle(handle);
  if (!block || block->is_free || block->magic != LLACE_MEM_MAGIC) {
    return NULL;
  }

  return (char *)g_mem_manager.pool + block->offset;
}

size_t llace_mem_size(llace_handle_t handle) {
  llace_mem_block_t *block = get_block_from_handle(handle);
  if (!block || block->is_free || block->magic != LLACE_MEM_MAGIC) {
    return 0;
  }
  return block->size;
}

size_t llace_mem_array_count(llace_handle_t handle) {
  llace_mem_block_t *block = get_block_from_handle(handle);
  if (!block || !block->is_array || block->magic != LLACE_MEM_MAGIC) {
    return 0;
  }
  return block->element_count;
}

size_t llace_mem_array_element_size(llace_handle_t handle) {
  llace_mem_block_t *block = get_block_from_handle(handle);
  if (!block || !block->is_array || block->magic != LLACE_MEM_MAGIC) {
    return 0;
  }
  return block->element_size;
}

bool llace_mem_is_valid(llace_handle_t handle) {
  if (!g_mem_manager.initialized || handle == LLACE_HANDLE_INVALID) {
    return false;
  }

  llace_mem_block_t *block = get_block_from_handle(handle);
  return block && !block->is_free && block->magic == LLACE_MEM_MAGIC;
}

bool llace_mem_is_array(llace_handle_t handle) {
  llace_mem_block_t *block = get_block_from_handle(handle);
  return block && block->is_array && block->magic == LLACE_MEM_MAGIC;
}

llace_error_t llace_mem_get_stats(llace_mem_stats_t *stats) {
  if (!g_mem_manager.initialized || !stats) {
    return LLACE_ERROR_INVLMOD;
  }

  stats->total_size = g_mem_manager.pool_size;
  stats->used_size = g_mem_manager.pool_used;
  stats->free_size = g_mem_manager.pool_size - g_mem_manager.pool_used;
  stats->fragmentation_percent = calculate_fragmentation();
  stats->allocation_count = g_mem_manager.allocation_count;
  stats->array_count = g_mem_manager.array_count;
  stats->compaction_count = g_mem_manager.compaction_count;
  stats->expansion_count = g_mem_manager.expansion_count;

  return LLACE_ERROR_NONE;
}

void llace_mem_print_stats(void) {
  llace_mem_stats_t stats;
  if (llace_mem_get_stats(&stats) == LLACE_ERROR_NONE) {
    printf("LLACE Memory Statistics:\n");
    printf("  Total Size: %zu bytes\n", stats.total_size);
    printf("  Used Size: %zu bytes\n", stats.used_size);
    printf("  Free Size: %zu bytes\n", stats.free_size);
    printf("  Fragmentation: %zu%%\n", stats.fragmentation_percent);
    printf("  Active Allocations: %zu\n", stats.allocation_count);
    printf("  Active Arrays: %zu\n", stats.array_count);
    printf("  Compactions: %zu\n", stats.compaction_count);
    printf("  Expansions: %zu\n", stats.expansion_count);
  }
}

llace_error_t llace_mem_configure(const llace_mem_config_t *config) {
  if (!g_mem_manager.initialized || !config) {
    return LLACE_ERROR_INVLMOD;
  }

  g_mem_manager.config = *config;
  return LLACE_ERROR_NONE;
}

llace_error_t llace_mem_get_config(llace_mem_config_t *config) {
  if (!g_mem_manager.initialized || !config) {
    return LLACE_ERROR_INVLMOD;
  }

  *config = g_mem_manager.config;
  return LLACE_ERROR_NONE;
}

#ifndef NDEBUG
void *llace_mem_array_get_checked(llace_handle_t handle, size_t element_size, size_t index) {
  llace_mem_block_t *block = get_block_from_handle(handle);
  if (!block || !block->is_array || block->magic != LLACE_MEM_MAGIC) {
    return NULL;
  }

  if (index >= block->element_count || element_size != block->element_size) {
    return NULL;
  }

  char *base = (char *)llace_mem_get(handle);
  return base ? base + (index * element_size) : NULL;
}
#endif