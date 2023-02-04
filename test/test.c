#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "../accupos/accupos.h"

#define ASS_DATA_BUF (8 * 1024 * 1024)

char ass_data_buf[ASS_DATA_BUF];

int main(const int argv, const char **argc) {
    if (argv != 4) {
        printf("ERROR: argv != 4\n");
        return 1;
    }
    size_t bytes_read;
    {
        FILE *fp = fopen(argc[1], "r");
        bytes_read = fread(ass_data_buf, 1, ASS_DATA_BUF - 1, fp);
        if (bytes_read >= ASS_DATA_BUF - 1) {
            printf("ERROR: File too large.\n");
            return 1;
        }
        ass_data_buf[bytes_read] = '\0';
        fclose(fp);
    }

    long width = strtol(argc[2], NULL, 10);
    long height = strtol(argc[3], NULL, 10);
    if (width <= 0 || height <= 0 || width >= 0x40000000 || height >= 0x40000000) {
        printf("ERROR: Invalid width or height.\nwidth = %ld, height = %ld\n", width, height);
        return 1;
    }

    struct timespec before_init, after_init;
    clock_gettime(CLOCK_REALTIME, &before_init);
    Accupos_Library *accupos = accupos_init(
        (int32_t) width, (int32_t) height, ass_data_buf, (int32_t) bytes_read);
    clock_gettime(CLOCK_REALTIME, &after_init);

    printf("accupos initialization: %lld ms\n", (after_init.tv_sec - before_init.tv_sec) * 1000 + (after_init.tv_nsec - before_init.tv_nsec) / 1000000);

    for (int i = 0; i < accupos->n_dialogues; i++) {
        Accupos_Dialogue *dialogue = &accupos->dialogues[i];

        printf(
            "#%d: <%s>, pos_x = %f, pos_y = %f, width = %d, height = %d, positioned = %s\n",
            i + 1,
            dialogue->raw,
            dialogue->pos_x,
            dialogue->pos_y,
            dialogue->width,
            dialogue->height,
            dialogue->is_positioned ? "true" : "false"
        );
    }

    accupos_done(accupos);

    return 0;
}
