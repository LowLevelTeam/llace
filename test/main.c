#include <llace/llace.h>

extern void test_config(unsigned*);
extern void test_mem(unsigned*);

int main(void) {
  LLACE_LOG_INFO("LLACE (Low Level Assembly & Compilation Engine) Tests");
  LLACE_LOG_INFO("========================================================");
  
  unsigned total_tests =
    2+  // memory
    2+  // config
    0
  ;
  unsigned total_tests_passed = 0;

  LLACE_LOG_INFO("Running memory tests...");
  test_mem(&total_tests_passed);
  
  LLACE_LOG_INFO("Running configuration tests...");
  test_config(&total_tests_passed);

  LLACE_LOG_INFO("========================================================");
  if (total_tests == total_tests_passed) {
    LLACE_LOG_INFO("All %u tests completed successfully!", total_tests_passed);
    return 0;
  } else {
    LLACE_LOG_ERROR("Tests failed: %u/%u passed", total_tests_passed, total_tests);
    return 1;
  }
}