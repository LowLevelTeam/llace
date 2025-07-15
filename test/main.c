#include <stdio.h>

extern void test_iir_basic(void);
extern void test_iir_debugging(void);
extern void test_iir_const_eval(void);

int main(void) {
  printf("LLACE IIR (Interpreted Intermediate Representation) Tests\n");
  printf("========================================================\n\n");
  
  // Run tests
  test_iir_basic();
  test_iir_debugging();
  test_iir_const_eval();
  
  printf("All IIR tests completed successfully!\n");
  
  return 0;
}