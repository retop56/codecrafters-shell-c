#include "arg_obj_def.h"
#include "cc_shell.h"

struct arg_obj *create_arg_obj();
void add_args(struct arg_obj *ao);
void clear_args(struct arg_obj *ao);
static char *get_normal_arg(struct arg_obj *ao);
static char *get_single_quote_arg(struct arg_obj *ao);
static char *get_double_quote_arg(struct arg_obj *ao);
