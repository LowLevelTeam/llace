#include <llace/ir.h>
#include <assert.h>
#include <string.h>

#define TEST_PFAIL(func) do { llace_error_t err = func; if (err != LLACE_ERROR_NONE) { LLACE_LOG_INFO("error %s", llace_error_str(err)); assert(err != LLACE_ERROR_NONE); } } while (0);

// Function
void test_function(unsigned *total_tests_passed) {}

// Global
void test_global(unsigned *total_tests_passed) {}

// Variable
void test_variable(unsigned *total_tests_passed) {}

// Value
void test_value(unsigned *total_tests_passed) {
  


}

// Block
void test_block(unsigned *total_tests_passed) {}

// Instruction
void test_instruction(unsigned *total_tests_passed) {

}

// Child Instruction
void test_cinstr(unsigned *total_tests_passed) {
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
