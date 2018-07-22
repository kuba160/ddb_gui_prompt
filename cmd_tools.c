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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <deadbeef/deadbeef.h>
#include "cmd_tools.h"

extern DB_functions_t *deadbeef;

// default argv size, will be reallocated if needed
#define ARGV_MAX 16
// default property size, will be reallocated if needed
#define PROPERTIES_MAX 8

char ** temp_table = 0;

// cmd_tab_complete functions try to complete last argv argument
// it is best to call these functions from macros

// tab-complete after table
char * cmd_tab_complete_table (const char **table, char **argv, int iter) {
	int argc;
	for (argc = 0; argv[argc] != NULL; argc++);
	int quotesc = 0;
	{
		int i;
		for (i = 0; argv[argc-1][i]; i++) {
			if (argv[argc-1][i] == '\"')
				quotesc++;
		}
	}
	if (quotesc >= 2) {
		if (iter == 0) {
			return strdup ("");
		}
		else
			return NULL;
	}
	char *to_extend = strdup_unescaped(argv[argc-1]);// malloc (16 + strlen(argv[i-1]));

	int to_extend_spaces = 0;
	int i;
	for (i = 0; to_extend[i]; i++) {
		if (to_extend[i] == ' ')
			to_extend_spaces++;
	}
	// Try to detect if there are names which include space, if that's the case put every option in quotes
	// sometimes it's useful, sometimes not
	int enquoteme = 0;
	{
		if (strlen(to_extend) > 0) {
			for (i = 0; table[i] != NULL; i++) {
				if (strncmp (table[i], to_extend, strlen(to_extend)) == 0 && has_spaces(table[i])) {
					enquoteme = 1;
					break;
				}
			}
		}
	}
	int j = 0;	
	for (i = 0; table[i] != NULL; i++) {
		if (strncmp (table[i], to_extend, strlen(to_extend)) == 0) {
			if (iter == j) {
				free (to_extend);
				char *ret = 0;
				if (enquoteme)
					ret = strdup_quoted (table[i]);
				else
					ret = strdup_escaped (table[i]);
				return ret;
			}
			else {
				j++;
			}
		}
	}
	free (to_extend);
	return NULL;
}

// tab-complete playlists
char * cmd_tab_complete_playlists (char **argv, int iter) {
	if (iter == 0) {
		int count = deadbeef->plt_get_count ();
		temp_table = malloc ((count+1) * sizeof(char *));
		int i;
		ddb_playlist_t *plt;
		char buffer[256];
		for (i = 0; 1; i++) {
			plt = deadbeef->plt_get_for_idx (i);
			if (!plt) {
				break;
			}
			deadbeef->plt_get_title (plt, buffer, 256);
			deadbeef->plt_unref (plt);
			temp_table[i] = strdup (buffer);
		}
		temp_table[i] = NULL;
	}
	// artist table generated;
	char *ret = cmd_tab_complete_table ((const char **)temp_table, argv, iter);
	if (!ret) {
		int i;
		for (i = 0; temp_table[i]; i++) {
			free (temp_table[i]);
		}
		free (temp_table);
		temp_table = 0;
	}
	return ret;
}

