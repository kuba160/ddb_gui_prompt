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
// Useful definitions/functions for command handling / tab-completion

// Return values for commands, CMD_EXECUTED equals NULL, can change in future
#define CMD_DIRECTORY "Directory"
#define CMD_EXECUTED  0//"Executed"
#define CMD_NOTFOUND  "NotFound"

// TAB COMPLETION
// these macros require standard names (argc/argv/iter) and have to be called from command

// end tab-completion processing
#define NO_TAB_COMPLETION \
	if (iter != -1) {return NULL;};

// tab-complete argument number X with string table Y (char **, table with entries ended with NULL, escaping will be done internally) 
#define TAB_COMPLETION_TABLE(X,Y) \
	if (argc == (1+X) && iter != -1) {return cmd_tab_complete_table(Y, argv, iter);}

// tab-complete argument number X with playlists
#define TAB_COMPLETION_PLAYLIST(X) \
	if (argc == (1+X) && iter != -1) {return cmd_tab_complete_playlists(argv, iter);}

// tab-complete argument number X with artists in current playlist
#define TAB_COMPLETION_ARTIST_CURR(X) \
	if (argc == (1+X) && iter != -1) {return cmd_tab_complete_meta("artist", argv, iter);}

// tab-complete argument number X with titles in current playlist
#define TAB_COMPLETION_TITLE_CURR(X) \
	if (argc == (1+X) && iter != -1) {return cmd_tab_complete_meta("title", argv, iter);}

// tab-complete argument number X with albums in current playlist
#define TAB_COMPLETION_ALBUM_CURR(X) \
	if (argc == (1+X) && iter != -1) {return cmd_tab_complete_meta("album", argv, iter);}

// tab-complete arguments X Y Z with artist/title/playlist, complementary, automatic argument detection, each can be 0
#define TAB_COMPLETION_ATA(X, Y, Z) \
	if (iter != -1) { \
		char *artist = 0, *title = 0, *album = 0; \
		if (argc-1 >=X && X > 0) { artist = argv[X];}; \
		if (argc-1 >=Y && Y > 0) { title = argv[Y];}; \
		if (argc-1 >=Z && Z > 0) { album = argv[Z];}; \
		return cmd_tab_complete_ata(artist, title, album, argv, iter); \
	};

// end tab-completion processing
#define TAB_COMPLETION_END NO_TAB_COMPLETION

// tab-completion functions, try to use macros instead

char * cmd_tab_complete_table (const char **table, char **argv, int iter);

char * cmd_tab_complete_playlists (char **argv, int iter);

char * cmd_tab_complete_ata (char * artist, char * title, char * album, char **argv, int iter);

char * cmd_tab_complete_meta (char * meta, char **argv, int iter);

// get items after data

// get item from playlist which matches artist/title/album, set each to NULL if not available, strings will be unescaped
DB_playItem_t* cmd_get_item (ddb_playlist_t *playlist, char * artist, char * title, char * album);

// get playlist which matches name
ddb_playlist_t* cmd_get_playlist (char * string);

// string functions/tools
int has_spaces (const char * string);

char * strdup_quoted (const char * string);

char * strdup_escaped (const char * string);

char * strdup_unescaped (const char * string);

// ARGV

int argv_count (char ** argv);

char ** argv_alloc (char * cmd);

void argv_free (char ** argv);

int argv_cat (char ** to, char ** from);
