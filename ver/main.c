#include "../src/task2.h"

#define INIT_CHAR 256
#define INIT_LINE 200

typedef struct {
  char *comanda;
  int length;
  enum { SELECT, UPDATE, DELETE } operation;
  char **conditions;
  int num_of_conditions;
} comanda; // works

typedef struct {
  char **campuri;
  int num_of_campuri;
  char *tabel;
} selectInfo;

void dinamicaly_add_string(char ***arr, int *size, char *string) {
  if (*size == 0) {
    *arr = malloc(sizeof(char *));
  } else {
    *arr = realloc(*arr, (*size + 1) * sizeof(char *));
  }
  if (*arr == NULL) {
    return;
  }

  (*arr)[*size] = malloc(strlen(string) + 1);
  if ((*arr)[*size] == NULL) {
    return;
  }
  snprintf((*arr)[*size], strlen(string) + 1, "%s", string);
  (*size)++;
}

bool is_lowercase(char *string) {
  for (int i = 0; string[i] != '\0'; i++) {
    if (string[i] >= 'A' && string[i] <= 'Z') {
      return false;
    }
  }
  return true;
}

void remove_char(char *str, char char_to_remove) {
  int i, j = 0;
  int len = strlen(str);

  for (i = 0; i < len; i++) {
    if (str[i] != char_to_remove) {
      str[j++] = str[i];
    }
  }
  str[j] = '\0';
}

void free_comands(comanda *comands, int num_of_comands) {
  for (int i = 0; i < num_of_comands; i++) {
    free(comands[i].comanda);
    if (comands[i].num_of_conditions > 0) {
      for (int j = 0; j < comands[i].num_of_conditions; j++) {
        free(comands[i].conditions[j]);
      }
      free(comands[i].conditions);
    }
  }
  free(comands);
}

void detect_comad_type(comanda *comanda) {
  char *comanda_copy = (char *)malloc(comanda->length * sizeof(char));
  if (comanda_copy == NULL) {
    return;
  }
  strcpy(comanda_copy, comanda->comanda);

  char *token = strtok(comanda_copy, " ");
  if (!strcmp(token, "SELECT")) {
    comanda->operation = SELECT;
  } else if (!strcmp(token, "UPDATE")) {
    comanda->operation = UPDATE;
  } else if (!strcmp(token, "DELETE")) {
    comanda->operation = DELETE;
  }

  free(comanda_copy);
}

void extract_init_condition(comanda *comanda) {
  char *comanda_copy = (char *)malloc(comanda->length * sizeof(char));
  if (comanda_copy == NULL) {
    return;
  }
  strcpy(comanda_copy, comanda->comanda);

  char *where = 0;
  char *and = 0;

  if (strstr(comanda_copy, "AND") != NULL) {
    comanda->conditions = (char **)malloc(2 * sizeof(char *));
    comanda->num_of_conditions = 2;
    and = strstr(comanda_copy, "AND");
    where = strstr(comanda_copy, "WHERE");
  } else if (strstr(comanda_copy, "WHERE") != NULL) {
    comanda->conditions = (char **)malloc(sizeof(char *));
    comanda->num_of_conditions = 1;
    where = strstr(comanda_copy, "WHERE");
  } else {
    comanda->conditions = NULL;
    comanda->num_of_conditions = 0;
    return;
  }

  and += 4;
  where += 6;
  char buffer[INIT_CHAR];
  int index = 0;

  while (*where != '\0' && *where != 'A') {
    buffer[index++] = *(where++);
  }
  buffer[index] = '\0';

  comanda->conditions[0] = (char *)malloc((index + 1) * sizeof(char));
  strcpy(comanda->conditions[0], buffer);

  if (comanda->num_of_conditions == 2) {
    index = 0;
    while (*and != '\0') {
      buffer[index++] = *(and++);
    }
    buffer[index] = '\0';

    comanda->conditions[1] = (char *)malloc((index + 1) * sizeof(char));
    strcpy(comanda->conditions[1], buffer);
  }
}

void split_condition(const char *input, char *camp, char *operator,
                     int * valoare) {
  char buffer[INIT_CHAR];
  char valoare_str[INIT_CHAR];
  strncpy(buffer, input, sizeof(buffer) - 1);
  buffer[sizeof(buffer) - 1] = '\0';

  char *token = strtok(buffer, " ");
  if (token != NULL) {
    strcpy(camp, token);
  }

  token = strtok(NULL, " ");
  if (token != NULL) {
    strcpy(operator, token);
  }

  token = strtok(NULL, " ");
  if (token != NULL) {
    strcpy(valoare_str, token);
  }

  *valoare = atoi(valoare_str);
}

void extract_info(comanda comanda, void *info) {
  char *comanda_copy = (char *)malloc((comanda.length + 1) * sizeof(char));
  if (comanda_copy == NULL) {
    return;
  }
  snprintf(comanda_copy, comanda.length + 1, "%s", comanda.comanda);

  if (comanda.operation == SELECT) {
    selectInfo sInfo =
        (selectInfo){.campuri = NULL, .num_of_campuri = 0, .tabel = NULL};

    char *token = strtok(comanda_copy, " ");

    while (token != NULL && strcmp(token, "FROM")) {
      token = strtok(NULL, " ");
      if (is_lowercase(token)) {
        remove_char(token, ',');
        dinamicaly_add_string(&sInfo.campuri, &sInfo.num_of_campuri, token);
      }
    }

    token = strtok(NULL, " ");
    if (is_lowercase(token)) {
      sInfo.tabel = (char *)malloc(strlen(token) + 1);
      snprintf(sInfo.tabel, strlen(token) + 1, "%s", token);
    }

    *(selectInfo *)info = sInfo;
  }

  free(comanda_copy);
}

int main(int argc, char *argv[]) {
  char filename[INIT_CHAR];
  strcpy(filename, argv[1]);

  secretariat *s = citeste_secretariat(filename);

  int num_of_comands = 0;
  scanf("%d", &num_of_comands);
  comanda *comands = calloc(num_of_comands, sizeof(comanda));
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
      } else {
        buffer[index++] = ch;
      }
    }

    comands[i].comanda = (char *)malloc((index + 1) * sizeof(char));
    for (int j = 0; j < index; j++) {
      comands[i].comanda[j] = buffer[j];
    }
    comands[i].length = index;
    detect_comad_type(&comands[i]);
    extract_init_condition(&comands[i]);
  }

  free(buffer);

  for (int i = 0; i < num_of_comands; i++) {
    printf("Comanda: %s | Tip: %s | Lenght: %d | ", comands[i].comanda,
           comands[i].operation == SELECT   ? "SELECT"
           : comands[i].operation == UPDATE ? "UPDATE"
                                            : "DELETE",
           comands[i].length);

    if (comands[i].operation == SELECT) {
      selectInfo info =
          (selectInfo){.campuri = NULL, .num_of_campuri = 0, .tabel = NULL};
      extract_info(comands[i], &info);
      printf("Campuri: ");
      for (int j = 0; j < info.num_of_campuri; j++) {
        printf("%s ", info.campuri[j]);
      }
    }
    printf("\n");
  }

  free_comands(comands, num_of_comands);
  return 0;
}
