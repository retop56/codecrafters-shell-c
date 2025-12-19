#include "cc_shell.h"
#include "arg_obj_def.h"
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

void expand_arg_obj(struct arg_obj *ao) {
  ao->limit = ao->limit * 2;
  char ** new_args = realloc(ao->args, ao->limit * 2);
  if (new_args == NULL) {
    fprintf(stderr, "Failed to rellocate arg_obj args size!");
    exit(EXIT_FAILURE);
  }
}

void add_args(struct arg_obj *ao, char *input) {
  char * tok;
  // 1. Call strtok on input
  tok = strtok(input, " ");
  char * new_arg;
  // 2. Create loop that will continue to run as long as tok is not null AKA there's another
  //    token left
  while (tok != NULL) {
    // 3. If it's successful, first check if there's enough space for this argument.
    //    If there isn't, expand arg_obj first
    if ((ao->size + 1) >= ao->limit) {
      expand_arg_obj(ao);
    }
    new_arg = strdup(tok);
    if (new_arg == NULL) {
      fprintf(stderr, "Failed to duplicate token before adding to arg_obj!");
      exit(EXIT_FAILURE);
    }
    // 4. Add new_arg to arg_obj array
    ao->args[ao->size++] = new_arg;
    tok = strtok(NULL, " "); // Get next token
  }
}

void clear_args(struct arg_obj *ao) {
  for (size_t i = 0; i < ao->size; i++) {
    free(ao->args[i]);
  }
  ao->size = 0;
}
