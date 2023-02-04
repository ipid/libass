#include "accupos.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../libass/ass_render.h"

typedef struct Accupos_LibassPrivate {
    ASS_Library *ass_library;
    ASS_Renderer *ass_renderer;
    ASS_Track *track;
} Accupos_LibassPrivate;

static void libass_message_callback(int level, const char *fmt, va_list va, void *data) {
    // 什么也不做
}

static bool init_libass_stuffs(Accupos_Library *lib, int32_t width, int32_t height) {
    lib->libass = calloc(1, sizeof(*lib->libass));
    if (!lib->libass) {
        return false;
    }

    lib->libass->ass_library = ass_library_init();
    if (!lib->libass->ass_library) {
        return false;
    }
    ass_set_message_cb(lib->libass->ass_library, libass_message_callback, NULL);

    lib->libass->ass_renderer = ass_renderer_init(lib->libass->ass_library);
    if (!lib->libass->ass_renderer) {
        accupos_done(lib);
        return false;
    }

    ass_set_font_scale(lib->libass->ass_renderer, 1.0);
    ass_set_storage_size(lib->libass->ass_renderer, width, height);
    ass_set_frame_size(lib->libass->ass_renderer, width, height);
    ass_set_fonts(lib->libass->ass_renderer, NULL, "sans-serif",
                  ASS_FONTPROVIDER_AUTODETECT, NULL, 1);

    return true;
}

static bool copy_events_to_accupos(Accupos_Library *lib) {
    if (lib->dialogues) {
        return false;
    }

    ASS_Event *ass_events = lib->libass->track->events;
    if (!ass_events) {
        return false;
    }

    lib->n_dialogues = (int) lib->libass->track->n_events;
    if (lib->n_dialogues <= 0) {
        return false;
    }

    lib->dialogues = calloc(lib->n_dialogues, sizeof(*lib->dialogues));
    if (!lib->dialogues) {
        return false;
    }

    for (int i = 0; i < lib->n_dialogues; i++) {
        if (!ass_events[i].raw_line) {
            return false;
        }
        lib->dialogues[i].raw = strdup(ass_events[i].raw_line);
        if (!lib->dialogues[i].raw) {
            return false;
        }

        lib->dialogues[i].pos_x = ass_events[i].rendered_pos_x;
        lib->dialogues[i].pos_y = ass_events[i].rendered_pos_y;
        lib->dialogues[i].width = ass_events[i].rendered_width;
        lib->dialogues[i].height = ass_events[i].rendered_height;
        lib->dialogues[i].is_positioned = ass_events[i].is_positioned;
    }

    return true;
}

static void free_libass_stuffs(Accupos_Library *lib) {
    if (!lib->libass) {
        return;
    }

    if (lib->libass->track) {
        ass_free_track(lib->libass->track);
        lib->libass->track = NULL;
    }

    if (lib->libass->ass_renderer) {
        ass_renderer_done(lib->libass->ass_renderer);
        lib->libass->ass_renderer = NULL;
    }

    if (lib->libass->ass_library) {
        ass_library_done(lib->libass->ass_library);
        lib->libass->ass_library = NULL;
    }

    free(lib->libass);
    lib->libass = NULL;
}

Accupos_Library *accupos_init(int32_t width, int32_t height, const char *ass_data, int32_t ass_data_len) {
    if (width <= 0 || height <= 0 || width >= 0x40000000 || height >= 0x40000000 || ass_data == NULL ||
        ass_data_len <= 8) {
        return NULL;
    }

    Accupos_Library *lib = (Accupos_Library *) calloc(1, sizeof(Accupos_Library));

    if (!lib) {
        return NULL;
    }

    if (!init_libass_stuffs(lib, width, height)) {
        accupos_done(lib);
        return NULL;
    }

    char *data_buf = calloc(ass_data_len + 1, sizeof(char));
    if (!data_buf) {
        accupos_done(lib);
        return NULL;
    }
    memcpy(data_buf, ass_data, ass_data_len);

    lib->libass->track = ass_read_memory(lib->libass->ass_library, data_buf, ass_data_len, "UTF-8");
    free(data_buf);

    if (lib->libass->track == NULL) {
        accupos_done(lib);
        return NULL;
    }

    if (accupos_render_all_events(lib->libass->ass_renderer, lib->libass->track) != 0) {
        accupos_done(lib);
        return NULL;
    }

    if (!copy_events_to_accupos(lib)) {
        accupos_done(lib);
        return NULL;
    }

    free_libass_stuffs(lib);
    return lib;
}

void accupos_done(Accupos_Library *lib) {
    if (!lib) {
        return;
    }

    if (lib->dialogues) {
        for (int i = 0; i < lib->n_dialogues; i++) {
            free((void *) lib->dialogues[i].raw);
            lib->dialogues[i].raw = NULL;
        }
        free(lib->dialogues);
        lib->dialogues = NULL;
    }

    free_libass_stuffs(lib);

    free(lib);
}
