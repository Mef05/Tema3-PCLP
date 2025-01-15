#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/structuri.h"

void cripteaza_studenti(secretariat *secretariat, void *key, size_t key_len,
                        void *iv, size_t iv_len, char *cale_output);