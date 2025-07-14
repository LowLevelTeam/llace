#include <llace/llace.h>
#include <llace/mem.h>
#include <string.h>
#include <stdio.h>

// ================ Version Information ================ //

const char *llace_version_str(void) {
  static char version_buffer[64];
  snprintf(version_buffer, sizeof(version_buffer), "%d.%d.%d", 
           LLACE_VERSION_MAJOR, LLACE_VERSION_MINOR, LLACE_VERSION_PATCH);
  return version_buffer;
}

// ================ Target Support ================ //

bool llace_target_is_supported(llace_target_t target) {
  switch (target) {
    case LLACE_TARGET_AMD64_GNU_ELF64:
      return true;
    default:
      return false;
  }
}

const char *llace_target_str(llace_target_t target) {
  switch (target) {
    case LLACE_TARGET_AMD64_GNU_ELF64:
      return "x86_64-gnu-elf64";
    default:
      return "unknown";
  }
}

// ================ Error Handling ================ //

const char *llace_error_str(llace_error_t error) {
  switch (error) {
    case LLACE_ERROR_NONE:
      return "No error";
    case LLACE_ERROR_NOMEM:
      return "Out of memory";
    case LLACE_ERROR_INVLMOD:
      return "Invalid module or operation";
    case LLACE_ERROR_INVLFUNC:
      return "Invalid function";
    case LLACE_ERROR_INVLTYPE:
      return "Invalid type";
    case LLACE_ERROR_IO:
      return "I/O error";
    case LLACE_ERROR_BADTARGET:
      return "Unsupported target";
    default:
      return "Unknown error";
  }
}

// ================ Library State ================ //

static struct {
  bool initialized;
  size_t initial_memory_size;
  const char *build_date;
  const char *build_config;
} g_llace_state = {
  .initialized = false,
  .initial_memory_size = 0,
  .build_date = __DATE__ " " __TIME__,
#ifdef NDEBUG
  .build_config = "Release"
#else
  .build_config = "Debug"
#endif
};

// ================ Library Management ================ //

llace_error_t llace_init(size_t initial_memory_size) {
  if (g_llace_state.initialized) {
    // Already initialized, cleanup first
    llace_cleanup();
  }
  
  // Initialize memory management system
  llace_error_t err = llace_mem_init(initial_memory_size);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Set up default memory configuration optimized for IR compilation
  llace_mem_config_t config;
  llace_mem_get_config(&config);
  
  // Configure for IR compiler usage patterns
  config.min_allocation_size = 8;     // Good alignment for most IR structures
  config.compaction_threshold = 40;   // Compact when 40% fragmented
  config.auto_expand = true;          // Allow pool to grow as needed
  config.auto_compact = true;         // Automatic defragmentation
  
  err = llace_mem_configure(&config);
  if (err != LLACE_ERROR_NONE) {
    llace_mem_cleanup();
    return err;
  }
  
  g_llace_state.initialized = true;
  g_llace_state.initial_memory_size = initial_memory_size;
  
  return LLACE_ERROR_NONE;
}

void llace_cleanup(void) {
  if (!g_llace_state.initialized) {
    return;
  }
  
  // Cleanup memory management system
  llace_mem_cleanup();
  
  // Reset state
  g_llace_state.initialized = false;
  g_llace_state.initial_memory_size = 0;
}

bool llace_is_initialized(void) {
  return g_llace_state.initialized;
}

// ================ Library Information ================ //

llace_error_t llace_get_info(llace_info_t *info) {
  if (!info) {
    return LLACE_ERROR_INVLMOD;
  }
  
  if (!g_llace_state.initialized) {
    return LLACE_ERROR_INVLMOD;
  }
  
  // Get memory statistics
  llace_mem_stats_t mem_stats;
  llace_error_t err = llace_mem_get_stats(&mem_stats);
  if (err != LLACE_ERROR_NONE) {
    return err;
  }
  
  // Fill in library information
  info->memory_pool_size = mem_stats.total_size;
  info->memory_used = mem_stats.used_size;
  info->active_allocations = mem_stats.allocation_count;
  info->compactions_performed = mem_stats.compaction_count;
  info->version = llace_version_str();
  info->build_date = g_llace_state.build_date;
  info->build_config = g_llace_state.build_config;
  
  return LLACE_ERROR_NONE;
}

void llace_print_info(void) {
  printf("LLACE IR Compiler Library\n");
  printf("========================\n");
  
  if (!g_llace_state.initialized) {
    printf("Status: Not initialized\n");
    printf("Version: %s\n", llace_version_str());
    printf("Build: %s (%s)\n", g_llace_state.build_config, g_llace_state.build_date);
    return;
  }
  
  llace_info_t info;
  if (llace_get_info(&info) == LLACE_ERROR_NONE) {
    printf("Status: Initialized\n");
    printf("Version: %s\n", info.version);
    printf("Build: %s (%s)\n", info.build_config, info.build_date);
    printf("Memory Pool: %zu bytes\n", info.memory_pool_size);
    printf("Memory Used: %zu bytes (%.1f%%)\n", 
           info.memory_used, 
           info.memory_pool_size > 0 ? 
           (double)info.memory_used / info.memory_pool_size * 100.0 : 0.0);
    printf("Active Allocations: %zu\n", info.active_allocations);
    printf("Compactions: %zu\n", info.compactions_performed);
  } else {
    printf("Status: Error retrieving information\n");
  }
  
  printf("\nSupported Targets:\n");
  printf("  - %s: %s\n", 
         llace_target_str(LLACE_TARGET_AMD64_GNU_ELF64),
         llace_target_is_supported(LLACE_TARGET_AMD64_GNU_ELF64) ? "supported" : "not supported");
}