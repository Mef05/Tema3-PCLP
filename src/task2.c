#include "task2.h"
#include <stdbool.h>

#define INIT_CHAR 256
#define INIT_LINE 200

typedef struct {
  char *comanda;
  int length;
  enum { SELECT, UPDATE, DELETE } operation;
  char **conditions;
  int num_of_conditions;
} comanda;

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
  int i = 0, j = 0;
  int len = (int)strlen(str);

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
                     char * valoare) {
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
    strcpy(valoare, token);
  }
}

typedef struct {
  char **campuri;
  int num_of_campuri;
  char *tabel;
} selectInfo;

typedef struct {
  char *tabel;
  char *camp;
  char *valoare;
} updateInfo;

typedef struct {
  char *tabel;
} deleteInfo;

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
  } else if (comanda.operation == UPDATE) {
    updateInfo uInfo =
        (updateInfo){.tabel = NULL, .camp = NULL, .valoare = NULL};

    char *token = strtok(comanda_copy, " ");
    token = strtok(NULL, " ");
    uInfo.tabel = (char *)calloc(strlen(token) + 1, sizeof(char));
    snprintf(uInfo.tabel, strlen(token) + 1, "%s", token);

    char **set = 0;
    int set_size = 0;

    token = strtok(NULL, " ");
    while (token != NULL) {
      token = strtok(NULL, " ");
      if (!strcmp(token, "WHERE")) {
        break;
      }
      if (strcmp(token, "=")) {
        dinamicaly_add_string(&set, &set_size, token);
      }
    }

    uInfo.camp = (char *)calloc(strlen(set[0]) + 1, sizeof(char));
    snprintf(uInfo.camp, strlen(set[0]) + 1, "%s", set[0]);
    free(set[0]);
    int val_len = 0;

    for (int i = 1; i < set_size; i++) {
      val_len += (int)strlen(set[i]) + 1;
    }
    uInfo.valoare = (char *)calloc(val_len, sizeof(char));

    for (int i = 1; i < set_size; i++) {
      strcat(uInfo.valoare, set[i]);
      if (i != set_size - 1) {
        strcat(uInfo.valoare, " ");
      }
    }

    remove_char(uInfo.valoare, '"');
    free(set);

    *(updateInfo *)info = uInfo;
  } else if (comanda.operation == DELETE) {
    deleteInfo dInfo = (deleteInfo){.tabel = NULL};

    char *token = strtok(comanda_copy, " ");
    token = strtok(NULL, " ");
    token = strtok(NULL, " ");
    dInfo.tabel = (char *)calloc(strlen(token) + 1, sizeof(char));
    snprintf(dInfo.tabel, strlen(token) + 1, "%s", token);

    *(deleteInfo *)info = dInfo;
  }

  free(comanda_copy);
}

char *intToString(int num) {
  int size = snprintf(NULL, 0, "%d", num) + 1;
  char *str = malloc(size);
  if (str == NULL) {
    exit(1);
  }
  sprintf(str, "%d", num);
  return str;
}

char *floatToString(float num) {
  int size = snprintf(NULL, 0, "%f", num) + 1;
  char *str = malloc(size);
  if (str == NULL) {
    exit(1);
  }
  sprintf(str, "%f", num);
  return str;
}

