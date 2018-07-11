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
#include <math.h>

#include <readline/readline.h>
#include <readline/history.h>
#include <deadbeef/deadbeef.h>
#include "cmd.h"

#define USE_COLORS
#ifdef USE_COLORS
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KBLD  "\x1B[1m" // bold
#else
#define KNRM ""
#define KRED ""
#define KGRN ""
#define KYEL ""
#define KBLU ""
#define KMAG ""
#define KCYN ""
#define KWHT ""
#define KBLD ""
#endif

//#define USE_UNICODE_SYMBOLS
#ifdef USE_UNICODE_SYMBOLS
#define SYMBOL_PLAY "▶"
#define SYMBOL_PAUSED "⏸"
#define SYMBOL_STOPPED "◾"
#else
#define SYMBOL_PLAY ">"
#define SYMBOL_PAUSED "/"
#define SYMBOL_STOPPED ">"
#endif

extern DB_gui_t plugin;
extern DB_functions_t *deadbeef;
extern int ui_running;

char * cmd_s[] = {"help", "play", "pause", "resume", "stop", "next", "prev", "seek", "signal", "volume", "quit", "list", "playlist", "playlists", "info", 0};
void (*cmd_f[])(int, char *[]) = {cmd_help, cmd_play, cmd_play_pause, cmd_play_pause, cmd_stop, cmd_next, cmd_prev, cmd_seek, cmd_signal, cmd_volume, cmd_quit, cmd_list, cmd_playlist, cmd_playlists, cmd_info, NULL};

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
	return "";
}

void cmd_play (int argc, char * argv[]) {
    if (argc <= 0) {
        printf ("%s: starts playback\n", cmd_name(argv[0]));
        printf ("\t1: track num (optional)\n");
        return;
    }
    if (argc == 1) {
        deadbeef->sendmessage (DB_EV_PLAY_CURRENT, 0, 0, 0);
    }
    else if (argc == 2) {
    	// select track
    	ddb_playlist_t *plt = deadbeef->plt_get_curr();
    	deadbeef->plt_deselect_all (plt);
    	DB_playItem_t *item = deadbeef->pl_get_for_idx (atoi(argv[1]));
    	deadbeef->pl_set_selected (item, 1);
    	deadbeef->pl_item_unref (item);
    	deadbeef->plt_unref (plt);

        deadbeef->sendmessage (DB_EV_PLAY_NUM, 0, atoi(argv[1]), 0);
    }
}

/* generate play list, also numbers. Doesn't work, need to find good method for such generation
char *
cmd_play_generator(const char *text, int state) {
	printf ("\ntext \"%s\" state %d\n",text,state);
    static int list_index, len;
    char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }
	ddb_playlist_t *plt = deadbeef->plt_get_curr();
    int sum = deadbeef->plt_get_item_count (plt, PL_MAIN);
    deadbeef->plt_unref (plt);
    if (strlen (text) == 2 && state == 0)
    	return strdup(text);
    if (strlen (text) == 2 && state == 1)
    	return NULL;
    if (state == sum)
    	return NULL;
    if (text[0] == '0') {
    	if (state >= 10)
    		return NULL;
    }
    if (text[0])
    	state += (text[0]-48)*10;
    if (text[0] && state > 10)
    	return NULL;

    char num[16];
    sprintf (num, "%02d", state);
    return strdup(num);
}
*/

void cmd_play_pause (int argc, char * argv[]) {
    if (argc != 1) {
    	if (cmd_find(argv[0]) == cmd_num("resume"))
        	printf ("%s: unpauses playback\n", cmd_name(argv[0]));
    	else
        	printf ("%s: pauses/unpauses playback\n", cmd_name(argv[0]));
        return;
    }
    if (cmd_find(argv[0]) == cmd_num("resume")) {
    	struct DB_output_s* output = deadbeef->get_output ();
    	if (output->state() != OUTPUT_STATE_PAUSED)
    		return;
    }
    deadbeef->sendmessage (DB_EV_TOGGLE_PAUSE, 0, 0, 0);
}

void cmd_stop (int argc, char * argv[]) {
    if (argc != 1) {
        printf ("%s: stops playback\n", cmd_name(argv[0]));
        return;
    }
    deadbeef->sendmessage (DB_EV_STOP, 0, 0, 0);
}

void cmd_next (int argc, char * argv[]) {
	// TODO: take argument to go for example 2 tracks after
    if (argc != 1) {
        printf ("%s: play next track\n", cmd_name(argv[0]));
        return;
    }
    deadbeef->sendmessage (DB_EV_NEXT, 0, 0, 0);
}

