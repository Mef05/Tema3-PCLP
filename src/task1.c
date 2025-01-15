#include "task1.h"

#define MAX_LINE 256
#define MAX_NAME 40
#define THOUSAND 1000
#define TEN 10

float calculeaza_medie_student(int student_id, inrolare *inrolari,
                               int nr_inrolari) {
  int inrolari_student = 0;
  float medie = 0;
  for (int i = 0; i < nr_inrolari; i++) {
    if (inrolari[i].id_student == student_id) {
      inrolari_student++;
      medie +=
          (inrolari[i].note[0] + inrolari[i].note[1] + inrolari[i].note[2]);
    }
  }

  if (inrolari_student == 0) {
    return 0;
  }

  medie /= (float)inrolari_student;
  medie *= THOUSAND;

  while ((int)medie % TEN != 0) {
    if ((int)medie % TEN >= 4) {
      medie += 1;
    } else {
      medie -= 1;
    }
  }

  medie /= THOUSAND;

  return medie;
}

secretariat *citeste_secretariat(const char *nume_fisier) {
  FILE *f = fopen(nume_fisier, "r");
  if (f == NULL) {
    return NULL;
  }

  int nr_studenti = 0, nr_materii = 0, nr_inrolari = 0;

  char line[MAX_LINE];
  enum { STUDENTI, MATERII, INROLARI } section = STUDENTI;

  while (fgets(line, sizeof(line), f)) {
    if (strstr(line, "[STUDENTI]")) {
      section = STUDENTI;
    } else if (strstr(line, "[MATERII]")) {
      section = MATERII;
    } else if (strstr(line, "[INROLARI]")) {
      section = INROLARI;
    } else {
      switch (section) {
      case STUDENTI:
        nr_studenti++;
        break;

      case MATERII:
        nr_materii++;
        break;

      case INROLARI:
        nr_inrolari++;
        break;

      default:
        break;
      }
    }
  }

  secretariat *s = malloc(sizeof(secretariat));
  if (s == NULL) {
    fclose(f);
    return NULL;
  }

  s->nr_studenti = nr_studenti;
  s->studenti = malloc(nr_studenti * sizeof(student));
  if (s->studenti == NULL) {
    free(s);
    fclose(f);
    return NULL;
  }
  s->nr_materii = nr_materii;
  s->materii = malloc(nr_materii * sizeof(materie));
  if (s->materii == NULL) {
    free(s->studenti);
    free(s);
    fclose(f);
    return NULL;
  }
  s->nr_inrolari = nr_inrolari;
  s->inrolari = malloc(nr_inrolari * sizeof(inrolare));
  if (s->inrolari == NULL) {
    free(s->materii);
    free(s->studenti);
    free(s);
    fclose(f);
    return NULL;
  }

  rewind(f);

  while (fgets(line, sizeof(line), f)) {
    if (strstr(line, "[STUDENTI]")) {
      section = STUDENTI;
    } else if (strstr(line, "[MATERII]")) {
      section = MATERII;
    } else if (strstr(line, "[INROLARI]")) {
      section = INROLARI;
    } else {
      switch (section) {
      case STUDENTI: {
        sscanf(line, "%d, %39[^,], %d, %c",
               &s->studenti[s->nr_studenti - nr_studenti].id,
               (char *)&s->studenti[s->nr_studenti - nr_studenti].nume,
               &s->studenti[s->nr_studenti - nr_studenti].an_studiu,
               &s->studenti[s->nr_studenti - nr_studenti].statut);
        nr_studenti ? nr_studenti-- : 0;
      } break;
      case MATERII: {
        s->materii[s->nr_materii - nr_materii].nume =
            malloc(MAX_NAME * sizeof(char));
        s->materii[s->nr_materii - nr_materii].nume_titular =
            malloc(MAX_NAME * sizeof(char));
        sscanf(line, "%d, %[^,], %[^,\n]",
               &s->materii[s->nr_materii - nr_materii].id,
               s->materii[s->nr_materii - nr_materii].nume,
               s->materii[s->nr_materii - nr_materii].nume_titular);
        nr_materii ? nr_materii-- : 0;
        break;
      }
      case INROLARI: {
        sscanf(line, "%d, %d, %f %f %f",
               &s->inrolari[s->nr_inrolari - nr_inrolari].id_student,
               &s->inrolari[s->nr_inrolari - nr_inrolari].id_materie,
               &s->inrolari[s->nr_inrolari - nr_inrolari].note[0],
               &s->inrolari[s->nr_inrolari - nr_inrolari].note[1],
               &s->inrolari[s->nr_inrolari - nr_inrolari].note[2]);
        nr_inrolari ? nr_inrolari-- : 0;
        break;
      }
      default:
        break;
      }
    }
  }

  for (int i = 0; i < s->nr_studenti; i++) {
    s->studenti[i].medie_generala = calculeaza_medie_student(
        s->studenti[i].id, s->inrolari, s->nr_inrolari);
  }

  fclose(f);

  return s;
}

void adauga_student(secretariat *s, int id, char *nume, int an_studiu,
                    char statut, float medie_generala) {
  if (s == NULL)
    exit(1);

  s->nr_studenti++;
  s->studenti = realloc(s->studenti, (s->nr_studenti) * sizeof(student));
  if (s->studenti == NULL)
    exit(2);

  s->studenti[s->nr_studenti] = (student){
      .id = id,
      .an_studiu = an_studiu,
      .statut = statut,
      .medie_generala = medie_generala,
  };
  snprintf(s->studenti[s->nr_studenti].nume, strlen(nume) + 1, "%s", nume);
}

void elibereaza_secretariat(secretariat **s) {
  if (s == NULL || *s == NULL) {
    return;
  }

  free((*s)->studenti);
  for (int i = 0; i < (*s)->nr_materii; i++) {
    free((*s)->materii[i].nume);
    free((*s)->materii[i].nume_titular);
  }
  free((*s)->materii);
  free((*s)->inrolari);
  free(*s);
  *s = NULL;
}
