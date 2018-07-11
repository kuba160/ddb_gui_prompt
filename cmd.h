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

int cmd_num (char *cmd);

const char *cmd_name (char *cmd);

void cmd_play (int argc, char * argv[]);

void cmd_play_pause (int argc, char * argv[]);

/*char * cmd_play_generator(const char *text, int state);*/

void cmd_stop (int argc, char * argv[]);

void cmd_next (int argc, char * argv[]);

void cmd_prev (int argc, char * argv[]);

void cmd_seek (int argc, char * argv[]);

void cmd_signal (int argc, char * argv[]);

void cmd_volume (int argc, char * argv[]);

void cmd_help (int argc, char * argv[]);

void cmd_quit (int argc, char * argv[]);

void cmd_list (int argc, char * argv[]);

void cmd_playlist (int argc, char * argv[]);

void cmd_playlists (int argc, char * argv[]);

void cmd_info (int argc, char * argv[]);

char * cmd_generator(const char *text, int state);

char ** cmd_completion(const char *text, int start, int end);

#define CMD_NOTFOUND -1
#define CMD_AMBIGOUS -2

int cmd_find (char * cmd);

int cmd (char *);