// tab completion after ATA (artist/title/album)
char * cmd_tab_complete_ata (char * artist_o, char * title_o, char * album_o, char ** argv, int iter) {
	if (iter == 0) {
		ddb_playlist_t *plt = deadbeef->plt_get_curr();
		if (!plt) {
			return NULL;
		}
		int it_count = deadbeef->plt_get_item_count (plt, PL_MAIN);
		if (!it_count) {
			return NULL;
		}
		// find what we are looking for
		int argc;
		for (argc = 0; argv[argc]; argc++);
		char *looking_for = 0;
		// one of X_o should have same adress as argv[argc-1]
		if (artist_o == argv[argc-1]) {
			looking_for = "artist";
		}
		else if (title_o == argv[argc-1]) {
			looking_for = "title";
		}
		else if (album_o == argv[argc-1]) {
			looking_for = "album";
		}
		else {
			return NULL;
		}
		temp_table = malloc ((it_count+1) * sizeof(char *));
		int i;
		int j = 0;
		char * artist = 0; 
		char * title = 0; 
		char * album = 0; 
		if (artist_o && strcmp (artist_o, "*"))
			artist = strdup_unescaped (artist_o);
		if (title_o && strcmp (title_o, "*"))
			title = strdup_unescaped (title_o);
		if (album_o && strcmp (album_o, "*"))
			album = strdup_unescaped (album_o);
		// search if it already exists
		int required = 0;
		artist ? required++ : 0;
		title ? required++ : 0;
		album ? required++ : 0;
		required--;

		DB_metaInfo_t* meta;
		DB_playItem_t *item;
		for (i = 0; 1; i++) {
			item = deadbeef->pl_get_for_idx (i);
			if (!item) {
				break;
			}
			int matches = 0;
			// 
			if (strcmp (looking_for, "artist")) {
				meta = deadbeef->pl_meta_for_key (item, "artist");
				if (artist && meta) {
					if (strcmp (artist,meta->value) == 0) {
						matches++;
					}
				}

			}
			if (strcmp (looking_for, "title")) {
				meta = deadbeef->pl_meta_for_key (item, "title");
				if (title && meta) {
					if (strcmp (title,meta->value) == 0) {
						matches++;
					}
				}

			}
			if (strcmp (looking_for, "album")) {
				meta = deadbeef->pl_meta_for_key (item, "album");
				if (album && meta) {
					if (strcmp (album,meta->value) == 0) {
						matches++;
					}
				}

			}
			meta = deadbeef->pl_meta_for_key (item, looking_for);
			if (matches >= required && meta) {
				// search if it already exists
				int found = 0;
				{
					int h;
					for (h = 0; h < j; h++) {
						if (strcmp (temp_table[h], meta->value) == 0) {
							found = 1;
							break;
						}
					}
				}
				if (!found) {
					temp_table[j] = strdup (meta->value);
					j++;
				}
			}
			deadbeef->pl_item_unref (item);
		}
		temp_table[j] = NULL;
		deadbeef->plt_unref (plt);
		// realloc back
		if (j+1 < it_count)
			temp_table = realloc (temp_table, (j+1) * sizeof (char *));
	}
	// artist table generated;
	char *ret = cmd_tab_complete_table ((const char **)temp_table, argv, iter);
	if (!ret) {
		int i;
		for (i = 0; temp_table[i]; i++) {
			free (temp_table[i]);
		}
		free (temp_table);
		temp_table = 0;
	}
	return ret;
}

// tab completion after metadata
char * cmd_tab_complete_meta (char * meta_s, char **argv, int iter) {
	if (iter == 0) {
		ddb_playlist_t *plt = deadbeef->plt_get_curr();
		if (!plt) {
			return NULL;
		}
		int it_count = deadbeef->plt_get_item_count (plt, PL_MAIN);
		if (!it_count) {
			return NULL;
		}
		temp_table = malloc ((it_count+1) * sizeof(char *));
		int i;
		int j = 0;
		DB_metaInfo_t* meta;
		DB_playItem_t *item;
		for (i = 0; 1; i++) {
			item = deadbeef->pl_get_for_idx (i);
			if (!item) {
				break;
			}
			meta = deadbeef->pl_meta_for_key (item, meta_s);
			if (!meta) {
				deadbeef->pl_item_unref (item);
				continue;
			}
			// search if it already exists
			int found = 0;
			{
				int h;
				for (h = 0; h < j; h++) {
					if (strcmp (temp_table[h], meta->value) == 0) {
						found = 1;
						break;
					}
				}
			}
			if (!found) {
				temp_table[j] = strdup (meta->value);
				j++;
			}
			deadbeef->pl_item_unref (item);
		}
		temp_table[j] = NULL;
		deadbeef->plt_unref (plt);
		// realloc back
		if (i < it_count)
			temp_table = realloc (temp_table, (i+1) * sizeof (char *));
	}
	// artist table generated;
	char *ret = cmd_tab_complete_table ((const char **)temp_table, argv, iter);
	if (!ret) {
		int i;
		for (i = 0; temp_table[i]; i++) {
			free (temp_table[i]);
		}
		free (temp_table);
		temp_table = 0;
	}
	return ret;
}

// tab-complete after properties
char * cmd_tab_complete_properties (struct property **table, char **argv, int iter) {
	int i;
	const char * strings[32];
	for (i = 0; table[i]; i++) {
		if (i >= 32) {
			break;
		}
		strings[i] = table[i]->key;
	}
	strings[i] = NULL;
	return cmd_tab_complete_table (strings, argv, iter);
}

