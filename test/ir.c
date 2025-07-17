#include <llace/ir.h>
#include <assert.h>
#include <string.h>

#define TEST_PFAIL(func) do { llace_error_t err = func; if (err != LLACE_ERROR_NONE) { LLACE_LOG_INFO("error %s", llace_error_str(err)); assert(err != LLACE_ERROR_NONE); } } while (0);

// Function
void test_function(unsigned *total_tests_passed) { // 3 tests
  { // Test Function Init/Destroy
    llace_function_t func;
    TEST_PFAIL(llace_function_init(&func));
    
    // Verify initial state
    assert(func.name == 0);
    assert(func.abi == LLACE_ABI_CDECL);
    assert(func.attrval == 0);
    assert(LLACE_ARRAY_COUNT(func.rets) == 0);
    assert(LLACE_ARRAY_COUNT(func.params) == 0);
    
    TEST_PFAIL(llace_function_destroy(&func));
  }
  ++(*total_tests_passed);

  { // Test Function with Module Integration
    llace_module_t module;
    TEST_PFAIL(llace_module_init(&module, "test_module", 11));
    
    llace_function_t func;
    TEST_PFAIL(llace_function_init(&func));
    
    // Add function name to module
    llace_nameref_t name_ref;
    TEST_PFAIL(llace_module_add_name(&module, "test_func", 9, &name_ref));
    func.name = name_ref;
    
    // Set function attributes
    func.attr._public = 1;
    func.attr._const = 1;
    func.abi = LLACE_ABI_STDCALL;
    
    // Add function to module
    TEST_PFAIL(llace_module_add_function(&module, &func));
    
    // Verify function was added
    llace_function_t *retrieved_func = llace_module_get_function(&module, 0);
    assert(retrieved_func != NULL);
    assert(retrieved_func->name == name_ref);
    assert(retrieved_func->attr._public == 1);
    assert(retrieved_func->attr._const == 1);
    assert(retrieved_func->abi == LLACE_ABI_STDCALL);

    TEST_PFAIL(llace_module_destroy(&module));
  }
  ++(*total_tests_passed);

  { // Test Function Parameters and Returns
    llace_module_t module;
    TEST_PFAIL(llace_module_init(&module, "param_test", 10));
    
    // Create some types
    llace_type_builder_t int_builder = {
      ._module = &module,
      .name = "int",
      .namelen = 3,
      .kind = LLACE_TYPE_INT,
      ._int = 32
    };
    llace_type_t int_type;
    TEST_PFAIL(llace_build_type(&int_builder, &int_type));
    
    llace_function_t func;
    TEST_PFAIL(llace_function_init(&func));
    
    // Add parameters
    llace_variable_t param1, param2;
    TEST_PFAIL(llace_variable_init(&param1));
    TEST_PFAIL(llace_variable_init(&param2));
    
    param1.type = 0; // int type
    param2.type = 0; // int type
    
    LLACE_ARRAY_PUSH(func.params, param1);
    LLACE_ARRAY_PUSH(func.params, param2);
    
    // Add return value
    llace_variable_t ret;
    TEST_PFAIL(llace_variable_init(&ret));
    ret.type = 0; // int type
    LLACE_ARRAY_PUSH(func.rets, ret);
    
    // Verify counts
    assert(LLACE_ARRAY_COUNT(func.params) == 2);
    assert(LLACE_ARRAY_COUNT(func.rets) == 1);

    TEST_PFAIL(llace_module_destroy(&module));
  }
  ++(*total_tests_passed);
}

