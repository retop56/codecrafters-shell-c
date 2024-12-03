#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
    char * tok = strtok(input, " ");
    if (strcmp(tok, "exit") == 0) {
      // Get exit number
      int exit_num = atoi(strtok(NULL, " "));
      exit(exit_num);
    }
    printf("%s: command not found\n", input);
  }
  return 0;
}
