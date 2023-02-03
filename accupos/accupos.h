#include <stdint.h>

#include "../libass/ass.h"

typedef struct {
    char *text;
    int32_t pos_x, pos_y;
    int32_t width, height;
} Accupos_Dialogue;

typedef struct {
    struct {
        ASS_Library *ass_library;
        ASS_Renderer *ass_renderer;
        ASS_Track *track;
    } libass;

    Accupos_Dialogue *dialogues;
    int32_t n_dialogues;
} Accupos_Library;

Accupos_Library *accupos_init(
    int32_t width, int32_t height,
    char *ass_data, int32_t ass_data_len
);

void accupos_done(Accupos_Library *lib);
