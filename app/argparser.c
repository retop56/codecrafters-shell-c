#include "cc_shell.h"
#include "arg_obj_def.h"
#include <stdlib.h>
#include <string.h>
#include "argparser.h"

struct arg_obj * create_arg_obj() {
  struct arg_obj *ao = malloc(sizeof(struct arg_obj));
  ao->size = 0;
  ao-> limit = 10;
  char **initial_args = (char **)malloc(sizeof(char*) * 10);
  if (initial_args == NULL) {
    fprintf(stderr, "Failed to create arg_obj!");
    exit(EXIT_FAILURE);
  } 
  ao->args = initial_args;
  return ao;
};

void expand_arg_obj(struct arg_obj * ao) {
  ao->limit = ao->limit * 2;
  char ** new_args = realloc(ao->args, ao->limit * 2);
  if (new_args == NULL) {
    fprintf(stderr, "Failed to rellocate arg_obj args size!");
    exit(EXIT_FAILURE);
  }
}

void add_args(struct arg_obj * ao, char * input) {
  if (input == NULL || ao == NULL) {
    return;
  }
  while (*input != '\0') {
    /*
     * Check to see if arg_obj has enough space for next arg. If it doesn't,
     * expand the size of arg_obj->args array
     */
    if ((ao->size + 1) >= ao->limit) {
      expand_arg_obj(ao);
    }
    if (strncmp(input, "\'", 1) == 0) {
      input = get_single_quote_arg(ao, input);
    } else if (strncmp(input, "\"", 1) == 0) {
      printf("Found argument that starts with a double quote!\n");
      exit(EXIT_FAILURE);
    } else {
      input = get_normal_arg(ao, input);
    }
  }
}

static char * get_normal_arg(struct arg_obj * ao, char * input) {
  size_t count = 0;
  char * curr_char = input;
  while (*curr_char != '\0' && *curr_char != ' ') {
    count++;
    curr_char++;
  }
  while (*curr_char == ' ') {
    curr_char++;
  }
  char * new_arg = strndup(input, count);
  if (new_arg == NULL) {
    fprintf(stderr, "strndup failed at line %d in %s\n", (__LINE__) - 2, __FUNCTION__);
  }
  ao->args[ao->size++] = new_arg;
  return curr_char;
}

static char * get_single_quote_arg(struct arg_obj * ao, char * input) {
  input++; // Skip past first single quote
  size_t count = 0;
  char * curr_char = input;
  while (*curr_char != '\0' && *curr_char != '\'') {
    count++;
    curr_char++;
  }
  if (*curr_char == '\'') {
    curr_char++;
  }
  char * new_arg = strndup(input, count);
  if (new_arg == NULL) {
    fprintf(stderr, "strndup failed at line %d in %s\n", (__LINE__) - 2, __FUNCTION__);
  }
  ao->args[ao->size++] = new_arg;
  return curr_char;
}

void clear_args(struct arg_obj * ao) {
  for (size_t i = 0; i < ao->size; i++) {
    free(ao->args[i]);
  }
  ao->size = 0;
}