// get item based on data (artist/title/album and playlist)
DB_playItem_t* cmd_get_item (ddb_playlist_t *playlist, char * artist_o, char * title_o, char * album_o) {
	ddb_playlist_t *plt;
	if (!playlist) {
		plt = deadbeef->plt_get_curr();
	}
	else {
		plt = playlist;
	}
	if (!plt) {
		return NULL;
	}
	int it_count = deadbeef->plt_get_item_count (plt, PL_MAIN);
	if (!it_count) {
		return NULL;
	}
	char * artist = 0; 
	char * title = 0; 
	char * album = 0; 
	if (artist_o && strcmp (artist_o, "*"))
		artist = strdup_unescaped (artist_o); 
	if (title_o && strcmp (title_o, "*"))
		title = strdup_unescaped (title_o); 
	if (album_o && strcmp (album_o, "*"))
		album = strdup_unescaped (album_o); 
	int i;
	DB_metaInfo_t* meta;
	DB_playItem_t *item;
	for (i = 0; 1; i++) {
		item = deadbeef->plt_get_item_for_idx (plt, i, PL_MAIN);
		if (!item) {
			break;
		}
		// search if it already exists
		int matches = 0;
		int required = 0;
		artist ? required++ : 0;
		title ? required++ : 0;
		album ? required++ : 0;
		if (!required) {
			break;
		}
		{
			meta = deadbeef->pl_meta_for_key (item, "artist");
			if (artist && meta) {
				if (strcmp(artist,meta->value) == 0) {
					matches++;
				}
			}
			meta = deadbeef->pl_meta_for_key (item, "title");
			if (title && meta) {
				if (strcmp(title,meta->value) == 0) {
					matches++;
				}
			}
			meta = deadbeef->pl_meta_for_key (item, "album");
			if (album && meta) {
				if (strcmp(album,meta->value) == 0) {
					matches++;
				}
			}
		}
		if (matches >= required) {
			if (!playlist) {
				deadbeef->plt_unref (plt);
			}
			free (artist);
			free (album);
			free (title);
			return item;
		}
		else {
			deadbeef->pl_item_unref (item);
		}
	}
	free (artist);
	free (album);
	free (title);
	return NULL;
}

// get playlist from escaped string after name
ddb_playlist_t* cmd_get_playlist (char * name) {
    char * string = strdup_unescaped (name);
    int i = 0;
    ddb_playlist_t * plt;
    char buffer[256];
    for (i = 0;(plt = deadbeef->plt_get_for_idx (i)); i++) {
    	if (!plt) {
    		break;
    	}
        deadbeef->plt_get_title (plt, buffer, 256);
        
        if (strcmp(string, buffer) == 0) {
        	free (string);
     		return plt;
   		}
        deadbeef->plt_unref (plt);
    }
    free (string);
    return NULL;
}

// detect spaces in string
int has_spaces (const char * string) {
	int contains_spaces = 0;
	int i;
	for (i = 0; string[i]; i++) {
		if (string[i] == ' ') {
			contains_spaces = 1;
			break;
		}
	}
	return contains_spaces;
}

// enquote string (put in "") and duplicate
char * strdup_quoted (const char * string) {
	int contains_spaces = 1;
	int i;
	for (i = 0; string[i]; i++) {
		if (string[i] == ' ') {
			contains_spaces = 1;
			break;
		}
	}
	char * ptr;
	if (contains_spaces) {
		ptr = malloc (strlen(string) + 1 + 2);
		ptr[0] = '\"'; ptr[1] = 0;
		strcat (ptr, string);
		strcat (ptr, "\"");
	}
	else {
		ptr = strdup (string);
	}
	return ptr;
}

// escape string (put in "") if needed and duplicate
char * strdup_escaped (const char * string) {
	int contains_spaces = 0;
	int i;
	for (i = 0; string[i]; i++) {
		if (string[i] == ' ') {
			contains_spaces = 1;
			break;
		}
	}
	char * ptr;
	if (contains_spaces) {
		ptr = malloc (strlen(string) + 1 + 2 + 6);
		ptr[0] = '\"'; ptr[1] = 0;
		strcat (ptr, string);
		strcat (ptr, "\"");
	}
	else {
		ptr = strdup (string);
	}
	return ptr;
}

