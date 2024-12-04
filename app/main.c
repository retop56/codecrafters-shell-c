#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char * curr_tok;
void handle_exit_command();
void handle_echo_command();
void handle_type_command();

int main() {
  char input[100];
  while (true) {
    printf("$ ");
    fflush(stdout);
    // Wait for user input
    fgets(input, 100, stdin);
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
      printf("%s: command not found\n", input);
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
  echo_result[0] = 0;
  curr_tok = strtok(NULL, " ");
  if (curr_tok == NULL) {
    printf("\n");
    return;
  } else {
    strcpy(echo_result, curr_tok);
    curr_tok = strtok(NULL, " ");
  }
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
    char exec_name[100];
    strcpy(exec_name, curr_tok);
    printf("Exec name: %s\n", exec_name);
    /*
     Loop through every path in the 'PATH' env_var and
     search for 'exec_name' in the path. If found, print it
    */  
    char *paths = getenv("PATH");
    curr_tok = strtok(paths, ":");
    while (curr_tok != NULL) {
      printf("Found path: %s\n", curr_tok);
      curr_tok = strtok(NULL, ":");
    }
    // printf("%s: not found\n", curr_tok);
  }
}

