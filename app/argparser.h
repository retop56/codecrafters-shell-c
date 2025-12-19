#include "arg_obj_def.h"

struct arg_obj * create_arg_obj();
void add_args(struct arg_obj * ao, char * input);
void clear_args(struct arg_obj * ao);
static char * get_normal_arg(struct arg_obj * ao, char * input);
static char * get_single_quote_arg(struct arg_obj * ao, char * input);