// remove "" from string and duplicate
char * strdup_unescaped (const char * string) {
	int contains_app = 0;
	int i;
	for (i = 0; string[i]; i++) {
		if (string[i] == '\"') {
			contains_app = 1;
			break;
		}
	}
	char * ptr;
	if (contains_app) {
		ptr = malloc (strlen(string));
		int i;
		int j = 0;
		int ap_count = 0;
		for (i = 0; string[i]; i++) {
			if (string[i] == '\"' && (i == 0 || string[i+1] == 0)) {
				ap_count++;
				continue;
			}
			else {
				ptr[j++] = string[i];
			}
		}
		ptr[j] = 0;
	}
	else {
		ptr = strdup (string);
	}
	return ptr;
}

// ARGV

int argv_count (char **argv) {
	int i;
	for (i = 0; argv[i] != NULL; i++);
	return i;
}

char ** argv_alloc (char * cmd) {
    // build argv list
    char * pch;
    int argv_size = ARGV_MAX;
    char ** argv = malloc (argv_size * sizeof(char *));

    if (!cmd) {
        argv[0] = NULL;
        return argv;
    }
    char buffer[strlen(cmd)+1];
    strcpy (buffer, cmd);

    int vi = 0;
    // quotes: should work fine
    #define SPECIAL_CHAR 27
    {
        int q;
        int spec_mode_on = 0;
        for (q = 0; buffer[q] != 0; q++) {
            if (buffer[q] == '\"') {
                spec_mode_on = !spec_mode_on;
                continue;
            }
            if (spec_mode_on && buffer[q] == ' ') {
                buffer[q] = SPECIAL_CHAR;
            }
        }
        if (spec_mode_on) {
            // need to support incomplete names for tab completion
            //free (argv);
            //return NULL;
        }

    }
    pch = strtok (buffer," ");
    while (pch != NULL) {
        argv[vi] = strdup (pch);
        vi++;
        if (vi >= argv_size) {
            argv_size *= 2;
            argv = realloc (argv, argv_size * sizeof(char *));
            if (!argv) {
                printf ("Failed to realloc memory!\n");
                return NULL;
            }
        }
        pch = strtok (NULL, " ");
    }
    argv[vi] = NULL;

    {
        int i;
        for (i = 0; argv[i] != NULL; i++) {
            // for every char
            int j;
            for (j = 0; argv[i][j] != 0; j++) {
                if (argv[i][j] == SPECIAL_CHAR)
                    argv[i][j] = ' ';
            }
        }
    }
    return argv;
}

void argv_free (char ** argv) {
	int i;
	for (i = 0; argv[i] != NULL; i++)
		free (argv[i]);
	free (argv);
    return;
}

int argv_cat (char ** to, char ** from) {
	int i;
	for (i = 0; to[i] != NULL; i++);
	int j;
	for (j = 0; from[j] != NULL; j++) {
		to[i] = strdup (from[j]);
		i++;
	}
	to[i] = NULL;
	return 0;
}

// Properties

int properties_count (property_t **properties) {
	int i;
	for (i = 0; properties[i] != NULL; i++);
	return i;
}

property_t * property_get (property_t **properties, const char * key) {
	int i;
	for (i = 0; properties[i] != NULL; i++) {
		if (strcmp(properties[i]->key, key) == 0) {
			return properties[i];
		}
	}
	return NULL;
}

int property_set (property_t *property, const char * value) {
	// todo error checking
	printf ("property_set called but has no error checking!\n");
    // todo evaluate prop_curr->type_string
    // char *config_argv[] = {"config_i", argv[3], "string", argv[4], NULL };
    //settings_config (4, config_argv, -1);
	deadbeef->conf_set_str (property->key, value);
	return 0;
}

property_t ** properties_alloc (const char * cmd) {
    // build properties list
    int properties_size = PROPERTIES_MAX;
    property_t ** properties = malloc (properties_size * sizeof(struct property *));

    if (!cmd) {
    	// return empty list
        properties[0] = NULL;
        return properties;
    }
    char buffer[strlen(cmd)+1];
    strcpy (buffer, cmd);

    char * ptr = buffer;
 	char * nl_ptr = ptr;
    char * endptr = strrchr (buffer, 0);
    int i;
    int to_break = 0;
    for (i = 0; ptr < endptr; i++) {
        if (i >= properties_size) {
            properties_size *= 2;
            properties = realloc (properties, properties_size * sizeof(char *));
            if (!properties) {
                printf ("Failed to realloc memory!\n");
                return NULL;
            }
        }
    	//
    	nl_ptr = strchr (ptr, '\n');
    	if (!nl_ptr) {
    		// probably last char
    		to_break = 1;
    	}
    	else {
    		*nl_ptr = 0;
    	}
    	properties[i] = property_alloc (ptr);
        if (!properties[i] || to_break) {
            // out of options
            break;
        }
        else {
        	// next
        	ptr = ++nl_ptr;
        }
    }
    properties[i] = NULL;
    return properties;
}

