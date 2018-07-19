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
char * cmd_get_path ();

void cmd_change_path (int argc, char * argv[]);

int cmd_num (char *cmd);

const char *cmd_name (char *cmd);

const char *dirm_name (char *cmd);

char * cmd_play (int argc, char * argv[], int iter);

char * cmd_play_pause (int argc, char * argv[], int iter);

/*char * cmd_play_generator(const char *text, int state);*/

char * cmd_stop (int argc, char * argv[], int iter);

char * cmd_next (int argc, char * argv[], int iter);

char * cmd_prev (int argc, char * argv[], int iter);

char * cmd_seek (int argc, char * argv[], int iter);

char * cmd_signal (int argc, char * argv[], int iter);

char * cmd_volume (int argc, char * argv[], int iter);

char * cmd_help (int argc, char * argv[], int iter);

char * cmd_quit (int argc, char * argv[], int iter);

char * cmd_list (int argc, char * argv[], int iter);

char * cmd_playlist (int argc, char * argv[], int iter);

char * cmd_playlists (int argc, char * argv[], int iter);

char * cmd_info (int argc, char * argv[], int iter);

char * cmd_dirm (int argc, char * argv[], int iter);

char * cmd_generator (const char *text, int state);

char * cmd_completion_iter (const char *text, int state);

char ** cmd_completion (const char *text, int start, int end);

#define FIND_NOTFOUND -1
#define FIND_AMBIGUOUS -2
#define FIND_DIRM -3

int cmd_find (char * cmd);

int cmd_ambiguous_print (char * cmd);

int cmd (char *);