bool verify_conditions(comanda comanda, secretariat s, int index) {
  if (comanda.num_of_conditions == 0) {
    return true;
  }

  char camp[INIT_CHAR];
  char operator[INIT_CHAR];
  char valoare[INIT_CHAR];

  for (int i = 0; i < comanda.num_of_conditions; i++) {
    split_condition(comanda.conditions[i - 1], camp, operator, & valoare);

    if (comanda.operation == SELECT) {

      selectInfo info =
          (selectInfo){.campuri = NULL, .num_of_campuri = 0, .tabel = NULL};
      extract_info(comanda, &info);

      if (!strcmp(info.tabel, "studenti")) {
        if (strcmp(camp, "id") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].id == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.studenti[index].id < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.studenti[index].id > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.studenti[index].id <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.studenti[index].id >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.studenti[index].id != atoi(valoare)) {
              return true;
            }
          }

        } else if (strcmp(camp, "nume") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (!strcmp(s.studenti[index].nume, valoare)) {
              return true;
            }
          }

        } else if (strcmp(camp, "an_studiu") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (!strcmp(s.studenti[index].an_studiu, valoare)) {
              return true;
            }
          }

        } else if (strcmp(camp, "statut") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].statut == valoare) {
              return true;
            }
          }

        } else if (strcmp(camp, "medie_generala") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].medie_generala == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.studenti[index].medie_generala < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.studenti[index].medie_generala > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.studenti[index].medie_generala <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.studenti[index].medie_generala >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.studenti[index].medie_generala != atoi(valoare)) {
              return true;
            }
          }
        }
      } else if (!strcmp(info.tabel, "materii")) {
        if (!strcmp(camp, "id")) {
          if (strcmp(operator, "=") == 0) {
            if (s.materii[index].id == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.materii[index].id < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.materii[index].id > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.materii[index].id <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.materii[index].id >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.materii[index].id != atoi(valoare)) {
              return true;
            }
          }
        } else if (!strcmp(camp, "nume")) {
          if (strcmp(operator, "=") == 0) {
            if (!strcmp(s.materii[index].nume, valoare)) {
              return true;
            }
          }
        } else if (!strcmp(camp, "nume_titular")) {
          if (strcmp(operator, "=") == 0) {
            if (!strcmp(s.materii[index].nume_titular, valoare)) {
              return true;
            }
          }
        }
      } else if (!strcmp(info.tabel, "inrolari")) {
        if (!strcmp(camp, "id_student")) {
          if (strcmp(operator, "=") == 0) {
            if (s.inrolari[index].id_student == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.inrolari[index].id_student < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.inrolari[index].id_student > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.inrolari[index].id_student <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.inrolari[index].id_student >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.inrolari[index].id_student != atoi(valoare)) {
              return true;
            }
          }
        } else if (!strcmp(camp, "id_materie")) {
          if (strcmp(operator, "=") == 0) {
            if (s.inrolari[index].id_materie == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.inrolari[index].id_materie < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.inrolari[index].id_materie > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.inrolari[index].id_materie <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.inrolari[index].id_materie >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.inrolari[index].id_materie != atoi(valoare)) {
              return true;
            }
          }
        } else if (!strcmp(camp, "note")) {
          float note_cond[NUMBER_OF_GRADES] = {0, 0, 0};
          sscanf(valoare, "%f %f %f", &note_cond[0], &note_cond[1],
                 &note_cond[2]);
          if (s.inrolari[index].note[0] == note_cond[0] &&
              s.inrolari[index].note[1] == note_cond[1] &&
              s.inrolari[index].note[2] == note_cond[2]) {
            return true;
          }
        }
      }

    } else if (comanda.operation == UPDATE) {
      updateInfo info =
          (updateInfo){.tabel = NULL, .camp = NULL, .valoare = NULL};
      extract_info(comanda, &info);

      if (!strcmp(info.tabel, "studenti")) {
        if (strcmp(camp, "id") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].id == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.studenti[index].id < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.studenti[index].id > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.studenti[index].id <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.studenti[index].id >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.studenti[index].id != atoi(valoare)) {
              return true;
            }
          }

        } else if (strcmp(camp, "nume") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (!strcmp(s.studenti[index].nume, valoare)) {
              return true;
            }
          }

        } else if (strcmp(camp, "an_studiu") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (!strcmp(s.studenti[index].an_studiu, valoare)) {
              return true;
            }
          }

        } else if (strcmp(camp, "statut") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].statut == valoare) {
              return true;
            }
          }

        } else if (strcmp(camp, "medie_generala") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].medie_generala == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.studenti[index].medie_generala < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.studenti[index].medie_generala > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.studenti[index].medie_generala <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.studenti[index].medie_generala >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.studenti[index].medie_generala != atoi(valoare)) {
              return true;
            }
          }
        }
      } else if (!strcmp(info.tabel, "materii")) {
        if (!strcmp(camp, "id")) {
          if (strcmp(operator, "=") == 0) {
            if (s.materii[index].id == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.materii[index].id < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.materii[index].id > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.materii[index].id <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.materii[index].id >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.materii[index].id != atoi(valoare)) {
              return true;
            }
          }
        } else if (!strcmp(camp, "nume")) {
          if (strcmp(operator, "=") == 0) {
            if (!strcmp(s.materii[index].nume, valoare)) {
              return true;
            }
          }
        } else if (!strcmp(camp, "nume_titular")) {
          if (strcmp(operator, "=") == 0) {
            if (!strcmp(s.materii[index].nume_titular, valoare)) {
              return true;
            }
          }
        }
      } else if (!strcmp(info.tabel, "inrolari")) {
        if (!strcmp(camp, "id_student")) {
          if (strcmp(operator, "=") == 0) {
            if (s.inrolari[index].id_student == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.inrolari[index].id_student < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.inrolari[index].id_student > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.inrolari[index].id_student <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.inrolari[index].id_student >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.inrolari[index].id_student != atoi(valoare)) {
              return true;
            }
          }
        } else if (!strcmp(camp, "id_materie")) {
          if (strcmp(operator, "=") == 0) {
            if (s.inrolari[index].id_materie == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.inrolari[index].id_materie < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.inrolari[index].id_materie > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.inrolari[index].id_materie <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.inrolari[index].id_materie >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.inrolari[index].id_materie != atoi(valoare)) {
              return true;
            }
          }
        } else if (!strcmp(camp, "note")) {
          float note_cond[NUMBER_OF_GRADES] = {0, 0, 0};
          sscanf(valoare, "%f %f %f", &note_cond[0], &note_cond[1],
                 &note_cond[2]);
          if (s.inrolari[index].note[0] == note_cond[0] &&
              s.inrolari[index].note[1] == note_cond[1] &&
              s.inrolari[index].note[2] == note_cond[2]) {
            return true;
          }
        }
      }

    } else if (comanda.operation == DELETE) {
      deleteInfo info = (deleteInfo){.tabel = NULL};
      extract_info(comanda, &info);

      if (!strcmp(info.tabel, "studenti")) {
        if (strcmp(camp, "id") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].id == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.studenti[index].id < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.studenti[index].id > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.studenti[index].id <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.studenti[index].id >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.studenti[index].id != atoi(valoare)) {
              return true;
            }
          }

        } else if (strcmp(camp, "nume") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (!strcmp(s.studenti[index].nume, valoare)) {
              return true;
            }
          }

        } else if (strcmp(camp, "an_studiu") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (!strcmp(s.studenti[index].an_studiu, valoare)) {
              return true;
            }
          }

        } else if (strcmp(camp, "statut") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].statut == valoare) {
              return true;
            }
          }

        } else if (strcmp(camp, "medie_generala") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].medie_generala == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.studenti[index].medie_generala < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.studenti[index].medie_generala > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.studenti[index].medie_generala <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.studenti[index].medie_generala >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.studenti[index].medie_generala != atoi(valoare)) {
              return true;
            }
          }
        }
      } else if (!strcmp(info.tabel, "materii")) {
        if (!strcmp(camp, "id")) {
          if (strcmp(operator, "=") == 0) {
            if (s.materii[index].id == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.materii[index].id < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.materii[index].id > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.materii[index].id <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.materii[index].id >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.materii[index].id != atoi(valoare)) {
              return true;
            }
          }
        } else if (!strcmp(camp, "nume")) {
          if (strcmp(operator, "=") == 0) {
            if (!strcmp(s.materii[index].nume, valoare)) {
              return true;
            }
          }
        } else if (!strcmp(camp, "nume_titular")) {
          if (strcmp(operator, "=") == 0) {
            if (!strcmp(s.materii[index].nume_titular, valoare)) {
              return true;
            }
          }
        }
      } else if (!strcmp(info.tabel, "inrolari")) {
        if (!strcmp(camp, "id_student")) {
          if (strcmp(operator, "=") == 0) {
            if (s.inrolari[index].id_student == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.inrolari[index].id_student < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.inrolari[index].id_student > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.inrolari[index].id_student <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.inrolari[index].id_student >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.inrolari[index].id_student != atoi(valoare)) {
              return true;
            }
          }
        } else if (!strcmp(camp, "id_materie")) {
          if (strcmp(operator, "=") == 0) {
            if (s.inrolari[index].id_materie == atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.inrolari[index].id_materie < atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.inrolari[index].id_materie > atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.inrolari[index].id_materie <= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.inrolari[index].id_materie >= atoi(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.inrolari[index].id_materie != atoi(valoare)) {
              return true;
            }
          }
        } else if (!strcmp(camp, "note")) {
          float note_cond[NUMBER_OF_GRADES] = {0, 0, 0};
          sscanf(valoare, "%f %f %f", &note_cond[0], &note_cond[1],
                 &note_cond[2]);
          if (s.inrolari[index].note[0] == note_cond[0] &&
              s.inrolari[index].note[1] == note_cond[1] &&
              s.inrolari[index].note[2] == note_cond[2]) {
            return true;
          }
        }
      }
    }
  }
}

void apply_comand(comanda comanda, secretariat *s) {
  if (comanda.operation == SELECT) {
    selectInfo info =
        (selectInfo){.campuri = NULL, .num_of_campuri = 0, .tabel = NULL};
    extract_info(comanda, &info);
  }
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

  // for (int i = 0; i < num_of_comands; i++) {
  //   printf("Comanda: %s | Tip: %s | Lenght: %d | ", comands[i].comanda,
  //          comands[i].operation == SELECT   ? "SELECT"
  //          : comands[i].operation == UPDATE ? "UPDATE"
  //                                           : "DELETE",
  //          comands[i].length);

  //   for (int j = 0; j < comands[i].num_of_conditions; j++) {
  //     char camp[INIT_CHAR];
  //     char operator[INIT_CHAR];
  //     char valoare[INIT_CHAR];

  //     split_condition(comands[i].conditions[j], camp, operator, & valoare);
  //     printf("Camp: %s | Operator: %s | Valoare: %s | ", camp, operator,
  //            valoare);
  //   }

  //   printf("\n");
  // }

  free_comands(comands, num_of_comands);
  return 0;
}
