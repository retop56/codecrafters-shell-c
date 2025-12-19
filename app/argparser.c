#include "argparser.h"
#include "arg_obj_def.h"
#include "cc_shell.h"
#include <stdlib.h>
#include <string.h>

int times_double_quotes_called = 0;

struct arg_obj *create_arg_obj() {
  struct arg_obj *ao = malloc(sizeof(struct arg_obj));
  ao->size = 0;
  ao->limit = 10;
  char **initial_args = (char **)malloc(sizeof(char *) * 10);
  if (initial_args == NULL) {
    fprintf(stderr, "Failed to create arg_obj!");
    exit(EXIT_FAILURE);
  }
  ao->args = initial_args;
  ao->input = NULL;
  return ao;
};

void expand_arg_obj(struct arg_obj *ao) {
  ao->limit = ao->limit * 2;
  char **new_args = realloc(ao->args, ao->limit * 2);
  if (new_args == NULL) {
    fprintf(stderr, "Failed to rellocate arg_obj args size!");
    exit(EXIT_FAILURE);
  }
}

void add_args(struct arg_obj *ao) {
  char *received_arg;
  while (*ao->input != '\0') {
    /*
     * Check to see if arg_obj has enough space for next arg. If it doesn't,
     * expand the size of arg_obj->args array
     */
    if ((ao->size + 1) >= ao->limit) {
      expand_arg_obj(ao);
    }
    if (strncmp(ao->input, "\'", 1) == 0) {
      received_arg = get_single_quote_arg(ao);
      if (strncmp(ao->input, "\'", 1) == 0) {
        char *second_arg = get_single_quote_arg(ao);
        size_t combined_size = strlen(received_arg) + strlen(second_arg) + 1;
        received_arg = (char *)realloc(received_arg, combined_size);
        strcat(received_arg, second_arg);
      }
      while (*ao->input == ' ') {
        ao->input++;
      }
      ao->args[ao->size++] = received_arg;
    } else if (strncmp(ao->input, "\"", 1) == 0) {
      received_arg = get_double_quote_arg(ao);
      if (strncmp(ao->input, "\"", 1) == 0) {
        char *second_arg = get_double_quote_arg(ao);
        size_t combined_size = strlen(received_arg) + strlen(second_arg) + 1;
        received_arg = (char *)realloc(received_arg, combined_size);
        strcat(received_arg, second_arg);
      }
      while (*ao->input == ' ') {
        ao->input++;
      }
      ao->args[ao->size++] = received_arg;
    } else {
      received_arg = get_normal_arg(ao);
      if (strncmp(ao->input, "\'\'", 2) == 0) {
        ao->input++;
        char *second_arg = get_single_quote_arg(ao);
        size_t combined_size = strlen(received_arg) + strlen(second_arg) + 1;
        received_arg = (char *)realloc(received_arg, combined_size);
        strcat(received_arg, second_arg);
      } else if (strncmp(ao->input, "\"\"", 2) == 0) {
        ao->input++;
        char *second_arg = get_double_quote_arg(ao);
        size_t combined_size = strlen(received_arg) + strlen(second_arg) + 1;
        received_arg = (char *)realloc(received_arg, combined_size);
        strcat(received_arg, second_arg);
      }
      while (*ao->input == ' ') {
        ao->input++;
      }
      ao->args[ao->size++] = received_arg;
    }
  }
}

static char *get_normal_arg(struct arg_obj *ao) {
  size_t count = 0;
  char *curr_char = ao->input;
  while (*curr_char != '\0' && *curr_char != ' ' && *curr_char != '\'' &&
         *curr_char != '\"') {
    count++;
    curr_char++;
  }
  char *new_arg = strndup(ao->input, count);
  if (new_arg == NULL) {
    fprintf(stderr, "strndup failed at line %d in %s\n", (__LINE__)-2,
            __FUNCTION__);
  }
  ao->input = curr_char;
  return new_arg;
}

static char *get_single_quote_arg(struct arg_obj *ao) {
  ao->input++; // Skip past first single quote
  size_t count = 0;
  char *curr_char = ao->input;
  while (*curr_char != '\0' && *curr_char != '\'') {
    count++;
    curr_char++;
  }
  if (*curr_char == '\'') { // Skip past second single quote
    curr_char++;
  }
  char *new_arg = strndup(ao->input, count);
  if (new_arg == NULL) {
    fprintf(stderr, "strndup failed at line %d in %s\n", (__LINE__)-2,
            __FUNCTION__);
  }
  ao->input = curr_char;
  return new_arg;
}

static char *get_double_quote_arg(struct arg_obj *ao) {
  /*times_double_quotes_called++;*/
  ao->input++; // Skip past first double quote
  size_t count = 0;
  char *curr_char = ao->input;
  while (*curr_char != '\0' && *curr_char != '\"') {
    count++;
    curr_char++;
  }
  if (*curr_char == '\"') {
    curr_char++;
  }
  char *new_arg = strndup(ao->input, count);
  if (new_arg == NULL) {
    fprintf(stderr, "strndup failed at line %d in %s\n", (__LINE__)-2,
            __FUNCTION__);
  }
  ao->input = curr_char;
  return new_arg;
}

void clear_args(struct arg_obj *ao) {
  for (size_t i = 0; i < ao->size; i++) {
    free(ao->args[i]);
  }
  ao->size = 0;
}
