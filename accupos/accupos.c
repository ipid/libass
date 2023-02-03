#include "accupos.h"

#include <stdlib.h>
#include <memory.h>
#include <stdbool.h>

#include "../libass/ass_render.h"

static void libass_message_callback(int level, const char *fmt, va_list va, void *data) {
    // 什么也不做
}

static bool init(Accupos_Library *lib, int32_t width, int32_t height) {
    lib->libass.ass_library = ass_library_init();
    if (!lib->libass.ass_library) {
        return false;
    }
    ass_set_message_cb(lib->libass.ass_library, libass_message_callback, NULL);

    lib->libass.ass_renderer = ass_renderer_init(lib->libass.ass_library);
    if (!lib->libass.ass_renderer) {
        accupos_done(lib);
        return false;
    }

    ass_set_font_scale(lib->libass.ass_renderer, 1.0);
    ass_set_storage_size(lib->libass.ass_renderer, width, height);
    ass_set_frame_size(lib->libass.ass_renderer, width, height);
    ass_set_fonts(lib->libass.ass_renderer, NULL, "sans-serif",
                  ASS_FONTPROVIDER_AUTODETECT, NULL, 1);

    return true;
}

static bool copy_events_to_accupos(Accupos_Library *lib) {
    if (lib->dialogues) {
        return false;
    }

    ASS_Event *ass_events = lib->libass.track->events;
    if (!ass_events) {
        return false;
    }

    lib->n_dialogues = (int) lib->libass.track->n_events;
    if (lib->n_dialogues <= 0) {
        return false;
    }

    lib->dialogues = calloc(lib->n_dialogues, sizeof(*lib->dialogues));
    if (!lib->dialogues) {
        return false;
    }

    for (int i = 0; i < lib->n_dialogues; i++) {
        lib->dialogues[i].text = strdup(ass_events[i].Text);
        if (!lib->dialogues[i].text) {
            return false;
        }

        lib->dialogues[i].pos_x = ass_events[i].rendered_pos_x;
        lib->dialogues[i].pos_y = ass_events[i].rendered_pos_y;
        lib->dialogues[i].width = ass_events[i].rendered_width;
        lib->dialogues[i].height = ass_events[i].rendered_height;
    }

    return true;
}

static void free_libass_stuffs(Accupos_Library *lib) {
    if (lib->libass.track) {
        ass_free_track(lib->libass.track);
        lib->libass.track = NULL;
    }

    if (lib->libass.ass_renderer) {
        ass_renderer_done(lib->libass.ass_renderer);
        lib->libass.ass_renderer = NULL;
    }

    if (lib->libass.ass_library) {
        ass_library_done(lib->libass.ass_library);
        lib->libass.ass_library = NULL;
    }
}

Accupos_Library *accupos_init(int32_t width, int32_t height, char *ass_data, int32_t ass_data_len) {
    Accupos_Library *lib = (Accupos_Library *) calloc(1, sizeof(Accupos_Library));

    if (!lib) {
        return NULL;
    }

    if (!init(lib, width, height)) {
        accupos_done(lib);
        return NULL;
    }

    lib->libass.track = ass_read_memory(lib->libass.ass_library, ass_data, ass_data_len, "UTF-8");
    if (lib->libass.track == NULL) {
        accupos_done(lib);
        return NULL;
    }

    if (ass_render_all_events(lib->libass.ass_renderer, lib->libass.track) != 0) {
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
            free(lib->dialogues[i].text);
        }
        free(lib->dialogues);
    }

    free_libass_stuffs(lib);
    
    free(lib);
}
