#include <llace/mem.h>
#include <string.h>

typedef struct {
  int id;
  char name[32];
  float value;
} person_t;

void test_mem(unsigned *total_tests_passed) { // 2 tests
  {
    llace_item_t person_handle = LLACE_NEW(person_t);

    person_t *person = LLACE_GET(person_t, person_handle);
    person->id = 1;
    strcpy(person->name, "John Doe");
    person->value = 3.14f;

    LLACE_FREE(person_handle);
  }
  ++(*total_tests_passed);

  {
    llace_array_t array_handle = LLACE_NEW_ARRAY(person_t, 16);

    for (size_t i = 0; i < 16; ++i) {
      person_t person = { .id=i, .value=3.14 };
      strcpy(person.name, "John Doe");
      LLACE_ARRAY_PUSH(array_handle, person);
    }

    LLACE_ARRAY_FOREACH(person_t, item, array_handle) {
      // item values recieved
    }

    LLACE_FREE_ARRAY(array_handle);
  }
  ++(*total_tests_passed);
}
