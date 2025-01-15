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

void detect_comad_type(comanda *comanda) {
  char *comanda_copy = (char *)malloc(comanda->length + 1 * sizeof(char));
  if (comanda_copy == NULL) {
    return;
  }
  snprintf(comanda_copy, comanda->length + 1, "%s", comanda->comanda);

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
  char *comanda_copy = (char *)malloc((comanda->length + 1) * sizeof(char));
  if (comanda_copy == NULL) {
    return;
  }
  snprintf(comanda_copy, comanda->length + 1, "%s", comanda->comanda);

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
    free(comanda_copy);
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
  snprintf(comanda->conditions[0], index + 1, "%s", buffer);

  if (comanda->num_of_conditions == 2) {
    index = 0;
    while (*and != '\0') {
      buffer[index++] = *(and++);
    }
    buffer[index] = '\0';

    comanda->conditions[1] = (char *)malloc((index + 1) * sizeof(char));
    // strcpy(comanda->conditions[1], buffer);
    snprintf(comanda->conditions[1], index + 1, "%s", buffer);
  }
  free(comanda_copy);
}

void split_condition(const char *input, char *camp, char *operator,
                     char * valoare) {
  char buffer[INIT_CHAR];
  snprintf(buffer, strlen(input) + 1, "%s", input);

  char *token = strtok(buffer, " ");
  if (token != NULL) {
    snprintf(camp, strlen(token) + 1, "%s", token);
  }

  token = strtok(NULL, " ");
  if (token != NULL) {
    snprintf(operator, strlen(token) + 1, "%s", token);
  }

  token = strtok(NULL, " ");
  if (token != NULL) {
    snprintf(valoare, strlen(token) + 1, "%s", token);
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

void free_select_info(selectInfo *info) {
  for (int i = 0; i < info->num_of_campuri; i++) {
    free(info->campuri[i]);
  }
  free(info->campuri);
  free(info->tabel);
}

void free_update_info(updateInfo *info) {
  free(info->tabel);
  free(info->camp);
  free(info->valoare);
}

void free_delete_info(deleteInfo *info) { free(info->tabel); }

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
  snprintf(str, size, "%d", num);
  return str;
}

char *floatToString(float num) {
  int size = snprintf(NULL, 0, "%f", num) + 1;
  char *str = malloc(size);
  if (str == NULL) {
    exit(1);
  }
  snprintf(str, size, "%f", num);
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
    split_condition(comanda.conditions[i - 1], camp, operator, valoare);

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
            if (!strcmp(intToString(s.studenti[index].an_studiu), valoare)) {
              return true;
            }
          }

        } else if (strcmp(camp, "statut") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].statut == valoare[0]) {
              return true;
            }
          }

        } else if (strcmp(camp, "medie_generala") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].medie_generala == atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.studenti[index].medie_generala < atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.studenti[index].medie_generala > atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.studenti[index].medie_generala <= atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.studenti[index].medie_generala >= atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.studenti[index].medie_generala != atof(valoare)) {
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
            if (!strcmp(intToString(s.studenti[index].an_studiu), valoare)) {
              return true;
            }
          }

        } else if (strcmp(camp, "statut") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].statut == valoare[0]) {
              return true;
            }
          }

        } else if (strcmp(camp, "medie_generala") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].medie_generala == atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.studenti[index].medie_generala < atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.studenti[index].medie_generala > atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.studenti[index].medie_generala <= atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.studenti[index].medie_generala >= atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.studenti[index].medie_generala != atof(valoare)) {
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
            if (s.studenti[index].an_studiu == atoi(valoare)) {
              return true;
            }
          }

        } else if (strcmp(camp, "statut") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].statut == valoare[0]) {
              return true;
            }
          }

        } else if (strcmp(camp, "medie_generala") == 0) {
          if (strcmp(operator, "=") == 0) {
            if (s.studenti[index].medie_generala == atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<") == 0) {
            if (s.studenti[index].medie_generala < atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">") == 0) {
            if (s.studenti[index].medie_generala > atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "<=") == 0) {
            if (s.studenti[index].medie_generala <= atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, ">=") == 0) {
            if (s.studenti[index].medie_generala >= atof(valoare)) {
              return true;
            }
          } else if (strcmp(operator, "!=") == 0) {
            if (s.studenti[index].medie_generala != atof(valoare)) {
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
  return false;
}

void apply_comand(comanda comanda, secretariat *s) {
  if (comanda.operation == SELECT) {
    selectInfo info =
        (selectInfo){.campuri = NULL, .num_of_campuri = 0, .tabel = NULL};
    extract_info(comanda, &info);

    if (!strcmp(info.tabel, "studenti")) {
      for (int j = 0; j < s->nr_studenti; j++) {
        if (verify_conditions(comanda, *s, j)) {
          for (int i = 0; i < info.num_of_campuri; i++) {
            if (i) {
              printf(" ");
            }
            if (!strcmp(info.campuri[i], "*")) {
              printf("%d %s %d %c %.2f\n", s->studenti[j].id,
                     s->studenti[j].nume, s->studenti[j].an_studiu,
                     s->studenti[j].statut, s->studenti[j].medie_generala);
            }
            if (!strcmp(info.campuri[i], "id")) {
              printf("%d", s->studenti[j].id);
            }
            if (!strcmp(info.campuri[i], "nume")) {
              printf("%s", s->studenti[j].nume);
            }
            if (!strcmp(info.campuri[i], "an_studiu")) {
              printf("%d", s->studenti[j].an_studiu);
            }
            if (!strcmp(info.campuri[i], "statut")) {
              printf("%c", s->studenti[j].statut);
            }
            if (!strcmp(info.campuri[i], "medie_generala")) {
              printf("%.2f", s->studenti[j].medie_generala);
            }
          }
          printf("\n");
        }
      }
    } else if (!strcmp(info.tabel, "materii")) {
      for (int j = 0; j < s->nr_materii; j++) {
        if (verify_conditions(comanda, *s, j)) {
          for (int i = 0; i < info.num_of_campuri; i++) {
            if (i) {
              printf(" ");
            }
            if (!strcmp(info.campuri[i], "*")) {
              printf("%d %s %s", s->materii[j].id, s->materii[j].nume,
                     s->materii[j].nume_titular);
            }
            if (!strcmp(info.campuri[i], "id")) {
              printf("%d", s->materii[j].id);
            }
            if (!strcmp(info.campuri[i], "nume")) {
              printf("%s", s->materii[j].nume);
            }
            if (!strcmp(info.campuri[i], "nume_titular")) {
              printf("%s", s->materii[j].nume_titular);
            }
          }
          printf("\n");
        }
      }
    } else if (!strcmp(info.tabel, "inrolari")) {
      for (int j = 0; j < s->nr_inrolari; j++) {
        if (verify_conditions(comanda, *s, j)) {
          for (int i = 0; i < info.num_of_campuri; i++) {
            if (i) {
              printf(" ");
            }
            if (!strcmp(info.campuri[i], "*")) {
              printf("%d %d %.2f %.2f %.2f", s->inrolari[j].id_student,
                     s->inrolari[j].id_materie, s->inrolari[j].note[0],
                     s->inrolari[j].note[1], s->inrolari[j].note[2]);
            }
            if (!strcmp(info.campuri[i], "id_student")) {
              printf("%d", s->inrolari[j].id_student);
            }
            if (!strcmp(info.campuri[i], "id_materie")) {
              printf("%d", s->inrolari[j].id_materie);
            }
            if (!strcmp(info.campuri[i], "note")) {
              printf("%.2f %.2f %.2f", s->inrolari[j].note[0],
                     s->inrolari[j].note[1], s->inrolari[j].note[2]);
            }
          }
          printf("\n");
        }
      }
    }
    free_select_info(&info);
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
    comands[i].comanda[index] = '\0';
    comands[i].length = (int)strlen(comands[i].comanda);
    detect_comad_type(&comands[i]);
    extract_init_condition(&comands[i]);
  }

  free(buffer);

  for (int i = 0; i < num_of_comands; i++) {
    apply_comand(comands[i], s);
  }

  // for (int i = 0; i < num_of_comands; i++) {
  //   printf("Comanda: %s | Tip: %s | Lenght: %d | ", comands[i].comanda,
  //          comands[i].operation == SELECT   ? "SELECT"
  //          : comands[i].operation == UPDATE ? "UPDATE"
  //                                           : "DELETE",
  //          comands[i].length);

  //   for (int j = 0; j < comands[i].num_of_conditions; j++) {
  //     printf("Condition: %s ", comands[i].conditions[j]);
  //   }
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
  elibereaza_secretariat(&s);
  free_comands(comands, num_of_comands);
  return 0;
}
