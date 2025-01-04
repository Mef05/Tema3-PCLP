
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  char buffer[100]; // Adjust size as needed
  int index = 0;
  char ch;

  int num_of_comands = 0;
  scanf("%d", &num_of_comands);

  char **lines = (char **)malloc(num_of_comands * sizeof(char *));

  printf("Enter lines ending with ';' (type 'exit;' to stop):\n");
  for (int i = 0; i < num_of_comands; i++) {
    printf("> ");
    index = 0;

    while ((ch = getchar()) != EOF) {
      if (ch == ';') {
        buffer[index] = '\0'; // Null-terminate the string
        break;
      } else if (ch == '\n') {
        continue; // Skip newline
      } else if (index < sizeof(buffer) - 1) {
        buffer[index++] = ch;
      }
    }

    if (strcmp(buffer, "exit") == 0) {
      printf("Exiting...\n");
      break;
    }

    printf("Processed line: '%s'\n", buffer);

    lines[i] = (char *)malloc((index + 1) * sizeof(char));
    for (int j = 0; j < index; j++) {
      lines[i][j] = buffer[j];
    }
  }

  printf("Lines entered:\n");
  for (int i = 0; i < num_of_comands; i++) {
    printf("%s\n", lines[i]);
    free(lines[i]);
  }

  return 0;
}
