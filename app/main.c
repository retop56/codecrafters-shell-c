#include <dirent.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFF_LENGTH 1000

char *curr_tok;
void handle_exit_command();
void handle_echo_command(char *args);
void handle_type_command();
void handle_program_execution(char *prog);
char *search_for_exec(char *exec_name);
void handle_pwd_command();
void handle_cd_command(char *command);
void handle_cat_command();

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
    // Make copy of input
    char *input_copy = strdup(input);
    // Get first token to see which command it is
    curr_tok = strtok(input, " ");
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

void handle_exit_command() { exit(0); }

void handle_echo_command(char *args) {
  char temp[BUFF_LENGTH] = {0};
  char echo_result[BUFF_LENGTH] = {0};
  if (args == NULL) {
    printf("\n");
    return;
  }
  if (strncmp(args, "'", 1) == 0) {
    curr_tok = strtok(&args[1], "'\r");
    strcat(echo_result, curr_tok);
    while ((curr_tok = strtok(NULL, "'\r")) != NULL) {
      strcat(echo_result, curr_tok);
    }
    printf("%s\n", echo_result);
    return;
  }
  if (strncmp(args, "\"", 1) == 0) {
    size_t echo_result_i = 0;
    size_t args_len = strlen(args);
    bool inside_quotes = true;
    for (size_t args_i = 1; args_i < args_len; args_i++) {
      if (args[args_i] == '\"') {
        inside_quotes = !inside_quotes;
        if (args_i + 1 >= args_len) { /* Reached end of args string*/
          break;
        }
        if (args_i + 1 < args_len && args[args_i + 1] == ' ') {
          while (args[args_i + 1] == ' ') {
            args_i++;
          }
          echo_result[echo_result_i++] = ' ';
        }
      } else if (args[args_i] == '\\') {
        if (args[args_i + 1] == '\\') {
          echo_result[echo_result_i++] = '\\';
          args_i++;
        } else if (args[args_i + 1] == '$') {
          echo_result[echo_result_i++] = '$';
          args_i++;
        } else if (args[args_i + 1] == '"') {
          echo_result[echo_result_i++] = '"';
          args_i++;
        } else {
          echo_result[echo_result_i++] = args[args_i];
          continue;
        }
      } else {
        echo_result[echo_result_i++] = args[args_i];
      }
    }
    printf("%s\n", echo_result);
    return;
  }
  size_t result_index = 0;
  size_t args_length = strlen(args);
  for (size_t j = 0; j < args_length; j++) {
    if (args[j] == ' ') {
      while (j + 1 < args_length && args[j + 1] == ' ') {
        j++;
      }
      echo_result[result_index++] = ' ';
      continue;
    }
    if (args[j] == '\\') {
      continue;
    }
    echo_result[result_index++] = args[j];
  }
  printf("%s\n", echo_result);
}

void handle_type_command() {
  curr_tok = strtok(NULL, " ");
  if (strcmp(curr_tok, "echo") == 0 || strcmp(curr_tok, "exit") == 0 ||
      strcmp(curr_tok, "type") == 0 || strcmp(curr_tok, "pwd") == 0) {
    printf("%s is a shell builtin\n", curr_tok);
  } else {
    char *exec_name = (char *)malloc(256 * sizeof(char));
    strcpy(exec_name, curr_tok);
    char *full_path = search_for_exec(exec_name);
    if (full_path == NULL) {
      printf("%s: not found\n", exec_name);
      free(full_path);
      free(exec_name);
      return;
    }
    /*Check permissions on file */
    printf("%s is %s\n", exec_name, full_path);
    free(full_path);
    free(exec_name);
  }
}