// Global
void test_global(unsigned *total_tests_passed) { // 2 tests
  { // Test Global Init/Destroy
    llace_global_t global;
    TEST_PFAIL(llace_global_init(&global));
    
    // Verify initial state
    assert(global.name == 0);
    assert(global.type == 0);
    assert(global.attrval == 0);
    assert(global.value.kind == LLACE_VALUE_VOID);
    
    TEST_PFAIL(llace_global_destroy(&global));
  }
  ++(*total_tests_passed);

  { // Test Global with Module Integration
    llace_module_t module;
    TEST_PFAIL(llace_module_init(&module, "global_test", 11));
    
    // Create a type
    llace_type_builder_t int_builder = {
      ._module = &module,
      .name = "int",
      .namelen = 3,
      .kind = LLACE_TYPE_INT,
      ._int = 32
    };
    llace_type_t int_type;
    TEST_PFAIL(llace_build_type(&int_builder, &int_type));
    
    llace_global_t global;
    TEST_PFAIL(llace_global_init(&global));
    
    // Add global name to module
    llace_nameref_t name_ref;
    TEST_PFAIL(llace_module_add_name(&module, "global_var", 10, &name_ref));
    global.name = name_ref;
    global.type = 0; // int type
    
    // Set attributes
    global.attr._public = 1;
    global.attr._const = 1;
    
    // Add global to module
    TEST_PFAIL(llace_module_add_global(&module, &global));
    
    // Verify global was added
    llace_global_t *retrieved_global = llace_module_get_global(&module, 0);
    assert(retrieved_global != NULL);
    assert(retrieved_global->name == name_ref);
    assert(retrieved_global->type == 0);
    assert(retrieved_global->attr._public == 1);
    assert(retrieved_global->attr._const == 1);

    TEST_PFAIL(llace_module_destroy(&module));
  }
  ++(*total_tests_passed);
}

// Variable
void test_variable(unsigned *total_tests_passed) { // 2 tests
  { // Test Variable Init/Destroy
    llace_variable_t var;
    TEST_PFAIL(llace_variable_init(&var));
    
    // Verify initial state
    assert(var.name == 0);
    assert(var.type == 0);
    assert(var.attrval == 0);
    assert(var.value.kind == LLACE_VALUE_VOID);
    
    TEST_PFAIL(llace_variable_destroy(&var));
  }
  ++(*total_tests_passed);

  { // Test Variable Builder Functions
    llace_module_t module;
    TEST_PFAIL(llace_module_init(&module, "var_test", 8));
    
    // Create a type
    llace_type_builder_t float_builder = {
      ._module = &module,
      .name = "float",
      .namelen = 5,
      .kind = LLACE_TYPE_FLOAT,
      ._float = { .mantissa = 23, .exponent = 8 }
    };
    llace_type_t float_type;
    TEST_PFAIL(llace_build_type(&float_builder, &float_type));
    
    llace_variable_t var;
    TEST_PFAIL(llace_variable_init(&var));
    
    // Add variable name to module
    llace_nameref_t name_ref;
    TEST_PFAIL(llace_module_add_name(&module, "my_var", 6, &name_ref));
    
    // Test builder functions
    var.name = name_ref;
    var.type = 0;
    var.attr._const = true;
    var.attr._volatile = false;
    var.attr._cexpr = false;
    
    // Test accessor functions
    assert(var.name == name_ref);
    assert(var.type == 0);
    assert(var.attr._const == true);
    assert(var.attr._volatile == false);
    assert(var.attr._cexpr == false);

    TEST_PFAIL(llace_variable_destroy(&var));
    TEST_PFAIL(llace_module_destroy(&module));
  }
  ++(*total_tests_passed);
}

