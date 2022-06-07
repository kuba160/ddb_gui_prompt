#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include <deadbeef/deadbeef.h>
#include <deadbeef/artwork.h>

#include "cmd.h"
#include "cmd_tools.h"

#define CATIMG_CMD "catimg -w 64 \"%s\""

extern DB_gui_t plugin;
extern DB_functions_t *deadbeef;

ddb_artwork_plugin_t *artwork = NULL;

typedef struct CoverData_s {
    uintptr_t cond;
    ddb_cover_info_t *cover;
} CoverData_t;

static void cover_callback (int error, ddb_cover_query_t *query, ddb_cover_info_t *cover) {
    CoverData_t *cd = (CoverData_t *) query->user_data;
    cd->cover = cover;
    deadbeef->cond_signal(cd->cond);
}

char* escape_shell_arg (char* str) {
    char *esc = strdup(str);
    int i = 0;
    int len = strlen(str) + 1;

    while( *str ) {
        if(*str == '\\' || *str == '$') {
            esc[i++] = '\\';
            len += 1;
            esc = (char*)realloc(esc, len);
        }
        esc[i++] = *str++;
    }
    esc[i] = 0;
    return esc;
}

char * cmd_cover (int argc, char * argv[], int iter) {
    NO_TAB_COMPLETION
    if (argc < 1) {
        printf ("%s: displays cover of current track using catimg\n", cmd_name(argv[0]));
        return NULL;
    }

    if (system("which catimg > /dev/null 2>&1")) {
        printf("catimg: command not found in path!\n");
        return NULL;
    }

    if (!artwork) {
        artwork = (ddb_artwork_plugin_t *) deadbeef->plug_get_for_id("artwork2");
        if (!artwork) {
            printf("Artwork plugin missing!\n");
            return NULL;
        }
    }

    DB_playItem_t *it = deadbeef->streamer_get_playing_track();
    if (it) {
        ddb_cover_query_t *query = calloc(sizeof(ddb_cover_query_t),1);
        if (query) {
            uintptr_t cond = deadbeef->cond_create();
            uintptr_t mutex = deadbeef->mutex_create();
            CoverData_t cd = {cond, NULL};
            query->_size = sizeof(ddb_cover_query_t);
            query->user_data = &cd;
            query->track = it;

            artwork->cover_get(query, cover_callback);

            deadbeef->cond_wait(cond, mutex);

            free(query);
            query = NULL;
            deadbeef->cond_free(cond);
            deadbeef->mutex_free(mutex);

            if (cd.cover) {
                if (cd.cover->image_filename) {
                    char *escaped = escape_shell_arg(cd.cover->image_filename);
                    char str[strlen(CATIMG_CMD) + strlen(escaped) + 4];
                    snprintf(str, sizeof(str), CATIMG_CMD, escaped);
                    free (escaped);
                    //printf("exec: %s\n", str);
                    system(str);
                }
                else {
                    printf("No cover...\n");
                }
                artwork->cover_info_release(cd.cover);
            }
            else {
                printf ("Cover art plugin did not return any cover...\n");
            }
        }

        deadbeef->pl_item_unref(it);
    }
    return NULL;
}

