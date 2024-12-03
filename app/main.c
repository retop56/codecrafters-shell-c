#include <stdio.h>
#include <string.h>
#include <stdbool.h>

int main() {
  char input[100];
  while (true) {
    printf("$ ");
    fflush(stdout);
    // Wait for user input
    fgets(input, 100, stdin);
    int len_of_input = strlen(input);
    input[len_of_input - 1] = '\0';
    printf("%s: command not found\n", input);
  }
  return 0;
}
