#pragma once

#include <stddef.h>
#include <stdbool.h>
struct arg_obj {
  size_t size;
  size_t capacity;
  char **args;
  char * input;
  char * curr_char;

  enum redir_t {STD_OUT, STD_ERR, INITIAL_VAL} redir_type;
};
