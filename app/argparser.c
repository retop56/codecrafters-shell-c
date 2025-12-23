#include "argparser.h"
#include "arg_obj_def.h"
#include "cc_shell.h"
#include <stdlib.h>
#include <string.h>

static char *get_normal_arg(struct arg_obj *ao);
static char *get_single_quote_arg(struct arg_obj *ao);
static char *get_double_quote_arg(struct arg_obj *ao);
static char *skip_past_adjacent_quotes_and_combine(struct arg_obj *ao, char *first_arg, char type_of_quote); 
static char handle_backslash_char(struct arg_obj *ao, BKSLSH_MODE bm);

int times_double_quotes_called = 0;
char curr_arg[BUFF_LENGTH];

struct arg_obj *create_arg_obj() {
  struct arg_obj *ao = malloc(sizeof(struct arg_obj));
  ao->size = 0;
  ao->capacity = 10;
  char **initial_args = (char **)malloc(sizeof(char *) * 10);
  if (initial_args == NULL) {
    fprintf(stderr, "Failed to create arg_obj!");
    exit(EXIT_FAILURE);
  }
  ao->args = initial_args;
  ao->input = NULL;
  ao->redir_type = INITIAL_VAL;
  return ao;
};

void expand_arg_obj(struct arg_obj *ao) {
  ao->capacity = ao->capacity * 2;
  char **new_args = realloc(ao->args, ao->capacity * 2);
  if (new_args == NULL) {
    fprintf(stderr, "Failed to rellocate arg_obj args size!");
    exit(EXIT_FAILURE);
  }
}

void add_args(struct arg_obj *ao) {
  char *received_arg;
  while (*ao->curr_char != '\0') {
    /*
     * Check to see if arg_obj has enough space for next arg. If it doesn't,
     * expand the size of arg_obj->args array
     */
    if ((ao->size + 1) >= ao->capacity) {
      expand_arg_obj(ao);
    }
    if (strncmp(ao->curr_char, "\'", 1) == 0) {
      received_arg = get_single_quote_arg(ao);
      if (strncmp(ao->curr_char, "\'", 1) == 0) {
        received_arg =
            skip_past_adjacent_quotes_and_combine(ao, received_arg, '\'');
      }
      while (*ao->curr_char == ' ') {
        ao->curr_char++;
      }
      ao->args[ao->size++] = received_arg;
    } else if (strncmp(ao->curr_char, "\"", 1) == 0) {
      received_arg = get_double_quote_arg(ao);
      if (strncmp(ao->curr_char, "\"", 1) == 0) {
        received_arg =
            skip_past_adjacent_quotes_and_combine(ao, received_arg, '\"');
      }
      while (*ao->curr_char == ' ') {
        ao->curr_char++;
      }
      ao->args[ao->size++] = received_arg;
    } else {
      received_arg = get_normal_arg(ao);
      if (strncmp(ao->curr_char, "\'\'", 2) == 0) {
        ao->curr_char++;
        received_arg =
            skip_past_adjacent_quotes_and_combine(ao, received_arg, '\'');
      } else if (strncmp(ao->curr_char, "\"\"", 2) == 0) {
        ao->curr_char++;
        received_arg =
            skip_past_adjacent_quotes_and_combine(ao, received_arg, '\"');
      }
      while (*ao->curr_char == ' ') {
        ao->curr_char++;
      }
      if (strncmp(received_arg, ">", 1) == 0) {
        ao->redir_type = STD_OUT;
      }
      if (strncmp(received_arg, "1>", 1) == 0) {
        ao->redir_type = STD_OUT;
      }
      ao->args[ao->size++] = received_arg;
    }
  }
}

static char *skip_past_adjacent_quotes_and_combine(struct arg_obj *ao,
                                                   char *first_arg,
                                                   char type_of_quote) {
  if (type_of_quote == '\"') {
    char *second_arg = get_double_quote_arg(ao);
    size_t combined_size = strlen(first_arg) + strlen(second_arg) + 1;
    first_arg = (char *)realloc(first_arg, combined_size);
    strcat(first_arg, second_arg);
    return first_arg;
  } else if (type_of_quote == '\'') {
    char *second_arg = get_single_quote_arg(ao);
    size_t combined_size = strlen(first_arg) + strlen(second_arg) + 1;
    first_arg = (char *)realloc(first_arg, combined_size);
    strcat(first_arg, second_arg);
    return first_arg;
  }
  return NULL;
}

