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

// Type definitions (needed before everything else)

// property of extended gui syntax
typedef struct property {
    // Full name
    char * name;
    // type, one of enum property_type
    int type;
    // type, string format
    const char * type_string;
    // min value of type (if supported by type)
    int type_min;
    // max value of type (if supported by type)
    int type_max;
    // slider step (if supported by type)
    int type_step;
    // count of values (if supported by type)
    int type_count;
    // config option for property
    char * key;
    // current value of property
    char * val;
    // array of possible values (if supported by type)
    char ** val_possible;
    // default value of property
    char * def;
    // v2
    // group (or groups if nested) property occurs
    char * group[5];
    // what property is required to enable this property (can be nested, pointer can have its own 'requires')
    struct property * requires;
} property_t;

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

// tab-complete argument number X with Y properties
#define TAB_COMPLETION_PROPERTIES(X,Y) \
    if (argc == (1+X) && iter != -1) {return cmd_tab_complete_properties(Y, argv, iter);}

// tab-complete argument number X with options from Z properties (if possible) for key Y
#define TAB_COMPLETION_PROPERTIES_OPTION(X,Y,Z) \
    if (argc == (1+X) && argc >= (1+Y) && iter != -1) { \
        property_t *prop = property_get (Z, argv[Y]); \
        if (prop && prop->val_possible) {return cmd_tab_complete_table((const char **)prop->val_possible, argv, iter);} \
        return NULL; \
    }

// end tab-completion processing
#define TAB_COMPLETION_END NO_TAB_COMPLETION

// tab-completion functions, try to use macros instead

char * cmd_tab_complete_table (const char **table, char **argv, int iter);

char * cmd_tab_complete_playlists (char **argv, int iter);

char * cmd_tab_complete_ata (char * artist, char * title, char * album, char **argv, int iter);

char * cmd_tab_complete_meta (char * meta, char **argv, int iter);

char * cmd_tab_complete_properties (struct property ** properties, char **argv, int iter);

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

// Properties

enum property_type {
    TYPE_ENTRY, TYPE_PASSWORD, TYPE_FILE, TYPE_CHECKBOX, TYPE_HSCALE, TYPE_SPINBTN, TYPE_VSCALE, TYPE_SELECT, TYPE_SELECT_S
};

int properties_count (struct property ** properties);

property_t * property_get (property_t **properties, const char * key);

int property_set (property_t *property, const char * value);

int properties_print (property_t **properties);

int property_print (property_t *property);

int is_property_available (property_t *prop);

property_t * property_requires (property_t *prop);

struct property ** properties_alloc (const char * string);

struct property * property_alloc (char * string);

void properties_free (struct property ** properties);

void property_free (struct property * property);

//void properties_free (struct property ** properties);

void property_update (struct property * property);

int properties_cat (struct property ** to, struct property ** from);
