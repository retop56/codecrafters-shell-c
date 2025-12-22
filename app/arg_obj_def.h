#pragma once

#include <stddef.h>
#include <stdbool.h>
struct arg_obj {
  size_t size;
  size_t capacity;
  char **args;
  char * input;
  char * curr_char;
  bool redirect_to_stdout;
};