void cmd_prev (int argc, char * argv[]) {
	// TODO: take argument to go for example 2 tracks before
    if (argc != 1) {
        printf ("%s: play previous track\n", cmd_name(argv[0]));
        return;
    }
    deadbeef->sendmessage (DB_EV_PREV, 0, 0, 0);
}

void cmd_seek (int argc, char * argv[]) {
    if (argc != 2) {
        printf ("%s: seek track\n", cmd_name(argv[0]));
        printf ("\t1: seconds to seek from current position\n");
        return;
    }
    DB_playItem_t* curr_track = deadbeef->streamer_get_playing_track ();
    if (!curr_track) {
    	return;
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
}

char *events[] = { "", "DB_EV_NEXT", "DB_EV_PREV", "DB_EV_PLAY_CURRENT", "DB_EV_PLAY_NUM", "DB_EV_STOP", "DB_EV_PAUSE", "DB_EV_PLAY_RANDOM", "DB_EV_TERMINATE", "DB_EV_PLAYLIST_REFRESH", "DB_EV_REINIT_SOUND", "DB_EV_CONFIGCHANGED", "DB_EV_TOGGLE_PAUSE", "DB_EV_ACTIVATED", "DB_EV_PAUSED", "DB_EV_PLAYLISTCHANGED", "DB_EV_VOLUMECHANGED", "DB_EV_OUTPUTCHANGED", "DB_EV_PLAYLISTSWITCHED", "DB_EV_SEEK", "DB_EV_ACTIONSCHANGED", "DB_EV_DSPCHAINCHANGED", "DB_EV_SELCHANGED", "DB_EV_PLUGINSLOADED", "DB_EV_FOCUS_SELECTION", 0};

void cmd_signal (int argc, char * argv[]) {
    if (argc <= 1) {
        printf ("%s: sends message event, arguments:\n", cmd_name(argv[0]));
        printf ("\t1: event: either number or name\n");
        printf ("\t2: p1: number\n");
        printf ("EXAMPLE: signal DB_EV_PLAY_CURRENT\n");
        return;
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
                    return;
                }
                else {
                    deadbeef->sendmessage (i, 0, 0, 0);
                    return;
                }
            }
        }
        printf ("unknown signal %s\n",argv[1]);
    }
}

char *
cmd_signal_generator(const char *text, int state) {
    static int list_index, len;
    char *name;

    if (!state) {
        list_index = 0;
        len = strlen(text);
    }

    while ((name = events[list_index++])) {
        if (strncmp(name, text, len) == 0) {
            return strdup(name);
        }
    }
    return NULL;
}

void cmd_volume (int argc, char * argv[]) {
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
    		return;
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
    //
}

void cmd_help (int argc, char * argv[]) {
    if (argc < 1) {
        printf ("%s: generates help page :)\n", cmd_name(argv[0]));
        printf ("\t 1: (optional) command name\n");
        return;
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
        if (i == CMD_NOTFOUND)
        	printf ("%s: command %s not found\n", cmd_name(argv[0]), argv[1]);
        else if (i == CMD_AMBIGOUS)
        	return;
        else {
        	char * empty[] = {argv[1], NULL};
        	cmd_f[i] (-1, empty);
        }
    }
    // func end
}

void cmd_quit (int argc, char * argv[]) {
    if (argc < 0) {
        printf ("%s: exits player\n", cmd_name(argv[0]));
        return;
    }
    // note for (long) future: check for background jobs
	deadbeef->sendmessage (DB_EV_TERMINATE, 0, 0, 0);
	ui_running = 0;
}

