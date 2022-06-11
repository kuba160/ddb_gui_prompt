/*
    Command prompt for DeaDBeeF
    Copyright (C) 2018 Jakub Wasylków <kuba_160@protonmail.com>

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <limits.h>
#include <float.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <deadbeef/deadbeef.h>

#include "common.h"
#include "cmd.h"
#include "cmd_tools.h"
#include "settings.h"
#include "cover.h"

extern DB_gui_t plugin;
extern DB_functions_t *deadbeef;
extern int ui_running;

typedef char *(*cmd_array[])(int, char *[], int);

const char * cmd_s[] = {"help", "play", "pause", "resume",
                        "stop", "next", "prev", "seek",
                        "signal", "volume", "quit", "list",
                        "playlist", "playlists", "info", "add_dir",
                        "settings","cover", 0};

cmd_array cmd_f = {cmd_help, cmd_play, cmd_play_pause, cmd_play_pause,
                   cmd_stop, cmd_next, cmd_prev, cmd_seek,
                   cmd_signal, cmd_volume, cmd_quit, cmd_list,
                   cmd_playlist, cmd_playlists, cmd_info, cmd_add_dir,
                   cmd_settings, cmd_cover, NULL};

const char * dirm_s[] = {"next", "prev", "exit", "list", "cd", "quit", "help",  NULL};

char cmd_path[128];
char * cmd_path_argv[16] = {NULL};

char * cmd_get_path () {
    if (!cmd_path_argv[0]) {
        return NULL;
    }
    cmd_path[0] = 0;
    strcat (cmd_path, cmd_path_argv[0]);
    if (!cmd_path_argv[1]) {strcat (cmd_path, "/");};
    int i;
    for (i = 1; cmd_path_argv[i] != 0; i++) {
        strcat (cmd_path, "/");
        strcat (cmd_path, cmd_path_argv[i]);
        // TODO check if not overflows
    }
    if (i == 0)
        strcat (cmd_path, "/");
    return cmd_path;
}

void cmd_change_path (int argc, char * argv[]) {
    int i;
    // special handling if ".." (could be more complete through...)
    if (argc == 1 && strcmp (argv[0],"..") == 0) {
        for (i = 0; cmd_path_argv[i] != 0; i++);
        int pos = i-1;
        if (pos >= 0) {
            if (cmd_path_argv[pos]) {
                free (cmd_path_argv[pos]);
            }
            cmd_path_argv[pos] = 0;
        }
        return;
    }
    for (i = 0; cmd_path_argv[i] != NULL; i++) {
        free (cmd_path_argv[i]);
    }
    if (argc == 0) {
        cmd_path_argv[0] = NULL;
        return;
    }
    int offset = 0;
    if (strcmp ("cd", cmd_name(argv[0])) == 0) {
        offset++;
    }
    for (i = 0; (i+offset) < argc; i++) {
        if (i == 0) {
            const char *dir = cmd_name(argv[i+offset]);
            if (strlen(dir)) {
                cmd_path_argv[i] = strdup(dir);
            }
            else {
                break;
            }
            continue;
        }
        cmd_path_argv[i] = strdup(argv[i+offset]);
    }
    cmd_path_argv[i] = NULL;
    return;
}

int cmd_num (char *cmd) {
    int i;
    for (i = 0; cmd_s[i] != NULL; i++){
        if (strcmp(cmd,cmd_s[i]) == 0)
            return i;
    }
    return -1;
}

const char *cmd_name (char *cmd) {
    int i = cmd_find (cmd);
    if (i >= 0)
        return cmd_s[i];
    // cannot return NULL as returned value can be evaluated directly
    return "";
}

const char *dirm_name (char *cmd) {
    int i;
    for (i = 0; dirm_s[i]; i++) {
        if (strncmp (cmd, dirm_s[i], strlen (cmd)) == 0)
            return dirm_s[i];
    }
    return "";
}

char * cmd_play (int argc, char * argv[], int iter) {
    TAB_COMPLETION_ATA (1,3,2);
    TAB_COMPLETION_END
    if (argc <= 0) {
        printf ("%s: starts playback\n", cmd_name(argv[0]));
        printf ("Defaults: start playing current track\n");
        printf ("\t1: track num (optional)\n");
        printf ("OR\n");
        printf ("\t1: artist\n\t2: album (optional)\n\t3: title (optional)\n\tUse \"*\" to search for specific title/album\n");
        return NULL;
    }
    if (argc == 1) {
        deadbeef->sendmessage (DB_EV_PLAY_CURRENT, 0, 0, 0);
    }
    else if (argc == 2) {
        ddb_playlist_t *plt = deadbeef->plt_get_curr();
        DB_playItem_t* find = cmd_get_item (NULL, argv[1], NULL, NULL);
        int num = 0;
        if (!find) {
            deadbeef->plt_deselect_all (plt);
            DB_playItem_t *item = deadbeef->pl_get_for_idx (atoi(argv[1]));
            deadbeef->pl_set_selected (item, 1);
            deadbeef->pl_item_unref (item);
            deadbeef->plt_unref (plt);
            num = atoi(argv[1]);
        }
        else {
            num = deadbeef->plt_get_item_idx (plt, find, PL_MAIN);
            deadbeef->pl_item_unref (find);
            deadbeef->plt_unref (plt);
        }
        deadbeef->sendmessage (DB_EV_PLAY_NUM, 0, num, 0);
    }
    else if (argc <= 4 && argc >= 3) {
        ddb_playlist_t *plt = deadbeef->plt_get_curr();
        char *title_o = (argc == 4) ? argv[3] : NULL;
        DB_playItem_t* find = cmd_get_item (NULL, argv[1], argv[2], title_o);
        int num = 0;
        if (find) {
            num = deadbeef->plt_get_item_idx (plt, find, PL_MAIN);
            deadbeef->pl_item_unref (find);
            deadbeef->sendmessage (DB_EV_PLAY_NUM, 0, num, 0);
        }
        deadbeef->plt_unref (plt);
    }
    return NULL;
}

char * cmd_play_pause (int argc, char * argv[], int iter) {
    NO_TAB_COMPLETION
    if (argc != 1) {
        if (cmd_find(argv[0]) == cmd_num("resume"))
            printf ("%s: unpauses playback\n", cmd_name(argv[0]));
        else
            printf ("%s: pauses/unpauses playback\n", cmd_name(argv[0]));
        return NULL;
    }
    if (cmd_find(argv[0]) == cmd_num("resume")) {
        struct DB_output_s* output = deadbeef->get_output ();
        if (output->state() != DDB_PLAYBACK_STATE_PAUSED)
            return NULL;
    }
    deadbeef->sendmessage (DB_EV_TOGGLE_PAUSE, 0, 0, 0);
    return NULL;
}

char * cmd_stop (int argc, char * argv[], int iter) {
    NO_TAB_COMPLETION
    if (argc != 1) {
        printf ("%s: stops playback\n", cmd_name(argv[0]));
        return NULL;
    }
    deadbeef->sendmessage (DB_EV_STOP, 0, 0, 0);
    return NULL;
}

char * cmd_next (int argc, char * argv[], int iter) {
    NO_TAB_COMPLETION
    // TODO: take argument to go for example 2 tracks after
    if (argc != 1) {
        printf ("%s: play next track\n", cmd_name(argv[0]));
        return NULL;
    }
    deadbeef->sendmessage (DB_EV_NEXT, 0, 0, 0);
    return NULL;
}

char * cmd_prev (int argc, char * argv[], int iter) {
    NO_TAB_COMPLETION
    // TODO: take argument to go for example 2 tracks before
    if (argc != 1) {
        printf ("%s: play previous track\n", cmd_name(argv[0]));
        return NULL;
    }
    deadbeef->sendmessage (DB_EV_PREV, 0, 0, 0);
    return NULL;
}

char * cmd_seek (int argc, char * argv[], int iter) {
    NO_TAB_COMPLETION
    if (argc != 2) {
        printf ("%s: seek track\n", cmd_name(argv[0]));
        printf ("\t1: seconds to seek from current position\n");
        return NULL;
    }
    DB_playItem_t* curr_track = deadbeef->streamer_get_playing_track ();
    if (!curr_track) {
        return NULL;
    }
    float item_length = deadbeef->pl_get_item_duration (curr_track) * 1000;
    deadbeef->pl_item_unref (curr_track);
    float pos = deadbeef->streamer_get_playpos () * 1000;
    if (argv[1][0] == '+') {
        pos += (atoi (argv[1]+1) * 1000);
    }
    if (argv[1][0] == '-') {
        pos -= (atoi (argv[1]+1) * 1000);
    }
    else {
        // assume it's a positive num
        pos += (atoi (argv[1]) * 1000);
    }
    if (pos < 0.0f)
        pos = 0.0;
    if (pos > item_length)
        deadbeef->sendmessage (DB_EV_NEXT, 0, 0, 0);
    deadbeef->sendmessage (DB_EV_SEEK, 0, pos, 0);
    return NULL;
}

const char *events[] = { "", "DB_EV_NEXT", "DB_EV_PREV", "DB_EV_PLAY_CURRENT", "DB_EV_PLAY_NUM", "DB_EV_STOP", "DB_EV_PAUSE", "DB_EV_PLAY_RANDOM", "DB_EV_TERMINATE", "DB_EV_PLAYLIST_REFRESH", "DB_EV_REINIT_SOUND", "DB_EV_CONFIGCHANGED", "DB_EV_TOGGLE_PAUSE", "DB_EV_ACTIVATED", "DB_EV_PAUSED", "DB_EV_PLAYLISTCHANGED", "DB_EV_VOLUMECHANGED", "DB_EV_OUTPUTCHANGED", "DB_EV_PLAYLISTSWITCHED", "DB_EV_SEEK", "DB_EV_ACTIONSCHANGED", "DB_EV_DSPCHAINCHANGED", "DB_EV_SELCHANGED", "DB_EV_PLUGINSLOADED", "DB_EV_FOCUS_SELECTION", 0};

char * cmd_signal (int argc, char * argv[], int iter) {
    TAB_COMPLETION_TABLE (1, events+1);
    TAB_COMPLETION_END
    if (argc <= 1) {
        printf ("%s: sends message event, arguments:\n", cmd_name(argv[0]));
        printf ("\t1: event: either number or name\n");
        printf ("\t2: p1: number\n");
        printf ("EXAMPLE: signal DB_EV_PLAY_CURRENT\n");
        return NULL;
    }
    if (argv[1][0] > 48 && argv[1][0] < 57) {
        // is probably a num
        // with p1
        if (argc == 3)
            deadbeef->sendmessage (atoi(argv[0]), 0, atoi(argv[1]), 0);
        else
            deadbeef->sendmessage (atoi(argv[0]), 0, 0, 0);
    }
    else {
        // can be event;
        int i;
        for (i = 1; events[i] != 0; i++) {
            if (strcmp (argv[1], events[i]) == 0) {
                if (argc == 3) {
                    deadbeef->sendmessage (i, 0, atoi(argv[2]), 0);
                    return NULL;
                }
                else {
                    deadbeef->sendmessage (i, 0, 0, 0);
                    return NULL;
                }
            }
        }
        printf ("unknown signal %s\n",argv[1]);
    }
    return NULL;
}

char * cmd_volume (int argc, char * argv[], int iter) {
    NO_TAB_COMPLETION
    if (argc < 1) {
        printf ("%s: read or set playback volume\n", cmd_name(argv[0]));
        printf ("\t1: volume in %% or dB\n");
        printf ("\t   prepend with + or - to set relative to current volume (in %%)\n");
    }
    else if (argc == 1) {
        printf ("%.0f%% (%.2f dB)\n", deadbeef->volume_get_db() * 2 + 100 , deadbeef->volume_get_db());
    }
    else if (argc == 2) {
        int vol_calc = -1;
        if (argv[1][0] == '+') {
            vol_calc = deadbeef->volume_get_db() * 2 + 100;
            vol_calc += atoi (argv[1]+1);
        }
        if (argv[1][0] == '-') {
            vol_calc = deadbeef->volume_get_db() * 2 + 100;
            vol_calc -= atoi (argv[1]+1);
        }
        if (vol_calc != -1) {
            deadbeef->volume_set_db ((vol_calc/100.0 * 50) - 50);
            return NULL;
        }
        int pct;
        char *end;
        pct = strtol (argv[1], &end, 10);
        if (!strcasecmp(end, "db")) {
            deadbeef->volume_set_db (pct);
        } else {
            deadbeef->volume_set_db ((pct/100.0 * 50) - 50);
        }
    }
    return NULL;
}

char * cmd_help (int argc, char * argv[], int iter) {
    TAB_COMPLETION_TABLE(1,cmd_s);
    TAB_COMPLETION_END
    if (argc < 1) {
        printf ("%s: generates help page :)\n", cmd_name(argv[0]));
        printf ("\t 1: (optional) command name\n");
        return NULL;
    }
    if (argc == 1) {
        printf ("DeaDBeeF prompt v%d.%d\n",plugin.plugin.version_major, plugin.plugin.version_minor);
        printf ("Available commands:");
        int i;
        for (i = 1; cmd_s[i] != 0; i++) {
            printf (" %s", cmd_s[i]);
        }
        printf (".\n");
        printf ("For more information about specific command write \"help command\"\n");
        printf ("Commands get automatically extended, f. ex. \"vol\" gets extended to \"volume\".\n");
        printf ("Tab completion is also available.\n");
        // TODO: better links (aliases) and info
    }
    else {
        // argv[1] is command help
        int i = cmd_find (argv[1]);
        if (i == FIND_NOTFOUND)
            printf ("%s: command %s not found\n", cmd_name(argv[0]), argv[1]);
        else if (i == FIND_AMBIGUOUS)
            cmd_ambiguous_print (argv[1]);
        else {
            char * empty[] = {argv[1], NULL};
            cmd_f[i] (-1, empty, -1);
        }
    }
    return NULL;
}

char * cmd_quit (int argc, char * argv[], int iter) {
    NO_TAB_COMPLETION
    if (argc < 0) {
        printf ("%s: exits player\n", cmd_name(argv[0]));
        return NULL;
    }
    // note for (long) future: check for background jobs
    deadbeef->sendmessage (DB_EV_TERMINATE, 0, 0, 0);
    ui_running = 0;
    return NULL;
}

char * cmd_list (int argc, char * argv[], int iter) {
    TAB_COMPLETION_PLAYLIST(1);
    TAB_COMPLETION_END
    if (argc < 0) {
        printf ("%s: lists playlist tracks (default: current playlist)\n", cmd_name(argv[0]));
        printf ("\t1: (optional) playlist number or playlist name\n");
        return NULL;
    }
    if (argc == 1 || argc == 2) {
        ddb_playlist_t *plt;
        if (argc == 1) {
            plt = deadbeef->plt_get_curr();
        }
        else {
            plt = cmd_get_playlist (argv[1]);
            if (!plt)
              plt = deadbeef->plt_get_for_idx(atoi(argv[1]));
        }
        ddb_tf_context_t context;
        context._size = sizeof(ddb_tf_context_t);
        context.flags = 0;
        //context.it = nowplaying;
        context.plt = plt;
        context.idx = 0;
        context.id = 0;
        context.iter = PL_MAIN;
        context.update = 0;
        context.dimmed = 0;
        DB_playItem_t *item = deadbeef->plt_get_first (plt, PL_MAIN);
        if (!item) {
            if (plt)
                deadbeef->plt_unref (plt);
            return NULL;
        }
        char * script = "%tracknumber%. %artist% - %title% /// %album%\n";
        char * code_script = deadbeef->tf_compile (script);
        int i = 0;
        char buffer[256];
        do {
            int offset = sprintf (buffer, "%02d: ",i);
            context.it = item;
            deadbeef->tf_eval (&context, code_script, buffer+offset, 250);
            printf ("%s\n",buffer);
            DB_playItem_t *new = deadbeef->pl_get_next (item, PL_MAIN);
            deadbeef->pl_item_unref (item);
            if (new) {
                i++;
                item = new;
            }
            else {
                break;
            }
        }
        while (1);

        deadbeef->tf_free (code_script);
        if (plt)
            deadbeef->plt_unref (plt);
    }
    return NULL;
}

char * cmd_playlist (int argc, char * argv[], int iter) {
    TAB_COMPLETION_PLAYLIST(1);
    TAB_COMPLETION_END
    if (argc < 1) {
        printf ("%s: sets current playlist\n", cmd_name(argv[0]));
        printf ("\t1: playlist number OR offset OR name\n");
        return NULL;
    }
    else if (argc == 1) {
        DB_playItem_t* curr_track = deadbeef->streamer_get_playing_track ();
        if (curr_track) {
            ddb_playlist_t* playlist = deadbeef->pl_get_playlist (curr_track);
            deadbeef->plt_set_curr (playlist);
            deadbeef->plt_unref (playlist);
            deadbeef->pl_item_unref (curr_track);
            return NULL;
        }
        else {
            ddb_playlist_t* playlist = deadbeef->plt_get_curr ();
            char buffer[256];
            deadbeef->plt_get_title (playlist, buffer, 256);
            deadbeef->plt_unref (playlist);
            printf ("Current playlist: %s\n",buffer);
        }
        return NULL;
    }
    else if (argc == 2) {
        ddb_playlist_t* find = cmd_get_playlist (argv[1]);
        if (!find) {
            int num = atoi (argv[1]);
            if (argv[1][0] == '+') {
                num = deadbeef->plt_get_curr_idx ();
                num += atoi (argv[1]+1);
            }
            if (argv[1][0] == '-') {
                num = deadbeef->plt_get_curr_idx ();
                num -= atoi (argv[1]+1);
            }
            ddb_playlist_t *plt = deadbeef->plt_get_for_idx (num);
            if (plt) {
                deadbeef->plt_set_curr (plt);
                deadbeef->plt_unref (plt);
            }
        }
        else {
            deadbeef->plt_set_curr (find);
            deadbeef->plt_unref (find);
        }
    }
    return NULL;
}

char * cmd_playlists (int argc, char * argv[], int iter) {
    NO_TAB_COMPLETION
    if (argc <= 0) {
        printf ("%s: lists all playlists\n", cmd_name(argv[0]));
        return NULL;
    }
    else if (argc == 1) {
        ddb_playlist_t* playlist = deadbeef->plt_get_for_idx (0);
        int i = 0;
        char buffer[256];
        do {
            int offset = sprintf (buffer, "%02d: ",i);
            deadbeef->plt_get_title (playlist, buffer+offset, 256-offset);
            printf ("%s\n",buffer);
            ddb_playlist_t* new = deadbeef->plt_get_for_idx (++i);
            deadbeef->plt_unref (playlist);
            if (new) {
                playlist = new;
            }
            else {
                break;
            }
        }
        while (1);
    }
    return NULL;
}

char * cmd_info (int argc, char * argv[], int iter) {
    NO_TAB_COMPLETION
    if (argc < 0) {
        printf ("%s: shows current position, including playlist\n", cmd_name(argv[0]));
        return NULL;
    }
    if (argc == 1) {
        struct DB_output_s* output = deadbeef->get_output ();
        int state = output->state ();
        const char * state_s;
        switch (state) {
            case OUTPUT_STATE_STOPPED:
                state_s = KRED "Stopped" KNRM;
                break;
            case OUTPUT_STATE_PAUSED:
                state_s = KYEL "Paused" KNRM;
                break;
            case OUTPUT_STATE_PLAYING:
            default:
                state_s = KGRN "Playing" KNRM;
                break;
        }
        printf ("Playback state: %s\n", state_s);

        DB_playItem_t* curr_track = deadbeef->streamer_get_playing_track ();
        ddb_playlist_t *plt;
        if (curr_track)
            plt = deadbeef->pl_get_playlist (curr_track);
        else
            plt = deadbeef->plt_get_curr ();

        ddb_tf_context_t context;
        context._size = sizeof(ddb_tf_context_t);
        context.flags = 0;
        context.it = curr_track;
        context.plt = plt;
        context.idx = 0;
        context.id = 0;
        context.iter = PL_MAIN;
        context.update = 0;
        context.dimmed = 0;

        if (state != OUTPUT_STATE_STOPPED) {
            char * script = "%artist% - %title%\n";
            char * code_script = deadbeef->tf_compile (script);
            char buffer[256];

            deadbeef->tf_eval (&context, code_script, buffer, 256);
            deadbeef->tf_free (code_script);
            printf ("%s\n",buffer);

            float item_length = deadbeef->pl_get_item_duration (curr_track);
            if (item_length < 0.1) {
                item_length = FLT_MAX;
            }
            float playback_pos = deadbeef->streamer_get_playpos ();
            float percent = playback_pos/item_length;
            #define PROGRESSBAR_LEN 20
            int progress = (int) (round(percent * PROGRESSBAR_LEN));
            char bar[PROGRESSBAR_LEN+1];
            bar[PROGRESSBAR_LEN] = 0;
            memset (bar, '#', progress);
            memset (bar+progress, '-', PROGRESSBAR_LEN-progress);
            printf ("|%s|\n", bar);
            char time[16];
            char time2[16];
            deadbeef->pl_format_time (playback_pos, time, 16);
            deadbeef->pl_format_time (item_length-playback_pos, time2, 16);
            // hard-coded value: 17
            printf("%s %17s\n", time, time2);
        }

        printf ("\n");
        printf ("No  Track  Artist  Title\t\tAlbum\n");

        char * script = "%tracknumber%.  %artist% - %title% /// %album%\n";
        char * code_script = deadbeef->tf_compile (script);

        int num = deadbeef->plt_get_item_idx (plt, curr_track, PL_MAIN);
        // display 4 tracks before and after
        int num_first = num - 4;
        if (num_first < 0)
            num_first = 0;
        DB_playItem_t *item = deadbeef->plt_get_item_for_idx (plt, num_first, PL_MAIN);
        if (!item) {
            // empty or failed
            printf ("Empty.\n");
            deadbeef->tf_free (code_script);
            if (plt)
                deadbeef->plt_unref (plt);
            if (curr_track)
                deadbeef->pl_item_unref (curr_track);
            return NULL;
        }
        int i = num_first;
        int printed_count = 0;
        char buffer[256];
        do {
            int offset = 0;
            if (i == num) {
                if (state == OUTPUT_STATE_PLAYING)
                    offset = sprintf (buffer, KBLD SYMBOL_PLAY " : ");
                else if (state == OUTPUT_STATE_PAUSED)
                    offset = sprintf (buffer, KBLD SYMBOL_PAUSED " : ");
                else if (state == OUTPUT_STATE_STOPPED)
                    offset = sprintf (buffer, KBLD SYMBOL_STOPPED " : ");
            }
            else {
                offset = sprintf (buffer, "%02d: ",i);
            }
            context.it = item;
            deadbeef->tf_eval (&context, code_script, buffer+offset, 250);
            printf ("%s\n" KNRM,buffer);
            printed_count++;

            deadbeef->pl_item_unref (item);
            if (printed_count >= 9) {
                break;
            }
            DB_playItem_t *new = deadbeef->pl_get_next (item, PL_MAIN);

            if (new) {
                i++;
                item = new;
            }
            else {
                printf ("================\n");
                break;
            }
        }
        while (1);

        deadbeef->tf_free (code_script);
        if (plt)
            deadbeef->plt_unref (plt);
        if (curr_track)
            deadbeef->pl_item_unref (curr_track);
    }
    return NULL;
}

char * cmd_add_dir (int argc, char * argv[], int iter) {
    NO_TAB_COMPLETION
    if (argc <= 0) {
        printf ("%s: adds directory to current playlist\n", cmd_name(argv[0]));
        printf ("\t1: directory to add\n");
        return NULL;
    }
    // argv[1] is dirname (if exists)
    if (argc == 2) {
        // visibility = -1
        // todo callback: int (*callback)(DB_playItem_t *it, void *user_data), void *user_data
        ddb_playlist_t* plt = deadbeef->plt_get_curr ();
        deadbeef->plt_add_dir2 (-1, plt, argv[1], NULL, NULL);
    }
    else {
        printf ("%s: too many arguments passed\n", cmd_name(argv[0]));
    }

    return NULL;
}

char * cmd_dirm (int argc, char * argv[], int iter) {
    if (argc == 2 && iter != -1) {
        if (strcmp (argv[0], "cd") == 0) {
            // todo fix changin dir
            int num = cmd_num (cmd_path_argv[0]);
            if (num == -1) {
                return NULL;
            }
            char ** argv_send = argv_alloc (NULL);
            argv_cat (argv_send, cmd_path_argv);
            argv_cat (argv_send, argv+1);// (char *[]){"", NULL});
            int argc_send = argv_count (argv_send);
            char *ret = cmd_f[num] (argc_send, argv_send, iter);
            argv_free (argv_send);
            return ret;
        }
        
    }
    NO_TAB_COMPLETION

    if (argc >= 1) {
        //const char * dirm_s[] = {"next", "prev", "exit", "list", "cd", NULL};
        if (strcmp (dirm_name(argv[0]), "exit") == 0 || strcmp (dirm_name(argv[0]), "quit") == 0) {
            if (argc == 1) {
                // cd to main dir
                char ** empty = {NULL};
                cmd_change_path (0, empty);
                return NULL;
            }
        }
        else if (strcmp (dirm_name(argv[0]), "cd") == 0) {
            if (argc == 1) {
                // cd to main main dir
                char main_dir[strlen(cmd_path_argv[0])+1];
                strcpy (main_dir, cmd_path_argv[0]);
                char * argument[] = {main_dir, NULL};
                cmd_change_path (1, argument);
                return NULL;
            }
            if (argc == 2) {
                cmd_change_path (1, argv+1);
            }
        }
        else if (strcmp (dirm_name(argv[0]), "list") == 0) {
            if (argc == 1) {
                // iter through cmd_path_argv
                int num = cmd_num (cmd_path_argv[0]);
                if (num == -1) {
                    return NULL;
                }
                char ** argv_send = argv_alloc (NULL);
                argv_cat (argv_send, cmd_path_argv);
                argv_cat (argv_send, (char *[]){"", NULL});
                int argc_send = argv_count (argv_send);
                int iter = 0;
                char * opt = 0;
                for (iter = 0; (opt = cmd_f[num] (argc_send, argv_send, iter)); iter++ ) {
                    printf ("%s ", opt);
                    if (!(iter % 4) && iter != 0) {
                        printf ("\n");
                    }
                }
                printf ("\n");
                argv_free (argv_send);
                return NULL;
            }
            if (argc == 2) {
                // todo concat cmd_path_argv and argv
                // list specified dir
                //cmd_change_path (1, argv+1);
            }
        }
        else if (strcmp (dirm_name(argv[0]), "help") == 0) {
            if (argc == 1) {
                // print help of current directory
                int num = cmd_num (cmd_path_argv[0]);
                if (num < 0) {
                    //printf
                    return CMD_NOTFOUND;
                }
                cmd_f[num] (-1, cmd_path_argv, -1);
                return NULL;
            }
            else {
                printf ("help does n");
            }
            if (argc == 2) {
                cmd_change_path (1, argv+1);
            }
        }
    }
    return NULL;
}

char * cmd_generator (const char *text, int state) {
    static int list_index, len;
    const char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = cmd_s[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }
    return NULL;
}

int cmd_completed_num = 0;

char * cmd_completion_iter (const char *text, int state) {

    char **argv = argv_alloc (NULL);
    argv_cat (argv, cmd_path_argv);
    char ** argv_2 = argv_alloc (rl_line_buffer);

    if (state == 0) {
        cmd_completed_num = INT_MAX;
    }

    // j = argv_2 NULL - last element
    int j;
    for (j = 0; argv_2[j] != NULL; j++);

    // hack? check if we have space on end, interpret it as searching for next argv began
    // bugfix: check if we got escaped, if that's the case we still look for the same argv!
    int escaped = 0;
    {
        int i;
        int escape_count = 0;
        for (i = 0; rl_line_buffer[i]; i++) {
            if (rl_line_buffer[i] == '\"') {
                escape_count++;
            }
        }
        if (escape_count % 2) {
            escaped = 1;
        }
    }
    if (!escaped) {
        char *zeroptr = strrchr (rl_line_buffer, 0);
        if (zeroptr && strlen(rl_line_buffer) != 0) {
            if (*(zeroptr-1) == ' ') {
                // search for next string
                argv_2[j] = strdup ("");
                j++;
                argv_2[j] = NULL;
            }
        }
        else if (rl_line_buffer[0] == 0) {
            // empty line
            argv_2[j] = strdup ("");
            j++;
            argv_2[j] = NULL;
        }
    }

    argv_cat (argv, argv_2);
    int argc = argv_count (argv);

    if (argc == 1) {
        // function completion
        char * ret = 0;
        if (argv[1] == NULL) // cmd not complete
            ret = cmd_generator (text, state);
        argv_free (argv);
        argv_free (argv_2);
        return ret;
    }
    // function-specific completion
    int num = cmd_num ((char *)cmd_name(argv[0]));
    if (num == -1) {
        // invalid cmd
        argv_free (argv);
        argv_free (argv_2);
        return NULL;
    }
    char * ret = 0;
    if (cmd_completed_num == INT_MAX) {
        ret = cmd_f[num](argc, argv, state);
    }
    // cmd_completed_num - if we are in directory add global command to completion
    if (!ret && cmd_completed_num == INT_MAX) {
        cmd_completed_num = state;
    }
    if (!ret && cmd_completed_num != INT_MAX) {
        // global function completion or already in global function?
        if (j >= 2) {
            ret = cmd_dirm (argc-1, argv+1, state-cmd_completed_num);
        }
        else {
            ret = cmd_tab_complete_table (dirm_s, argv_2, state-cmd_completed_num);
        }
    }
    
    argv_free (argv);
    argv_free (argv_2);
    return ret;
}

char ** cmd_completion (const char *text, int start, int end) {
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, cmd_completion_iter); 
}

int cmd_find (char * cmd) {
    int i;
    int exact_cmd = -1;
    int possible_cmds[10];
    int poss_p = 0;
    memset (possible_cmds, -1, sizeof (int) * 10);
    possible_cmds[9] = 0;
    // exceptions
    char * links_s[] = {"p", "pl", "pls", "pp", "ls", NULL};
    char * links_n[] = {"play", "playlist", "playlists", "pause", "list", NULL};

    // find exact name or possible names;
    for (i = 0; cmd_s[i] != 0; i++) {
        if (strcmp(cmd, cmd_s[i]) == 0) {
            exact_cmd = i;
            // value can be overridden by dirm
        }
        if (strncmp(cmd, cmd_s[i], strlen(cmd)) == 0) {
            possible_cmds[poss_p] = i;
            poss_p++;
        }
        int links_i;
        for (links_i = 0; links_s[links_i] != NULL; links_i++) {
            if (strcmp(cmd, links_s[links_i]) == 0) {
                exact_cmd = cmd_num(links_n[links_i]);
                break;
            }
        }
        // dir navigation
        if (cmd_path_argv[0]) {
            int dirm_i;
            for (dirm_i = 0; dirm_s[dirm_i] != NULL; dirm_i++) {
                if (strcmp(cmd, dirm_s[dirm_i]) == 0) {
                    exact_cmd = FIND_DIRM;
                    break;
                }
                else if (strncmp(cmd, dirm_s[dirm_i], strlen(cmd)) == 0) {
                    int exists = 0;
                    {
                        int h;
                        for (h = 0; possible_cmds[h]; h++) {
                            if (possible_cmds[h] == FIND_DIRM) {
                                exists = 1;
                                break;
                            }
                        }
                    }
                    if (!exists) {
                        // hack: enforce dirm when manually detected that we are in dir
                        if (cmd_path_argv[0]) {
                            exact_cmd = FIND_DIRM;
                            break;
                        }
                        possible_cmds[poss_p] = FIND_DIRM;
                        poss_p++;
                    }
                }
            }
        }
        if (exact_cmd != -1) {
            break;
        }
    }
    if (exact_cmd != -1) {
        return exact_cmd;
    }
    else if (possible_cmds[0] == -1) {
        return FIND_NOTFOUND;
    }
    else if (possible_cmds[1] == -1) {
        return possible_cmds[0];
    }
    else {
        // use cmd_ambiguous_print to show available commands
        return FIND_AMBIGUOUS;
    }
}

int cmd_ambiguous_print (char * cmd) {
    int possible_cmds[10];
    int poss_p = 0;
    memset (possible_cmds, -1, sizeof (int) * 10);
    int i;
    for (i = 0; cmd_s[i] != 0; i++) {
        if (strncmp(cmd, cmd_s[i], strlen(cmd)) == 0) {
            possible_cmds[poss_p] = i;
            poss_p++;
        }
    }
    if (possible_cmds[0] == -1) {
        return FIND_NOTFOUND;
    }
    else if (possible_cmds[1] == -1) {
        return possible_cmds[0];
    }
    else {
        printf ("Ambigous command: %s\n", cmd);
        printf ("Possible commands:");
        int j;
        for (j = 0; possible_cmds[j] != -1; j++) {
            printf (" %s", cmd_s[possible_cmds[j]]);
        }
        printf ("\n");
        return FIND_AMBIGUOUS;
    }
}

int cmd (char * buffer) {
    // build argv list
    //printf ("cmd: %s\n",buffer);

    char **argv = argv_alloc (NULL);
    argv_cat (argv, cmd_path_argv);
    char ** argv_2 = argv_alloc (buffer);
    argv_cat (argv, argv_2);

    int argc = argv_count (argv);
    if (argc == 0) {
        if (argv) {
            argv_free (argv);
        }
        if (argv_2) {
            argv_free (argv_2);
        }
        return -1;
    }

    int find_ret = cmd_find (argv[0]);
    int second_ret = 0;
    if (argv_2[0])
        second_ret = cmd_find (argv_2[0]);

    int out_ret = 0;
    if (second_ret == FIND_DIRM) {
        cmd_dirm (argv_count (argv_2), argv_2, -1);
        out_ret = 0;
    }
    else if (find_ret == FIND_NOTFOUND) {
        out_ret = -1;
    }
    else if (find_ret == FIND_AMBIGUOUS) {
        cmd_ambiguous_print (argv[0]);
        out_ret = -2;
    }
    else {
        // find_ret is command num
        char * cmd_ret = cmd_f[find_ret] (argc, argv, -1);
        if (cmd_ret) {
            if (strcmp (cmd_ret, CMD_DIRECTORY) == 0) {
                //
                cmd_change_path (argc, argv);
                out_ret = 0;
            }
            else if (strcmp (cmd_ret, CMD_NOTFOUND) == 0 && cmd_path_argv[0]) {
                out_ret = -1;
            }
            else {
                out_ret = 0;
            }
        }
    }

    argv_free (argv);
    argv_free (argv_2);
    return out_ret;
}