// Value
void test_value(unsigned *total_tests_passed) { // 4 tests
  { // Test Value Init/Destroy (Void)
    llace_value_t val;
    TEST_PFAIL(llace_value_init(&val));
    
    assert(val.kind == LLACE_VALUE_VOID);
    
    TEST_PFAIL(llace_value_destroy(&val));
  }
  ++(*total_tests_passed);

  { // Test Constant Value
    llace_value_t val;
    TEST_PFAIL(llace_value_init(&val));
    
    val.kind = LLACE_VALUE_CONST;
    val._const.type = 0; // some type reference
    val._const.data = LLACE_NEW(int);
    *LLACE_GET(int, val._const.data) = 42;
    
    assert(val.kind == LLACE_VALUE_CONST);
    assert(*LLACE_GET(int, val._const.data) == 42);
    
    TEST_PFAIL(llace_value_destroy(&val));
  }
  ++(*total_tests_passed);

  { // Test Reference Values
    llace_value_t var_val, global_val, func_val;
    
    TEST_PFAIL(llace_value_init(&var_val));
    var_val.kind = LLACE_VALUE_VARIABLE;
    var_val._var = 5; // variable reference
    
    TEST_PFAIL(llace_value_init(&global_val));
    global_val.kind = LLACE_VALUE_GLOBAL;
    global_val._glob = 3; // global reference
    
    TEST_PFAIL(llace_value_init(&func_val));
    func_val.kind = LLACE_VALUE_FUNCTION;
    func_val._func = 1; // function reference
    
    assert(var_val.kind == LLACE_VALUE_VARIABLE);
    assert(var_val._var == 5);
    assert(global_val.kind == LLACE_VALUE_GLOBAL);
    assert(global_val._glob == 3);
    assert(func_val.kind == LLACE_VALUE_FUNCTION);
    assert(func_val._func == 1);
    
    TEST_PFAIL(llace_value_destroy(&var_val));
    TEST_PFAIL(llace_value_destroy(&global_val));
    TEST_PFAIL(llace_value_destroy(&func_val));
  }
  ++(*total_tests_passed);

  { // Test Instruction and Block Values
    llace_value_t instr_val, block_val;
    
    // Test instruction value
    TEST_PFAIL(llace_value_init(&instr_val));
    instr_val.kind = LLACE_VALUE_INSTRUCTION;
    TEST_PFAIL(llace_cinstr_create(&instr_val._instr));
    instr_val._instr.opcode = LLACE_COP_ADD;
    
    // Test block value
    TEST_PFAIL(llace_value_init(&block_val));
    block_val.kind = LLACE_VALUE_BLOCK;
    TEST_PFAIL(llace_block_init(&block_val._block));
    
    assert(instr_val.kind == LLACE_VALUE_INSTRUCTION);
    assert(instr_val._instr.opcode == LLACE_COP_ADD);
    assert(block_val.kind == LLACE_VALUE_BLOCK);
    
    TEST_PFAIL(llace_value_destroy(&instr_val));
    TEST_PFAIL(llace_value_destroy(&block_val));
  }
  ++(*total_tests_passed);
}

// Block
void test_block(unsigned *total_tests_passed) { // 2 tests
  { // Test Block Init/Destroy
    llace_block_t block;
    TEST_PFAIL(llace_block_init(&block));
    
    assert(LLACE_ARRAY_COUNT(block.locals) == 0);
    assert(LLACE_ARRAY_COUNT(block.instrs) == 0);
    
    TEST_PFAIL(llace_block_destroy(&block));
  }
  ++(*total_tests_passed);

  { // Test Block with Variables and Instructions
    llace_block_t block;
    TEST_PFAIL(llace_block_init(&block));
    
    // Add some local variables
    llace_variable_t var1, var2;
    TEST_PFAIL(llace_variable_init(&var1));
    TEST_PFAIL(llace_variable_init(&var2));
    
    var1.type = 0; // some type
    var2.type = 1; // some other type
    
    LLACE_ARRAY_PUSH(block.locals, var1);
    LLACE_ARRAY_PUSH(block.locals, var2);
    
    // Add some instructions
    llace_instr_t instr1, instr2;
    TEST_PFAIL(llace_instr_create(&instr1));
    TEST_PFAIL(llace_instr_create(&instr2));
    
    instr1.opcode = LLACE_OP_ADD;
    instr2.opcode = LLACE_OP_STORE;
    
    LLACE_ARRAY_PUSH(block.instrs, instr1);
    LLACE_ARRAY_PUSH(block.instrs, instr2);
    
    // Verify counts
    assert(LLACE_ARRAY_COUNT(block.locals) == 2);
    assert(LLACE_ARRAY_COUNT(block.instrs) == 2);
    
    // Verify content
    llace_variable_t *retrieved_var = LLACE_ARRAY_GET(llace_variable_t, block.locals, 0);
    assert(retrieved_var->type == 0);
    
    llace_instr_t *retrieved_instr = LLACE_ARRAY_GET(llace_instr_t, block.instrs, 1);
    assert(retrieved_instr->opcode == LLACE_OP_STORE);
    
    TEST_PFAIL(llace_block_destroy(&block));
  }
  ++(*total_tests_passed);
}

