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
void handle_program_execution();
void search_for_exec();

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
    }
    else {
      handle_program_execution(input);
    }
  }
  return 0;
}

void handle_exit_command() {
  // Get exit number
  int exit_num = atoi(strtok(NULL, " "));
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
    strcmp(curr_tok, "type") == 0) {
      printf("%s is a shell builtin\n", curr_tok);
    }
  else {
    char *exec_name = (char*) malloc(256 * sizeof(char));
    strcpy(exec_name, curr_tok);
    search_for_exec(exec_name);
  }
}

void handle_program_execution(char * prog) {
  if ((curr_tok = strtok(NULL, " ")) == NULL) {
    printf("%s: command not found\n", prog);
    return;
  }
  char * prog_args = strdup(curr_tok);
  curr_tok = strtok(NULL, " ");
  while (curr_tok != NULL) {
    strcat(prog_args, curr_tok);
    curr_tok = strtok(NULL, " ");
  }
  char * full_path_with_prog = (char *) malloc(sizeof(char) * 1000);
  snprintf(full_path_with_prog, 1000, "./%s %s\n", prog, prog_args);
  printf("%s", full_path_with_prog);
}

void search_for_exec(char * exec_name) {

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
        printf("%s is %s/%s\n", exec_name, curr_tok, exec_name);
        closedir(dirp);
        free(paths);
        return;
      }
    }
    closedir(dirp);
    curr_tok = strtok(NULL, ":");  // Get next directory from 'PATH'
  }
  printf("%s: not found\n", exec_name);
  free(paths);
}