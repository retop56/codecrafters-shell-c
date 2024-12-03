#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char * curr_tok;

int main() {
  char input[100];
  char temp[100];
  char echo_result[100];
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
      // Get exit number
      int exit_num = atoi(strtok(NULL, " "));
      exit(exit_num);
    } else if (strcmp(curr_tok, "echo") == 0) {
      echo_result[0] = 0;
      curr_tok = strtok(NULL, " ");
      if (curr_tok == NULL) {
        printf("\n");
        continue;
      } else {
        strcat(echo_result, curr_tok);
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
    else {
      printf("%s: command not found\n", input);
    }
  }
  return 0;
}
