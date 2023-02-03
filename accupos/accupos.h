#include <stdint.h>

#include "../libass/ass.h"

typedef struct {
    const char *raw;
    int32_t pos_x, pos_y;
    int32_t width, height;
} Accupos_Dialogue;

typedef struct Accupos_LibassPrivate Accupos_LibassPrivate;

typedef struct {
    Accupos_LibassPrivate *libass;
    Accupos_Dialogue *dialogues;
    int32_t n_dialogues;
} Accupos_Library;

Accupos_Library *accupos_init(
    int32_t width, int32_t height,
    const char *ass_data, int32_t ass_data_len
);

void accupos_done(Accupos_Library *lib);

int32_t accupos_get_dialogue_num(Accupos_Library *lib);
const char *accupos_get_ith_raw(Accupos_Library *lib, int32_t i);
int32_t accupos_get_ith_pos_x(Accupos_Library *lib, int32_t i);
int32_t accupos_get_ith_pos_y(Accupos_Library *lib, int32_t i);
int32_t accupos_get_ith_width(Accupos_Library *lib, int32_t i);
int32_t accupos_get_ith_height(Accupos_Library *lib, int32_t i);
