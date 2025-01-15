#pragma once

#include "../include/structuri.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

secretariat *citeste_secretariat(const char *nume_fisier);

void adauga_student(secretariat *s, int id, char *nume, int an_studiu,
                    char statut, float medie_generala);

void elibereaza_secretariat(secretariat **s);