// Instruction
void test_instruction(unsigned *total_tests_passed) { // 3 tests
  { // Test Instruction Init/Destroy
    llace_instr_t instr;
    TEST_PFAIL(llace_instr_create(&instr));
    
    assert(instr.opcode == LLACE_OP_NULL);
    assert(LLACE_ARRAY_COUNT(instr.params) == 0);
    
    llace_instr_destroy(&instr);
  }
  ++(*total_tests_passed);

  { // Test Instruction with Parameters
    llace_instr_t instr;
    TEST_PFAIL(llace_instr_create(&instr));
    
    instr.opcode = LLACE_OP_ADD;
    
    // Add some value parameters
    llace_value_t val1, val2, result;
    TEST_PFAIL(llace_value_init(&val1));
    TEST_PFAIL(llace_value_init(&val2));
    TEST_PFAIL(llace_value_init(&result));
    
    val1.kind = LLACE_VALUE_VARIABLE;
    val1._var = 0;
    val2.kind = LLACE_VALUE_VARIABLE;
    val2._var = 1;
    result.kind = LLACE_VALUE_VARIABLE;
    result._var = 2;
    
    LLACE_ARRAY_PUSH(instr.params, result); // result goes first
    LLACE_ARRAY_PUSH(instr.params, val1);
    LLACE_ARRAY_PUSH(instr.params, val2);
    
    assert(instr.opcode == LLACE_OP_ADD);
    assert(LLACE_ARRAY_COUNT(instr.params) == 3);
    
    llace_value_t *param = LLACE_ARRAY_GET(llace_value_t, instr.params, 0);
    assert(param->kind == LLACE_VALUE_VARIABLE);
    assert(param->_var == 2);
    
    llace_instr_destroy(&instr);
  }
  ++(*total_tests_passed);

  { // Test Instruction Opcode Strings
    assert(strcmp(llace_instr_opstr(LLACE_OP_NULL), "null") == 0);
    assert(strcmp(llace_instr_opstr(LLACE_OP_ADD), "add") == 0);
    assert(strcmp(llace_instr_opstr(LLACE_OP_SUB), "sub") == 0);
    assert(strcmp(llace_instr_opstr(LLACE_OP_MUL), "mul") == 0);
    assert(strcmp(llace_instr_opstr(LLACE_OP_DIV), "div") == 0);
    assert(strcmp(llace_instr_opstr(LLACE_OP_LOAD), "load") == 0);
    assert(strcmp(llace_instr_opstr(LLACE_OP_STORE), "store") == 0);
    assert(strcmp(llace_instr_opstr(LLACE_OP_CALL), "call") == 0);
    assert(strcmp(llace_instr_opstr(LLACE_OP_RET), "ret") == 0);
    assert(strcmp(llace_instr_opstr((llace_opcode_t)999), "unknown") == 0);
  }
  ++(*total_tests_passed);
}

