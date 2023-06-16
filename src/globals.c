#include "globals.h"

// Initialize the global variables
size_t TEXT_SIZE = 0;
size_t DATA_SIZE = 0;
size_t TEXT_BEG = 0;
size_t TEXT_END = 0;
size_t DATA_BEG = 0;
size_t DATA_END = 0;

void free_2d(char **array, size_t len) {
  for (size_t i = 0; i < len; i++)
    free(array[i]);
  free(array);
}

size_t get_index(const char **array, size_t size, const char *element) {
  for (size_t i = 0; i < size; i++) {
    if (!strcmp(array[i], element))
      return i;
  }
  return -1; // Element not found
}