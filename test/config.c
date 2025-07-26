#include <llace/config.h>
#include <stdbool.h>

void test_config(unsigned *total_tests_passed) { // 2 test
  { // Target Test
    llace_target_t target;
    llace_target_get_host(&target);
    llace_target_log(&target);
  } ++(*total_tests_passed);

  { // Config Test
    llace_config_t conf;
    llace_config_init(&conf);
    if (llace_config_valid(&conf)) {
      ++(*total_tests_passed);
    } else { 
      LLACE_LOG_ERROR("Config is not valid");
    }
  } 
  
}