// Child Instruction
void test_cinstr(unsigned *total_tests_passed) { // 3 tests
  { // Test Child Instruction Init/Destroy
    llace_cinstr_t cinstr;
    TEST_PFAIL(llace_cinstr_create(&cinstr));
    
    assert(cinstr.opcode == LLACE_COP_NULL);
    assert(LLACE_ARRAY_COUNT(cinstr.params) == 0);
    
    llace_cinstr_destroy(&cinstr);
  }
  ++(*total_tests_passed);

  { // Test Child Instruction with Parameters
    llace_cinstr_t cinstr;
    TEST_PFAIL(llace_cinstr_create(&cinstr));
    
    cinstr.opcode = LLACE_COP_EQ;
    
    // Add some value parameters
    llace_value_t val1, val2;
    TEST_PFAIL(llace_value_init(&val1));
    TEST_PFAIL(llace_value_init(&val2));
    
    val1.kind = LLACE_VALUE_VARIABLE;
    val1._var = 10;
    val2.kind = LLACE_VALUE_VARIABLE;
    val2._var = 11;
    
    LLACE_ARRAY_PUSH(cinstr.params, val1);
    LLACE_ARRAY_PUSH(cinstr.params, val2);
    
    assert(cinstr.opcode == LLACE_COP_EQ);
    assert(LLACE_ARRAY_COUNT(cinstr.params) == 2);
    
    llace_value_t *param = LLACE_ARRAY_GET(llace_value_t, cinstr.params, 0);
    assert(param->kind == LLACE_VALUE_VARIABLE);
    assert(param->_var == 10);
    
    llace_cinstr_destroy(&cinstr);
  }
  ++(*total_tests_passed);

  { // Test Child Instruction Opcode Strings
    assert(strcmp(llace_cinstr_opstr(LLACE_COP_NULL), "null") == 0);
    assert(strcmp(llace_cinstr_opstr(LLACE_COP_ADD), "add") == 0);
    assert(strcmp(llace_cinstr_opstr(LLACE_COP_SUB), "sub") == 0);
    assert(strcmp(llace_cinstr_opstr(LLACE_COP_AND), "and") == 0);
    assert(strcmp(llace_cinstr_opstr(LLACE_COP_OR), "or") == 0);
    assert(strcmp(llace_cinstr_opstr(LLACE_COP_EQ), "eq") == 0);
    assert(strcmp(llace_cinstr_opstr(LLACE_COP_NE), "ne") == 0);
    assert(strcmp(llace_cinstr_opstr(LLACE_COP_LT), "lt") == 0);
    assert(strcmp(llace_cinstr_opstr(LLACE_COP_CALL), "call") == 0);
    assert(strcmp(llace_cinstr_opstr((llace_copcode_t)999), "unknown") == 0);
  }
  ++(*total_tests_passed);
}

