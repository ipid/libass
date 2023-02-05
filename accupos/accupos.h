#include <stdint.h>

#include "../libass/ass.h"

typedef struct {
    // Start: points to the slash \. E.g. '\\' of \rStyle
    // End: points to the end of style name. E.g. 'e' of \rStyle
    int32_t start, end;
} Accupos_RTag;

typedef struct {
    double pos_x, pos_y;
    const char *raw;

    Accupos_RTag *rtags;
    int32_t n_rtags;

    int32_t width, height;
    int32_t is_positioned;
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
