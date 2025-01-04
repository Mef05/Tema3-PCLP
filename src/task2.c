#include "task2.h"

#define INIT_CHAR 256
#define INIT_LINE 100

void display_fields(void *data, const char *fields, const char *table) {
  if (strcmp(table, "studenti") == 0) {
    student *st = (student *)data;
    if (strcmp(fields, "*") == 0) {
      printf("%d %s %d %c %.2f\n", st->id, st->nume, st->an_studiu, st->statut,
             st->medie_generala);
    } else {
      char fields_copy[INIT_CHAR];
      strcpy(fields_copy, fields);
      char *field = strtok(fields_copy, ",");
      while (field != NULL) {
        if (strcmp(field, "id") == 0) {
          printf("%d ", st->id);
        } else if (strcmp(field, "nume") == 0) {
          printf("%s ", st->nume);
        } else if (strcmp(field, "an_studiu") == 0) {
          printf("%d ", st->an_studiu);
        } else if (strcmp(field, "statut") == 0) {
          printf("%c ", st->statut);
        } else if (strcmp(field, "medie_generala") == 0) {
          printf("%.2f ", st->medie_generala);
        }
        field = strtok(NULL, ",");
      }
      printf("\n");
    }
  } else if (strcmp(table, "materii") == 0) {
    materie *mt = (materie *)data;
    if (strcmp(fields, "*") == 0) {
      printf("%d %s %s\n", mt->id, mt->nume, mt->nume_titular);
    } else {
      char fields_copy[INIT_CHAR];
      strcpy(fields_copy, fields);
      char *field = strtok(fields_copy, ",");
      while (field != NULL) {
        if (strcmp(field, "id") == 0) {
          printf("%d ", mt->id);
        } else if (strcmp(field, "nume") == 0) {
          printf("%s ", mt->nume);
        } else if (strcmp(field, "nume_titular") == 0) {
          printf("%s ", mt->nume_titular);
        }
        field = strtok(NULL, ",");
      }
      printf("\n");
    }
  } else if (strcmp(table, "inrolari") == 0) {
    inrolare *in = (inrolare *)data;
    if (strcmp(fields, "*") == 0) {
      printf("%d %d %.2f %.2f %.2f\n", in->id_student, in->id_materie,
             in->note[0], in->note[1], in->note[2]);
    } else {
      char fields_copy[INIT_CHAR];
      strcpy(fields_copy, fields);
      char *field = strtok(fields_copy, ",");
      while (field != NULL) {
        if (strcmp(field, "id_student") == 0) {
          printf("%d ", in->id_student);
        } else if (strcmp(field, "id_materie") == 0) {
          printf("%d ", in->id_materie);
        } else if (strcmp(field, "note") == 0) {
          printf("%.2f %.2f %.2f ", in->note[0], in->note[1], in->note[2]);
        }
        field = strtok(NULL, ",");
      }
      printf("\n");
    }
  }
}

int main(int argc, char *argv[]) {
  char filename[INIT_CHAR];
  strcpy(filename, argv[1]);

  secretariat *s = citeste_secretariat(filename);

  int num_of_comands = 0;
  scanf("%d", &num_of_comands);
  char **comands = (char **)malloc(num_of_comands * sizeof(char *));
  if (comands == NULL) {
    return -1;
  }

  int index = 0;
  char ch = 0;
  char *buffer = (char *)malloc(INIT_LINE * sizeof(char));
  if (buffer == NULL) {
    return -1;
  }

  for (int i = 0; i < num_of_comands; i++) {
    index = 0;

    while ((ch = (char)getchar()) != EOF) {
      if (ch == ';') {
        buffer[index] = '\0';
        break;
      } else if (ch == '\n') {
        continue;
      } else if (index < sizeof(buffer) - 1) {
        buffer[index++] = ch;
      }
      if (index = INIT_LINE - 1) {
        buffer = (char *)realloc(buffer, 2 * INIT_LINE * sizeof(char));
      }
      if (buffer == NULL) {
        return -1;
      }
    }

    comands[i] = (char *)malloc((index + 1) * sizeof(char));
    for (int j = 0; j < index; j++) {
      comands[i][j] = buffer[j];
    }
  }

  free(buffer);

  for (int i = 0; i < num_of_comands; i++) {
    printf("%s\n", comands[i]);
    free(comands[i]);
  }
  return 0;
}