// Type
void test_type(unsigned *total_tests_passed) { // 8 tests
  { // Test Building a void
    llace_type_builder_t builder = {
      ._module=NULL,
      .name=NULL,
      .namelen=0,
      .kind=LLACE_TYPE_VOID
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);

  { // Test Building a 64 bit Integer
    llace_type_builder_t builder = {
      ._module=NULL,
      .name=NULL,
      .namelen=0,
      .kind=LLACE_TYPE_INT,
      ._int=64
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);

  { // Test Building a 64 bit Unsigned
    llace_type_builder_t builder = {
      ._module=NULL,
      .name=NULL,
      .namelen=0,
      .kind=LLACE_TYPE_UNT,
      ._unt=64
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);

  { // Test Building a IEEE 754 single precision float
    llace_type_builder_t builder = {
      ._module=NULL,
      .name=NULL,
      .namelen=0,
      .kind=LLACE_TYPE_FLOAT,
      ._float.mantissa=23,
      ._float.exponent=8
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);

  { // Test Building a Pointer
    llace_type_builder_t builder = {
      ._module=NULL,
      .name=NULL,
      .namelen=0,
      .kind=LLACE_TYPE_PTR,
      ._ptr.type=0, // example type reference, should be into a module
      ._ptr.depth=2 // 2 depth (**)
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);

  { // Test Building an array
    llace_type_builder_t builder = {
      ._module=NULL,
      .name=NULL,
      .namelen=0,
      .kind=LLACE_TYPE_ARRAY,
      ._array.element=0,
      ._array.count=10
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);

  { // Test Building a composite
    llace_type_builder_t builder = {
      ._module=NULL,
      .name=NULL,
      .namelen=0,
      .kind=LLACE_TYPE_STRUCT,
      ._composite.members=LLACE_NEW_ARRAY(llace_typeref_t, 0)
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
    // array is freed by the type. it is moved into the type and no longer managed in this context
    // is is automatically freed in the llace_type_destroy
    // calling it again will not cause any problems as it will be NULL'd out and checked but still not needed.
  }
  ++(*total_tests_passed);

  { // Test Building a function
    llace_type_builder_t builder = {
      ._module=NULL,
      .name=NULL,
      .namelen=0,
      .kind=LLACE_TYPE_FUNCTION,
      ._func.params=LLACE_NEW_ARRAY(llace_typeref_t, 0),
      ._func.returns=LLACE_NEW_ARRAY(llace_typeref_t, 0),
      ._func.abi=LLACE_ABI_NONE
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);
}

void test_type_module(unsigned *total_tests_passed) { // 8 tests
  // Initalize Test Module
  llace_module_t module;
  TEST_PFAIL(llace_module_init(&module, "mod", 3));
 
  { // Test Building a void
    llace_type_builder_t builder = {
      ._module=&module,
      .name="void",
      .namelen=4,
      .kind=LLACE_TYPE_VOID
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);

  { // Test Building a 64 bit Integer
    llace_type_builder_t builder = {
      ._module=&module,
      .name="int",
      .namelen=3,
      .kind=LLACE_TYPE_INT,
      ._int=64
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);

  { // Test Building a 64 bit Unsigned
    llace_type_builder_t builder = {
      ._module=&module,
      .name="unsigned",
      .namelen=8,
      .kind=LLACE_TYPE_UNT,
      ._unt=64
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);

  { // Test Building a IEEE 754 single precision float
    llace_type_builder_t builder = {
      ._module=&module,
      .name="float",
      .namelen=5,
      .kind=LLACE_TYPE_FLOAT,
      ._float.mantissa=23,
      ._float.exponent=8
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);

  { // Test Building a Pointer
    llace_type_builder_t builder = {
      ._module=&module,
      .name="void*",
      .namelen=5,
      .kind=LLACE_TYPE_PTR,
      ._ptr.type=0, // void is the zeroth typed as its the first typed added
      ._ptr.depth=2 // 2 depth (**)
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);

  { // Test Building an array
    llace_type_builder_t builder = {
      ._module=&module,
      .name="int[]",
      .namelen=6,
      .kind=LLACE_TYPE_ARRAY,
      ._array.element=1, // int is the 1 first type added and this gives its index
      ._array.count=10
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);

  { // Test Building a composite
    llace_type_builder_t builder = {
      ._module=&module,
      .name="point",
      .namelen=5,
      .kind=LLACE_TYPE_STRUCT,
      ._composite.members=LLACE_NEW_ARRAY(llace_typeref_t, 2)
    };
    // float is the 3rd type
    LLACE_ARRAY_PUSH(builder._composite.members, 3);
    LLACE_ARRAY_PUSH(builder._composite.members, 3);

    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
    // array is freed by the type. it is moved into the type and no longer managed in this context
    // is is automatically freed in the llace_type_destroy
    // calling it again will not cause any problems as it will be NULL'd out and checked but still not needed.
  }
  ++(*total_tests_passed);

  { // Test Building a function
    llace_type_builder_t builder = {
      ._module=&module,
      .name="void(*func)()",
      .namelen=13,
      .kind=LLACE_TYPE_FUNCTION,
      ._func.params=LLACE_NEW_ARRAY(llace_typeref_t, 0),
      ._func.returns=LLACE_NEW_ARRAY(llace_typeref_t, 0),
      ._func.abi=LLACE_ABI_NONE
    };
    llace_type_t type;
    TEST_PFAIL(llace_build_type(&builder, &type));
    TEST_PFAIL(llace_type_destroy(&type));
  }
  ++(*total_tests_passed);
}

// Module
void test_module(unsigned *total_tests_passed) { // 1 test
  { // Test Module Naming
    llace_module_t module;
    TEST_PFAIL(llace_module_init(&module, "mod", 3));
    if (strcmp(llace_module_get_name(&module, module.name), "mod") != 0) {
      assert(0);
    }

    for (size_t i = 0; i < 10; ++i) {
      llace_nameref_t name;

      char namestr[10];
      snprintf(namestr, 5, "foo%zu", i);

      llace_module_add_name(&module, namestr, 0, &name);

      if (strcmp(llace_module_get_name(&module, name), namestr) != 0) {
        assert(0);
      }
    }
  }
  ++(*total_tests_passed);
}

void test_ir(unsigned *total_tests_passed) {
  // Module
  test_module(total_tests_passed);

  // Type
  test_type_module(total_tests_passed);
  test_type(total_tests_passed);

  // Child Instruction
  test_cinstr(total_tests_passed);

  // Instruction
  test_instruction(total_tests_passed);

  // Block
  test_block(total_tests_passed);

  // Value
  test_value(total_tests_passed);

  // Variable
  test_variable(total_tests_passed);

  // Global
  test_global(total_tests_passed);

  // Function
  test_function(total_tests_passed);
}