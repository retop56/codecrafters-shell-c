#ifndef handle_commands

#include <stddef.h>
struct arg_obj {
  size_t size;
  size_t limit;
  char **args;
};

void handle_exit_command();
void handle_echo_command(char *args);
void handle_type_command();
void handle_program_execution(char *prog);
char *search_for_exec(char *exec_name);
void handle_pwd_command();
void handle_cd_command(char *command);
void handle_cat_command();
struct arg_obj create_arg_obj();
void add_args(struct arg_obj *ao);
void clear_args(struct arg_obj *ao);

#endif // !handle_commands
