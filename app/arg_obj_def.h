#pragma once

#include <stddef.h>
struct arg_obj {
  size_t size;
  size_t limit;
  char **args;
  char * input;
};
