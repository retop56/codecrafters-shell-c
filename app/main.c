#include "arg_obj_def.h"
#include "argparser.h"
#include "cc_shell.h"
#include "handle_commands.h"
char *curr_tok;
char input[BUFF_LENGTH];

int main() {
  struct arg_obj *ao = create_arg_obj();
  while (true) {
    printf("$ ");
    fflush(NULL);
    // Wait for user input
    if (fgets(input, BUFF_LENGTH, stdin) == NULL) {
      continue;
    }
    // Replace \n at end of string with null
    int len_of_input = strlen(input);
    input[len_of_input - 1] = '\0';
    // Creates arg_obj and populates it with arguments from input
    add_args(ao, (char *)&input);
    // Make copy of input
    /*char *input_copy = strdup(input);*/
    // Get first token to see which command it is
    /*char *curr_tok = strtok(input, " ");*/
    if (strncmp(ao->args[0], "exit", 4) == 0) {
      handle_exit_command();
    } else if (strncmp(ao->args[0], "echo", 4) == 0) {
      handle_echo_command(ao);
    } else if (strncmp(ao->args[0], "type", 4) == 0) {
      handle_type_command(ao);
    }else {
      handle_program_execution(ao);
    }
    clear_args(ao);
  }
  free(ao);
  return 0;
}