static char *get_normal_arg(struct arg_obj *ao) {
  size_t count = 0;
  while (*ao->curr_char != '\0' && *ao->curr_char != ' ' &&
         *ao->curr_char != '\'' && *ao->curr_char != '\"') {
    if (*ao->curr_char == '\\') {
      curr_arg[count++] = handle_backslash_char(ao, OUTSIDE_QUOTES);
    } else {
      curr_arg[count++] = *ao->curr_char++;
    }
  }
  curr_arg[count] = '\0';
  char *new_arg = strndup(curr_arg, count);
  if (new_arg == NULL) {
    fprintf(stderr, "strndup failed at line %d in %s\n", (__LINE__)-2,
            __FUNCTION__);
  }
  return new_arg;
}

static char *get_single_quote_arg(struct arg_obj *ao) {
  ao->curr_char++; // Skip past first single quote
  size_t count = 0;
  while (*ao->curr_char != '\0' && *ao->curr_char != '\'') {
    curr_arg[count++] = *ao->curr_char++;
  }
  curr_arg[count] = '\0';
  if (*ao->curr_char == '\'') { // Skip past second single quote
    ao->curr_char++;
  }
  char *new_arg = strndup(curr_arg, count);
  if (new_arg == NULL) {
    fprintf(stderr, "strndup failed at line %d in %s\n", (__LINE__)-2,
            __FUNCTION__);
  }
  return new_arg;
}

static char handle_backslash_char(struct arg_obj *ao, BKSLSH_MODE bm) {
  if (*ao->curr_char != '\\') {
    fprintf(stderr, "How the fuck did you get here? %s Line %d\n", __FUNCTION__,
            __LINE__);
    exit(EXIT_FAILURE);
  }
  switch (bm) {
  case OUTSIDE_QUOTES:
    ao->curr_char++;
    switch (*ao->curr_char) {
    case ' ':
      ao->curr_char++;
      return ' ';
    case '\'':
      ao->curr_char++;
      return '\'';
    case '\"':
      ao->curr_char++;
      return '\"';
    case 'n':
      ao->curr_char++;
      return 'n';
    case '\\':
      ao->curr_char++;
      return '\\';
    default:
      fprintf(stderr, "Failed switch block in %s\n", __FUNCTION__);
      exit(EXIT_FAILURE);
    }
  case INSIDE_DOUBLE_QUOTES:
    ao->curr_char++;
    switch (*ao->curr_char) {
    case '\\':
      ao->curr_char++;
      return '\\';
    case '\"':
      ao->curr_char++;
      return '\"';
    default:
      return '\\';
    }
  default:
    fprintf(stderr, "Fuck you. %s Line %d", __FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }
}

static char *get_double_quote_arg(struct arg_obj *ao) {
  ao->curr_char++; // Skip past first double quote
  size_t count = 0;
  while (*ao->curr_char != '\0' && *ao->curr_char != '\"') {
    if (*ao->curr_char == '\\') {
      curr_arg[count++] = handle_backslash_char(ao, INSIDE_DOUBLE_QUOTES);
    } else {
      curr_arg[count++] = *ao->curr_char++;
    }
  }
  if (*ao->curr_char == '\"') {
    ao->curr_char++;
  }
  /* 
   * running the while loop again to collect reset of argument that's adjacent to end of 
   * quote. Example below    |
   *                         |
   *                         V
   * echo "test\"insidequotes"example\"
   * */
  while (*ao->curr_char != '\"' && *ao->curr_char != ' ' && *ao->curr_char != '\0') {
    if (*ao->curr_char == '\\') {
      curr_arg[count++] = handle_backslash_char(ao, OUTSIDE_QUOTES);
    } else {
      curr_arg[count++] = *ao->curr_char++;
    }
  }
  curr_arg[count] = '\0';
  char *new_arg = strndup(curr_arg, count);
  if (new_arg == NULL) {
    fprintf(stderr, "strndup failed at line %d in %s\n", (__LINE__)-2,
            __FUNCTION__);
  }
  return new_arg;
}

void clear_args(struct arg_obj *ao) {
  for (size_t i = 0; i < ao->size; i++) {
    free(ao->args[i]);
  }
  ao->size = 0;
  ao->input = NULL;
  ao->curr_char = NULL;
  ao->redir_type = INITIAL_VAL;
}
