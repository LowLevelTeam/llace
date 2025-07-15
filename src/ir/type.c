#include <llace/ir/type.h>
#include <llace/ir/module.h>
#include <llace/detail/common.h>
#include <string.h>


llace_error_t llace_type_init(llace_type_t *type) {
  if (!type) {
    LLACE_LOG_ERROR("Type is %p", type);
    return LLACE_ERROR_BADARG;
  }

  memset(type, 0, sizeof(llace_type_t));
  type->name = 0;
  type->kind = LLACE_TYPE_VOID;
  type->size = 0;
  type->alignment = 1;
  
  return LLACE_ERROR_NONE;
}

llace_error_t llace_type_destroy(llace_type_t *type) {
  if (!type) {
    LLACE_LOG_ERROR("Type is %p", type);
    return LLACE_ERROR_BADARG;
  }

  switch (type->kind) {
    case LLACE_TYPE_VOID:
    case LLACE_TYPE_INT:
    case LLACE_TYPE_UNT:
    case LLACE_TYPE_FLOAT:
    case LLACE_TYPE_PTR:
    case LLACE_TYPE_VPTR:
    case LLACE_TYPE_VARADIC:
    case LLACE_TYPE_ARRAY:
      break;
    case LLACE_TYPE_STRUCT:
    case LLACE_TYPE_UNION:
      LLACE_FREE_ARRAY(type->_composite.members);
      break;
    case LLACE_TYPE_FUNCTION:
      LLACE_FREE_ARRAY(type->_func.params);
      LLACE_FREE_ARRAY(type->_func.returns);
      break;
  };

  memset(type, 0, sizeof(llace_type_t));
  return LLACE_ERROR_NONE;
}

// ================ Building ================ //

llace_error_t llace_build_type(llace_type_builder_t *builder, llace_type_t *dest) {
  if (!builder) {
    LLACE_LOG_ERROR("Builder is %p", builder);
    return LLACE_ERROR_BADARG;
  }

  llace_type_t type;

  LLACE_RUNCHECK(llace_type_init(&type));

  type.kind = builder->kind;

  switch (builder->kind) {
    case LLACE_TYPE_VOID:
    case LLACE_TYPE_VPTR:
    case LLACE_TYPE_VARADIC:
      break;
    case LLACE_TYPE_INT:
      type._int = builder->_int;
      break;
    case LLACE_TYPE_UNT:
      type._unt = builder->_unt;
      break;
    case LLACE_TYPE_FLOAT:
      type._float.mantissa = builder->_float.mantissa;
      type._float.exponent = builder->_float.exponent;
      break;
    case LLACE_TYPE_PTR:
      type._ptr.type = builder->_ptr.type;
      type._ptr.depth = builder->_ptr.depth;
      break;
    case LLACE_TYPE_ARRAY:
      type._array.element = builder->_array.element;
      type._array.count = builder->_array.count;
      break;
    case LLACE_TYPE_STRUCT:
    case LLACE_TYPE_UNION:
      type._composite.members = builder->_composite.members;
      break;
    case LLACE_TYPE_FUNCTION:
      type._func.abi = builder->_func.abi;
      type._func.params = builder->_func.params;
      type._func.returns = builder->_func.returns;
      break;
    default:
      LLACE_LOG_ERROR("Invalid Value Kind %d", type.kind);
      return LLACE_ERROR_BADARG;
  };

  if (builder->name) {
    if (!builder->_module) {
      LLACE_LOG_DEBUG("Name given but no module available? is this a mistake?");
    } else {
      LLACE_RUNCHECK(llace_module_add_name(builder->_module, builder->name, (builder->namelen == 0 ? strlen(builder->name) : builder->namelen), &type.name));
    }
  }

  if (dest) {
    *dest = type;
  }

  if (builder->_module) {
    LLACE_RUNCHECK(llace_module_add_type(builder->_module, &type))
  }

  return LLACE_ERROR_NONE;
}
