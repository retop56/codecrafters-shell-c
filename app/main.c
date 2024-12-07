#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unistd.h>

char * curr_tok;
void handle_exit_command();
void handle_echo_command();
void handle_type_command();
void handle_program_execution(char * prog);
char * search_for_exec(char * exec_name);
void handle_pwd_command();
void handle_cd_command(char * command);

int main() {
  char input[100];
  while (true) {
    printf("$ ");
    fflush(stdout);
    // Wait for user input
    if (fgets(input, 100, stdin) == NULL) {
      continue;
    }
    // Replace \n at end of string with null
    int len_of_input = strlen(input);
    input[len_of_input - 1] = '\0';
    // Get first token to see which command it is
    curr_tok = strtok(input, " ");
    if (strcmp(curr_tok, "exit") == 0) {
      handle_exit_command();
    } else if (strcmp(curr_tok, "echo") == 0) {
      handle_echo_command();
    } else if (strcmp(curr_tok, "type") == 0) {
      handle_type_command();
    } else if (strcmp(curr_tok, "pwd") == 0) {
      handle_pwd_command();
    } else if (strcmp(curr_tok, "cd") == 0) {
      char * cd_arg = strtok(NULL, " ");
      handle_cd_command(cd_arg);
    }
    else {
      handle_program_execution(input);
    }
  }
  return 0;
}

void handle_exit_command() {
  // Get exit number
  char * exit_arg = strtok(NULL, " ");
  if (exit_arg == NULL) {
    fprintf(stderr, "Unable to process argument to exit command!\n");
    printf("Usage: exit <number>\n");
    return;
  }
  int exit_num = atoi(exit_arg);
  exit(exit_num);
}

void handle_echo_command() {
  char temp[100];
  char echo_result[100];
  curr_tok = strtok(NULL, " ");
  if (curr_tok == NULL) {
    printf("\n");
    return;
  } 
  strcpy(echo_result, curr_tok);
  curr_tok = strtok(NULL, " ");
  temp[0] = 0;
  while (curr_tok != NULL) {
    sprintf(temp, " %s", curr_tok);
    strcat(echo_result, temp);
    curr_tok = strtok(NULL, " ");
  } 
  printf("%s\n", echo_result);
}

void handle_type_command() {
  curr_tok = strtok(NULL, " ");
  if (
    strcmp(curr_tok, "echo") == 0 ||
    strcmp(curr_tok, "exit") == 0 ||
    strcmp(curr_tok, "type") == 0 ||
    strcmp(curr_tok, "pwd") == 0) {
      printf("%s is a shell builtin\n", curr_tok);
    }
  else {
    char *exec_name = (char*) malloc(256 * sizeof(char));
    strcpy(exec_name, curr_tok);
    char * full_path = search_for_exec(exec_name); 
    if (full_path == NULL) {
      printf("%s: not found\n", exec_name);
      return;
    } 
    printf("%s is %s\n", exec_name, full_path);
    free(full_path);
    free(exec_name);
  }
}

void handle_program_execution(char * prog) {
  if ((curr_tok = strtok(NULL, " ")) == NULL) {
    printf("%s: command not found\n", prog);
    return;
  }
  char * prog_args = (char *) calloc(1000, sizeof(char));
  strcat(prog_args, curr_tok);
  curr_tok = strtok(NULL, " ");
  while (curr_tok != NULL) {
    strcat(prog_args, curr_tok);
    curr_tok = strtok(NULL, " ");
  }
  char * full_path = search_for_exec(prog);
  if (full_path == NULL) {
    fprintf(stderr, "Unable to find '%s'!\n", prog);
    free(prog_args);
    return;
  }
  char * full_path_with_args = (char *) calloc(1000, sizeof(char));
  snprintf(full_path_with_args, 1000, "%s %s\n", full_path, prog_args);
  system(full_path_with_args);
}

char * search_for_exec(char * exec_name) {

  // Loop through every path in the 'PATH' env_var and
  // search for 'exec_name' in the path. If found, print it

  char *paths = strdup(getenv("PATH"));
  if (paths == NULL) {
    printf("Unable to get 'PATH' environment variable!\n");
    exit(EXIT_FAILURE);
  }
  curr_tok = strtok(paths, ":");
  int num_of_paths_checked = 0;
  struct dirent *files;
  while (curr_tok != NULL) {
    num_of_paths_checked++;
    DIR *dirp = opendir(curr_tok);
    if (dirp == NULL) {
      curr_tok = strtok(NULL, ":");
      continue;
    } 
    // Loop through every file in directory and check if it's a match with 'exec_name'
    while ((files = readdir(dirp)) != NULL) {
      if ((strcmp(files->d_name, exec_name)) == 0) {
        char * result = (char *) malloc(1000 * sizeof(char));
        sprintf(result, "%s/%s", curr_tok, exec_name);
        closedir(dirp);
        free(paths);
        return result;
      }
    }
    closedir(dirp);
    curr_tok = strtok(NULL, ":");  // Get next directory from 'PATH'
  }
  free(paths);
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

void handle_cd_command(char * command) {
  if (*command == '\0') {
    return;
  } else if (strncmp(command, "./", 2) == 0) {
    char *fixed_arg = strdup(&command[1]);
    char * new_dir = (char *) calloc(1000, sizeof(char));
    strcat(new_dir, getenv("PWD"));
    strcat(new_dir, fixed_arg);
    if (chdir(new_dir) != 0) {
      printf("cd: %s: No such file or directory\n", command);
      return;
    }
    setenv("PWD", new_dir, 1);
    return;
  } else if (strncmp(command, "../", 3) == 0) {
    char * new_command = strdup(command + 3);
    char * curr_pwd = getenv("PWD");
    int i = strlen(curr_pwd);
    for (; i > 0; i--) {
      if (curr_pwd[i] == '/') {
        break;
      }
    }
    char * new_pwd = strndup(curr_pwd, i);
    setenv("PWD", new_pwd, 1);
    handle_cd_command(new_command);
    return;
  } else if (strncmp(command, '~', 1) == 0) {
    char * home_dir = getenv("HOME");
    setenv("PWD", home_dir, 1);
    return;
  } else if (chdir(command) != 0) {
    printf("cd: %s: No such file or directory\n", command);
    return;
  }
  setenv("PWD", command, 1);
}