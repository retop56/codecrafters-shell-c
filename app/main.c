#include "arg_obj_def.h"
#include "argparser.h"
#include "cc_shell.h"
#include "handle_commands.h"
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
    ao->input = (char *)&input;
    ao->curr_char = (char *)&input;
    add_args(ao);
    if (ao->redir_type == STD_OUT ||
        ao->redir_type == STD_ERR) {
      handle_program_execution_w_redirect(ao);
    } else if (strncmp(ao->args[0], "exit", 4) == 0) {
      handle_exit_command();
    } else if (strncmp(ao->args[0], "echo", 4) == 0) {
      handle_echo_command(ao);
    } else if (strncmp(ao->args[0], "type", 4) == 0) {
      handle_type_command(ao);
    } else if (strncmp(ao->args[0], "pwd", 3) == 0) {
      handle_pwd_command();
    } else if (strncmp(ao->args[0], "cd", 2) == 0) {
      handle_cd_command(ao);
    } else {
      handle_program_execution(ao);
    } 
    clear_args(ao);
  }
  free(ao);
  return 0;
}
