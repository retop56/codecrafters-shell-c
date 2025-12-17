#include "cc_shell.h"
#include "handle_commands.h"
extern char * curr_tok;

struct arg_obj create_arg_obj() {
  struct arg_obj *ao = malloc(sizeof(struct arg_obj));
  ao->size = 0;
  ao-> limit = 10;
  char **initial_args = (char **)malloc(sizeof(char*) * 10);
  if (initial_args == NULL) {
    fprintf(stderr, "Failed to create arg_obj!");
    exit(EXIT_FAILURE);
  } 
  ao->args = initial_args;
  return *ao;
};

void add_args(struct arg_obj *ao) {
  size_t ct_len = strlen(curr_tok);
  char *arg = (char*) malloc((sizeof(char) * ct_len) + 1);
  strcat(arg, curr_tok);
  if ((ao->size + 1) < ao->limit) {
    ao->args[ao->size++] = arg;
  } else {
    ao->limit = ao->limit * 2;
    char ** new_args = realloc(ao->args, ao->limit * 2);
    if (new_args == NULL) {
      fprintf(stderr, "Failed to rellocate arg_obj args size!");
      exit(EXIT_FAILURE);
    }
    ao->args[ao->size++] = arg; 
  }
}

void clear_args(struct arg_obj *ao) {
  for (size_t i = 0; i < ao->size; i++) {
    ao->args[i] = NULL;
  }
  ao->size = 0;
}