struct property * property_alloc (char * string) {
    if (!string || strlen(string) == 0) {
        return NULL;
    }
    struct property * temp = malloc (sizeof(struct property));
    memset (temp, 0, sizeof(struct property));
    if (!temp) {
        return NULL;
    }
    // todo escape chars
    char ** argv = argv_alloc (string);

    if (!argv[0]) {
        argv_free (argv);
        free (temp);
        return NULL;
    }

    // PROPERTY name type key def
    if (strcmp (argv[0], "property") != 0) {
        printf ("invalid header\n");
        argv_free (argv);
        free (temp);
        return NULL;
    }
    temp->name = strdup_unescaped (argv[1]);
    // type
    if (strcmp (argv[2], "entry") == 0) {
        temp->type = TYPE_ENTRY;
    }
    else if (strcmp (argv[2], "password") == 0) {
        temp->type = TYPE_PASSWORD;
    }
    else if (strcmp (argv[2], "file") == 0) {
        temp->type = TYPE_FILE;
    }
    else if (strcmp (argv[2], "checkbox") == 0) {
        temp->type = TYPE_CHECKBOX;
    }
    else if (strncmp (argv[2], "hscale", 6) == 0) {
        temp->type = TYPE_HSCALE;
        // todo options [min, max, step]

    }
    else if (strncmp (argv[2], "spinbtn", 7) == 0) {
        temp->type = TYPE_SPINBTN;
        // todo options [min, max, step]
    }
    else if (strncmp (argv[2], "vscale", 6) == 0) {
        temp->type = TYPE_VSCALE;
        // todo options [min, max, step]
    }
    else if (strncmp (argv[2], "select", 6) == 0) {
        temp->type = TYPE_SELECT;
        // todo options [count]
    }
    // todo switch to const
    temp->type_string = strdup (argv[2]);

    temp->key = strdup(argv[3]);

    // get curr value
    {
        DB_conf_item_t *item = NULL;
        item = deadbeef->conf_find (temp->key, item);
        if (item) {
            temp->val = strdup(item->value);
        }
    }

    // strip last chars ;\n
    int finished = 0;
    {
        char * semicolon = strchr (argv[4], ';');
        if (semicolon) {
            *semicolon = 0;
            finished = 1;
        }
    }
    temp->def = strdup (argv[4]);

    if (finished) {
        argv_free (argv);
        return temp;
    }

    // TODO handle complicated types
    /*
    ptr = strtok (ptr," ");
    // todo handle \""
    while (ptr != NULL) {
        if (a == 0) {
            if (strncmp (ptr, "property", strlen("property") ) != 0) {
                // not valid option!
                // todo handle
                free (ptr_orig);
                free (temp);
                return NULL;
            }
        }
        if (a == 1) {
            // name
        }
        if (a == 2) {
            // type
        }
        if (a == 3) {
            // key
        }
        if (a == 4) {
            // def
        }
        printf ()
        // = strdup (pch);

        a++;
    }

    */
    return temp;
}

void properties_free (property_t ** argv) {
	int i;
	for (i = 0; argv[i] != NULL; i++)
		free (argv[i]);
	free (argv);
    return;
}

void property_free (struct property * prop) {
    free (prop->name);
    free (prop->type_string);
    free (prop->key);
    free (prop->val);
    free (prop->def);
    free (prop);
    return;
}

int properties_cat (property_t ** to, property_t ** from) {
	int i;
	for (i = 0; to[i] != NULL; i++);
	int j;
	for (j = 0; from[j] != NULL; j++) {
		//
		property_t *temp = malloc (sizeof(property_t));
		memcpy (temp, from[j], sizeof(property_t));
		to[i] = temp;
		i++;
	}
	to[i] = NULL;
	return 0;
}