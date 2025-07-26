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

    if (person_handle.data != NULL) {
      person_t *person = LLACE_GET(person_t, person_handle);
      person->id = 1;
      strcpy(person->name, "John Doe");
      person->value = 3.14f;

      if (person->id == 1 && strcmp(person->name, "John Doe") == 0 && person->value == 3.14f) {
        ++(*total_tests_passed);
      } else {
        LLACE_LOG_ERROR("Memory item test failed: id=%d, name='%s', value=%f", 
                        person->id, person->name, person->value);
      }
    } else {
      LLACE_LOG_ERROR("Memory item test failed: allocation returned NULL");
    }

    LLACE_FREE(person_handle);
  }

  {
    llace_array_t array_handle = LLACE_NEW_ARRAY(person_t, 16);

    if (array_handle.data != NULL) {
      for (size_t i = 0; i < 16; ++i) {
        person_t person = { .id=i, .value=3.14 };
        strcpy(person.name, "John Doe");
        LLACE_ARRAY_PUSH(array_handle, person);
      }

      bool all_correct = true;
      LLACE_ARRAY_FOREACH(person_t, item, array_handle) {
        if (strcmp(item->name, "John Doe") != 0) {
          LLACE_LOG_ERROR("Memory array item name mismatch: id=%d, name='%s'", item->id, item->name);
          all_correct = false;
        } else if (item->value != 3.14f) {
          LLACE_LOG_ERROR("Memory array item value mismatch: id=%d, value=%f", item->id, item->value);
          all_correct = false;
        }
      }

      if (LLACE_ARRAY_COUNT(array_handle) == 16 && all_correct) {
        ++(*total_tests_passed);
      } else {
        LLACE_LOG_ERROR("Memory array test failed: count=%zu, all_correct=%d", 
                        LLACE_ARRAY_COUNT(array_handle), all_correct);
      }
    } else {
      LLACE_LOG_ERROR("Memory array test failed: allocation returned NULL");
    }

    LLACE_FREE_ARRAY(array_handle);
  }
}