void handle_program_execution(char *input) {
  char *prog_args[100];
  size_t num_of_args = 0;
  char *curr_arg = strtok(input, " ");
  while (curr_arg != NULL) {
    prog_args[num_of_args++] = strdup(curr_arg);
    curr_arg = strtok(NULL, " ");
  }
  char *full_path = search_for_exec(prog_args[0]);
  if (full_path == NULL) {
    printf("%s: command not found\n", input);
    return;
  }
  printf("Program was passed %zu args (including program name).\n",
         num_of_args);
  /*fflush(stdout);*/
  printf("Arg #0 (program name): %s\n", prog_args[0]);
  /*fflush(stdout);*/
  for (size_t i = 1; i < num_of_args; i++) {
    printf("Arg #%zu: %s\n", i, prog_args[i]);
    /*fflush(stdout);*/
  }
  // Calculate length of full_path w/ args (and null at the end)
  size_t len_of_full_path = strlen(full_path);
  for (size_t i = 1; i < num_of_args; i++) {
    len_of_full_path += 1; // Add one for space in between args
    len_of_full_path += strlen(prog_args[i]);
  }
  len_of_full_path += 1; // Add one at end for null;
  char *real_full_path = (char *)malloc(len_of_full_path * sizeof(char));
  real_full_path[0] = '\0';
  strcat(real_full_path, full_path);
  for (size_t i = 1; i < num_of_args; i++) {
    strcat(real_full_path, " ");
    strcat(real_full_path, prog_args[i]);
  }
  prog_args[0] = full_path;
  prog_args[num_of_args] = NULL;
  pid_t p;
  /*fflush(stdout);*/
  p = fork();
  switch (p) {
  case -1:
    printf("Fork failed!\n");
    exit(EXIT_FAILURE);
  case 0:
    execvp(prog_args[0], prog_args);
    printf("Unreachable");
    break;
  default:
    free(full_path);
    free(real_full_path);
    wait(&p);
    break;
  }
}

char *search_for_exec(char *exec_name) {
  char *full_path = (char *)malloc(PATH_MAX * sizeof(char));
  char *paths = strdup(getenv("PATH"));
  char *start_of_paths = paths;
  if (paths == NULL) {
    printf("Unable to get 'PATH' environment variable!\n");
    exit(EXIT_FAILURE);
  }
  /*printf("paths received : %s\n", paths); //Debug line*/
  curr_tok = strtok(paths, ":");
  // Loop through every path in the 'PATH' env_var and
  // search for 'exec_name' in the path. If found, print it
  DIR *dirp;
  while (curr_tok != NULL) {
    /*printf("Currently looking at '%s'\n", curr_tok);*/ // Debug line
    dirp = opendir(curr_tok);
    if (dirp == NULL) {
      curr_tok = strtok(NULL, ":");
      continue;
    }
    struct dirent *pDirent;
    // Loop through every entry in directory
    while ((pDirent = readdir(dirp)) != NULL) {
      // Found matching file, now need to check if it's executable
      if (strcmp(exec_name, pDirent->d_name) == 0) {
        sprintf(full_path, "%s/%s", curr_tok,
                exec_name); // Construct full path name
        if ((access(full_path, X_OK)) == 0) {
          /*printf("%s is executable!\n", full_path);*/ // Debug line
          closedir(dirp);
          free(start_of_paths);
          return full_path;
        }
      }
    }
    curr_tok = strtok(NULL, ":");
  }
  free(full_path);
  free(start_of_paths);
  closedir(dirp);
  return NULL;
}

void handle_pwd_command() {
  const char *pwd_str = getenv("PWD");
  if (pwd_str == NULL) {
    fprintf(stderr, "Unable to get pwd!\n");
    return;
  }
  printf("%s\n", pwd_str);
}

void handle_cd_command(char *command) {
  if (*command == '\0') {
    return;
  } else if (strncmp(command, "./", 2) == 0) {
    char *fixed_arg = strdup(&command[1]);
    char *new_dir = (char *)calloc(1000, sizeof(char));
    strcat(new_dir, getenv("PWD"));
    strcat(new_dir, fixed_arg);
    if (chdir(new_dir) != 0) {
      printf("cd: %s: No such file or directory\n", command);
      return;
    }
    setenv("PWD", new_dir, 1);
    return;
  } else if (strncmp(command, "../", 3) == 0) {
    char *new_command = strdup(command + 3);
    char *curr_pwd = getenv("PWD");
    int i = strlen(curr_pwd);
    for (; i > 0; i--) {
      if (curr_pwd[i] == '/') {
        break;
      }
    }
    char *new_pwd = strndup(curr_pwd, i);
    setenv("PWD", new_pwd, 1);
    handle_cd_command(new_command);
    return;
  } else if (strncmp(command, "~", 1) == 0) {
    char *home_dir = getenv("HOME");
    setenv("PWD", home_dir, 1);
    return;
  } else if (chdir(command) != 0) {
    printf("cd: %s: No such file or directory\n", command);
    return;
  }
  setenv("PWD", command, 1);
}

void handle_cat_command() {
  char temp[BUFF_LENGTH] = {0};
  strcat(temp, "cat ");
  while ((curr_tok = strtok(NULL, "")) != NULL) {
    strcat(temp, curr_tok);
  }
  system(temp);
  return;
}
