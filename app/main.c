#include "cc_shell.h"
#include "handle_commands.h"

char *curr_tok;

int main() {
  char input[BUFF_LENGTH];
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
    /*struct arg_obj ao = create_arg_obj();*/
    /*add_args(&ao);*/
    // Make copy of input
    char *input_copy = strdup(input);
    // Get first token to see which command it is
    char *curr_tok = strtok(input, " ");
    if (strncmp(curr_tok, "exit", 4) == 0) {
      handle_exit_command();
    } else if (strncmp(curr_tok, "echo", 4) == 0) {
      handle_echo_command(&input_copy[5]);
    } else if (strncmp(curr_tok, "type", 4) == 0) {
      handle_type_command();
    } else if (strncmp(curr_tok, "pwd", 3) == 0) {
      handle_pwd_command();
    } else if (strncmp(curr_tok, "cd", 2) == 0) {
      char *cd_arg = strtok(NULL, " ");
      handle_cd_command(cd_arg);
    } else if (strncmp(curr_tok, "cat", 3) == 0) {
      handle_cat_command();
    } else {
      handle_program_execution(input_copy);
    }
  }
  return 0;
}

