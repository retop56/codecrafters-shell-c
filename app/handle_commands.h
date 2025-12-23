#include "arg_obj_def.h"

void handle_exit_command();
void handle_echo_command(struct arg_obj *ao);
void handle_type_command(struct arg_obj *ao);
void handle_program_execution(struct arg_obj *prog);
void handle_pwd_command();
void handle_cd_command(struct arg_obj *ao);
void handle_program_execution_w_redirect(struct arg_obj *prog);
