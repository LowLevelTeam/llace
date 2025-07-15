#include <llace/llace.h>

extern void test_config(unsigned*);
extern void test_mem(unsigned*);
extern void test_ir(unsigned*);

int main(void) {
  LLACE_LOG_INFO("LLACE (Low Level Assembly & Compilation Engine) Tests");
  LLACE_LOG_INFO("========================================================");
  
  unsigned total_tests =
    2+  // memory
    2+  // config
    1+  // IR Module
    16+ // IR Type
    0+  // IR Child Instruction
    0+  // IR Instruction
    0+  // IR Block
    0+  // IR Value
    0+  // IR Variable
    0+  // IR Global
    0+  // IR Function
    0
  ;
  unsigned total_tests_passed = 0;

  test_mem(&total_tests_passed);
  test_config(&total_tests_passed);
  test_ir(&total_tests_passed);

  if (total_tests == total_tests_passed) {
    LLACE_LOG_INFO("All %zu tests completed successfully!", total_tests_passed);
  } else {
    LLACE_LOG_INFO("Total tests Failed %u", total_tests - total_tests_passed);
  }

  return 0;
}