void cmd_list (int argc, char * argv[]) {
    if (argc < 0) {
        printf ("%s: lists playlist tracks (default: current playlist)\n", cmd_name(argv[0]));
        printf ("\t1: (optional) playlist number\n");
        return;
    }
    if (argc == 1 || argc == 2) {
    	ddb_playlist_t *plt;
    	if (argc == 1)
    		plt = deadbeef->plt_get_curr();
    	else
    		plt = deadbeef->plt_get_for_idx(atoi(argv[1]));

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
	    char * script = "%tracknumber%. %artist% - %title% /// %album%\n";
	    char * code_script = deadbeef->tf_compile (script);
    	DB_playItem_t *item = deadbeef->plt_get_first (plt, PL_MAIN);
    	if (!item)
    		return;
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
}

void cmd_playlist (int argc, char * argv[]) {
    if (argc < 1) {
        printf ("%s: sets current playlist\n", cmd_name(argv[0]));
        printf ("\t1: playlist number (or offset)\n");
        return;
    }
    else if (argc == 1) {
    	DB_playItem_t* curr_track = deadbeef->streamer_get_playing_track ();
    	if (curr_track) {
    		ddb_playlist_t* playlist = deadbeef->pl_get_playlist (curr_track);
    		deadbeef->plt_set_curr (playlist);
    		deadbeef->plt_unref (playlist);
    		deadbeef->pl_item_unref (curr_track);
    		return;
    	}
    	else {
			ddb_playlist_t* playlist = deadbeef->plt_get_curr ();
			char buffer[256];
			deadbeef->plt_get_title (playlist, buffer, 256);
			deadbeef->plt_unref (playlist);
			printf ("Current playlist: %s\n",buffer);
		}
    	return;
    }
    else if (argc == 2) {
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
}

void cmd_playlists (int argc, char * argv[]) {
    if (argc <= 0) {
        printf ("%s: lists all playlists\n", cmd_name(argv[0]));
        return;
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
    			//i++;
    			playlist = new;
    		}
    		else {
    			break;
    		}
    	}
    	while (1);
    }
}

void cmd_info (int argc, char * argv[]) {
    if (argc < 0) {
        printf ("%s: shows current position, including playlist\n", cmd_name(argv[0]));
        return;
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
	    	printf ("%s\n",buffer);

	    	float item_length = deadbeef->pl_get_item_duration (curr_track);
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
    		printf ("Failed to generate list\n");
    		return;
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
}

char *
cmd_generator(const char *text, int state) {
    static int list_index, len;
    char *name;

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

char **
cmd_completion(const char *text, int start, int end) {
    rl_attempted_completion_over = 1;
    // TODO do not depend on (int) start, make tab completion on (example) "sig D*" working
    if (start == 0)
        return rl_completion_matches(text, cmd_generator);
    else if (strncmp(rl_line_buffer, "help ", 5) == 0 && start == 5)
        return rl_completion_matches(text, cmd_generator);
    else if (strncmp(rl_line_buffer, "signal ", 7) == 0 && start == 7)
        return rl_completion_matches(text, cmd_signal_generator);
    //else if (strncmp(rl_line_buffer, "play ", 5) == 0 && start == 5)
    //    return rl_completion_matches(text, cmd_play_generator);
    else
        return NULL;
    // saved for later (file addition)
    //return rl_completion_matches(text, rl_filename_completion_function);
        
}

int cmd_find (char * cmd) {
	int i;
	int exact_cmd = -1;
	int possible_cmds[10];
	int poss_p = 0;
	memset (possible_cmds, -1, sizeof (int) * 10);

	// exceptions
	char * links_s[] = {"p", "pl", "pls", "pp", "ls", NULL};
	char * links_n[] = {"play", "playlist", "playlists", "pause", "list", NULL};

	// find exact name or possible names;
    for (i = 0; cmd_s[i] != 0; i++) {
        if (strcmp(cmd, cmd_s[i]) == 0) {
        	exact_cmd = i;
        	break;
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
        if (exact_cmd != -1) {
        	break;
        }
    }
    if (exact_cmd != -1) {
    	return exact_cmd;
    }
    else if (possible_cmds[0] == -1) {
    	return CMD_NOTFOUND;
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
    	return CMD_AMBIGOUS;
    }
}

#define ARGV_MAX 9

int cmd (char * buffer_orig) {
    char buffer[strlen(buffer_orig)+1];
    strcpy (buffer, buffer_orig);

    // build argv list
    char * pch;
    char * argv[ARGV_MAX];
    int vi = 0;
    pch = strtok (buffer," ");
    while (pch != NULL) {
        argv[vi] = malloc (strlen(pch)+1);
        if (argv[vi])
            strcpy (argv[vi],pch);
        vi++;
        if (vi >= ARGV_MAX) {
            printf ("argv max!!!\n");
            break;
        }
        pch = strtok (NULL, " ");
    }
    argv[vi] = NULL;

    if (argv[0] == NULL) {
    	return -1;
    }

    int find_ret = cmd_find (argv[0]);

    if (find_ret == CMD_NOTFOUND) {
    	return -1;
    }
    else if (find_ret == CMD_AMBIGOUS) {
    	return -2;
    }
    else {
    	// find_ret is command num
    	cmd_f[find_ret] (vi, argv);
    	return 0;
    }
}
