// ================ Type Implementation ================ //

static size_t llace_type_calculate_size(llace_type_kind_t kind, unsigned long bit_width) {
  switch (kind) {
    case LLACE_TYPE_VOID: return 0;
    case LLACE_TYPE_INT:
    case LLACE_TYPE_UNT:
      return (bit_width + 7) / 8; // Round up to nearest byte
    case LLACE_TYPE_FLOAT:
      if (bit_width <= 32) return 4;
      if (bit_width <= 64) return 8;
      return 16; // long double
    case LLACE_TYPE_PTR:
      return sizeof(void*);
    default:
      return 0;
  }
}

static size_t llace_type_calculate_alignment(llace_type_kind_t kind, size_t size) {
  switch (kind) {
    case LLACE_TYPE_VOID: return 0; // no data to be alligned
    case LLACE_TYPE_INT:
    case LLACE_TYPE_UNT:
    case LLACE_TYPE_FLOAT:
      // Align to size, but cap at pointer size
      if (size <= 1) return 1;
      if (size <= 2) return 2;
      if (size <= 4) return 4;
      return sizeof(void*);
    case LLACE_TYPE_PTR:
      return sizeof(void*);
    default:
      return -1;
  }
}

// Type functions
void llace_type_destroy(llace_type_t *type) {
  if (!type) return;
  
  if (type->name) {
    free((void *)type->name);
  }
  // Free any other resources associated with the type if necessary
  // This could include freeing arrays, structs, etc.
  // For now, we just zero out the type structure since we don't dynamically allocate

  memset(type, 0, sizeof(llace_type_t));
}

void llace_type_init(llace_type_t *type, llace_type_kind_t kind, size_t size, size_t alignment) {
  if (!type) return;
  memset(type, 0, sizeof(llace_type_t));
  type->kind = kind;
  type->size = size;
  type->alignment = alignment;
}

llace_error_t llace_type_name(llace_type_t *type, const char *name) {
  if (!type || !name) return;
  
  // Free existing name if any
  if (type->name) {
    free((void *)type->name);
  }
  
  // Allocate new name
  type->name = strdup(name);
  if (!type->name) {
    return LLACE_ERROR_NOMEM; // Memory allocation failed
  }

  return LLACE_ERROR_NONE; // Success
}