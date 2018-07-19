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

#include <deadbeef/deadbeef.h>
#include "common.h"
#include "settings.h"
#include "cmd_tools.h"

#include "cmd.h"

extern DB_functions_t *deadbeef;

const char * main_s[] = {"plugins", "config", NULL};
char * (*main_f[])(int, char *[], int) = {settings_plugins, settings_config, NULL};

int main_num (char *cmd) {
    if (!cmd) {
        return -1;
    }
	int i;
	for (i = 0; main_s[i] != NULL; i++){
		if (strcmp(cmd,main_s[i]) == 0)
			return i;
	}
	return -1;
}

int config_exists (char *conf) {
    DB_conf_item_t *item = NULL;
    item = deadbeef->conf_find (conf, item);
    return item ? 1 : 0;
}

char * settings_config (int argc, char * argv[], int iter) {
    // settings config.option [type] [new value]
    if (iter != -1 && argc == 2) {
        int i;
        int j = 0;
        DB_conf_item_t *item = NULL;
        for (i = 0; 1; i++) {
             item = deadbeef->conf_find (argv[1], item);
             if (!item) {
                break;
             }
             if (iter == j) {
                return strdup(item->key);
             }
             j++;
         }
         return NULL;
    }
    const char *types[] = {"string", "int", "int64", "float", "delete", NULL};
    TAB_COMPLETION_TABLE (2,types);
    TAB_COMPLETION_END;
    if (argc == 1) {
        return CMD_DIRECTORY;
    }

    if (argc == 2) {
        // check if item exists, then print
        DB_conf_item_t *item = NULL;
        item = deadbeef->conf_find (argv[1], item);
        if (!item) {
            printf ("Config option %s not found.\n", argv[1]);
            return NULL;
        }
        {
            DB_conf_item_t *item2 = NULL;
            item2 = deadbeef->conf_find (argv[1], item);
            if (item2) {
                printf ("ambigous?!\n");
                //return NULL;
            }
        }
        printf ("%s = %s\n", item->key, item->value);
        return NULL;
    }
    if (argc == 3) {
        int i;
        int correct = 0;
        for (i = 0; types[i] != NULL; i++) {
            if (strcmp (types[i], argv[2]) == 0) {
                correct = 1;
                break;
            }
        }
        if (!correct) {
            printf ("Type %s not valid.\n", argv[2]);
            return NULL;
        }
        if (strcmp(argv[2], "delete") == 0) {
            // check if item exists
            DB_conf_item_t *item = NULL;
            item = deadbeef->conf_find (argv[1], item);
            if (!item) {
                printf ("Config option %s not found.\n", argv[1]);
                return NULL;
            }
            deadbeef->conf_remove_items (argv[1]);
            printf ("%s deleted.\n", argv[1]);
            return NULL;
        }
    }
    if (argc == 4) {
        // check if item exists
        DB_conf_item_t *item = NULL;
        item = deadbeef->conf_find (argv[1], item);
        if (!item) {
            printf ("Config option %s not found, creating.\n", argv[1]);
            //return NULL;
        }
        {
            DB_conf_item_t *item2 = NULL;
            item2 = deadbeef->conf_find (argv[1], item);
            if (item2) {
                printf ("ambigous?!\n");
                //return NULL;
            }
        }
        // validate type
        {
            int i;
            int correct = 0;
            for (i = 0; types[i] != NULL; i++) {
                if (strcmp (types[i], argv[2]) == 0) {
                    correct = 1;
                    break;
                }
            }
            if (!correct) {
                printf ("Type %s not valid.\n", argv[2]);
                return NULL;
            }
        }
        // TODO: (quite important) switch from atoX functions to strtoX and detect errors!
        if (strcmp(argv[2], "string") == 0) {
            deadbeef->conf_set_str (argv[1], argv[3]);
        }
        else if (strcmp(argv[2], "int") == 0) {
            deadbeef->conf_set_int (argv[1], atoi(argv[3]));
        }
        else if (strcmp(argv[2], "int64") == 0) {
            deadbeef->conf_set_int (argv[1], atoll(argv[3]));
        }
        else if (strcmp(argv[2], "float") == 0) {
            deadbeef->conf_set_int (argv[1], atof(argv[3]));
        }
        else if (strcmp(argv[2], "delete") == 0) {
            printf ("Type 'delete' does not take any arguments.\n");
            return NULL;
        }
        printf ("%s = %s\n", argv[1], argv[3]);
        return NULL;
    }

    return NULL;
}

struct property {
    char * name;
    int type;
    char * type_string;
    int type_min;
    int type_max;
    int type_step;
    int type_count;
    char * key;
    char * val;
    char * def;
};

enum property_type {
    TYPE_ENTRY, TYPE_PASSWORD, TYPE_FILE, TYPE_CHECKBOX, TYPE_HSCALE, TYPE_SPINBTN, TYPE_VSCALE, TYPE_SELECT
};

// internal use only
struct property * property_gen (struct DB_plugin_s *plugin, int iter) {
    if (!plugin->configdialog) {
        return NULL;
    }
    struct property * temp = malloc (sizeof(struct property));
    memset (temp, 0, sizeof(struct property));
    if (!temp) {
        return NULL;
    }
    int i;
    // use internal buffer
    char * ptr_orig = malloc (strlen(plugin->configdialog) + 1);
    strcpy (ptr_orig, plugin->configdialog);
    char * ptr = ptr_orig;
    for (i = 0; i < iter; i++) {
        ptr = strchr (ptr, '\n');
        if (!ptr) {
            // out of options
            free (ptr_orig);
            free (temp);
            return NULL;
        }
        ptr++;
        //
    }
    // todo escape chars
    char ** argv = argv_alloc (ptr);

    if (!argv[0]) {
        argv_free (argv);
        free (ptr_orig);
        free (temp);
        return NULL;
    }

    // PROPERTY name type key def
    if (strcmp (argv[0], "property") != 0) {
        printf ("invalid header\n");
        // todo free
    }
    temp->name = strdup(argv[1]);
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

void property_free (struct property * prop) {
    free (prop->name);
    free (prop->type_string);
    free (prop->key);
    free (prop->val);
    free (prop->def);
    free (prop);
    return;
}

// MAX_PLUGINS = 100 (defined in deadbeef/plugins.c)
struct DB_plugin_s * plugins[101] = {NULL};
const char * plugin_names[128];
int plug_table_generated = 0;

int plugins_num (char * name_o) {
    int num = -1;
    int i;
    for (i = 0; plugins[i] != 0; i++) {
        char *name = strdup_unescaped (name_o);
        if (plugins[i]->id) {
            if (strcmp(plugins[i]->id, name) == 0) {
                num = i;
                free (name);
                break;
            }
        }
        else if (plugins[i]->name) {
            if (strcmp(plugins[i]->name, name) == 0) {
                num = i;
                free (name);
                break;
            }
        }
        free (name);
    }
    return num;
}

void plug_table_generate () {
    if (!plug_table_generated) {
        struct DB_plugin_s ** plgs = deadbeef->plug_get_list();
        int i;
        int pos = 0;
        for (i = 0; plgs[i] != 0; i++) {
            if (i >= 100) {
                printf ("plug list max!\n");
                break;
            }
            // require id
            //
            plugins[pos] = plgs[i];
            if (plgs[i]->id) {
                plugin_names[pos] = plgs[i]->id;
            }
            else if (plgs[i]->name) {
                plugin_names[pos] = plgs[i]->name;
            }
            pos++;
            //}
        }
        plugins[i] = NULL;
        plugin_names[i] = NULL;
        plug_table_generated = 1;
    }
}

char * settings_plugins (int argc, char * argv[], int iter) {
    // cache plugin pointers
    if (!plug_table_generated) {
        plug_table_generate ();
    }
    TAB_COMPLETION_TABLE(1,plugin_names);
    int num = -1;
    if (argc < 0) {
        printf ("plugins: settings separated by plugins\n");
        return NULL;
    }
    else if (argc == 1) {
        return CMD_DIRECTORY;
    }
    else if (argc >= 2) {
        num = plugins_num (argv[1]);
        if (num == -1) {
            if (iter == -1)
                printf ("Could not find plugin %s\n", argv[1]);
            return NULL;
        }
        //char *possible_options[] = {"descr", "name", "copyright", "website", "config", "version", "type", NULL};
        const char *options[10];
        int opt = 0;
        if (plugins[num]->descr)
            options[opt++] = "description";
        if (plugins[num]->name)
            options[opt++] = "name";
        if (plugins[num]->copyright)
            options[opt++] = "copyright";
        if (plugins[num]->website)
            options[opt++] = "website";
        if (plugins[num]->configdialog)
            options[opt++] = "config";
        options[opt++] = "version";
        options[opt++] = "type";
        options[opt++] = NULL;
        TAB_COMPLETION_TABLE(2,options);
    }
    if (argc == 2 && iter == -1 && num != -1) {
        return CMD_DIRECTORY;
    }
    if (argc == 3 && iter == -1) {
        if (strcmp (argv[2],"description") == 0) {
            if (plugins[num]->descr) {
                printf ("%s\n", plugins[num]->descr);
            }
            else {
                printf ("(no description)\n");
            }
        }
        else if (strcmp (argv[2],"name") == 0) {
            if (plugins[num]->name) {
                printf ("%s\n", plugins[num]->name);
            }
            else {
                printf ("(no name)\n");
            }
        }
        else if (strcmp (argv[2],"copyright") == 0) {
            if (plugins[num]->copyright) {
                printf ("%s\n", plugins[num]->copyright);
            }
            else {
                printf ("(no copyright info)\n");
            }
        }
        else if (strcmp (argv[2],"website") == 0) {
            if (plugins[num]->website) {
                printf ("%s\n", plugins[num]->website);
            }
            else {
                printf ("(no website)\n");
            }
        }
        else if (strcmp (argv[2],"config") == 0) {
            if (plugins[num]->configdialog) {
                // list options
                int i;
                struct property * prop;
                for (i = 0; 1; i++) {
                    prop = property_gen (plugins[num], i);
                    if (!prop) {
                        break;
                    }
                    printf ("%s - %s (default: %s)\n", prop->key, prop->name, prop->def);
                    property_free (prop);
                }

            }
            else {
                printf ("(no config)\n");
            }
        }
        else if (strcmp (argv[2],"version") == 0) {
            printf ("%s v%d.%d\n", plugins[num]->name, plugins[num]->version_major, plugins[num]->version_minor);
        }
        else if (strcmp (argv[2],"type") == 0) {
            const char * types[] = {"DB_PLUGIN_DECODER", "DB_PLUGIN_OUTPUT", "DB_PLUGIN_DSP", "DB_PLUGIN_MISC", "DB_PLUGIN_VFS", "DB_PLUGIN_PLAYLIST", "DB_PLUGIN_GUI", NULL };
            printf ("%s: %s\n", plugins[num]->name, types[plugins[num]->type - 1]);
        }
        return NULL;
    }
    if (argc == 4) {
        if (strcmp (argv[2],"config") == 0) {
            if (plugins[num]->configdialog) {
                if (iter != -1) {
                    int i;
                    int j = 0;
                    struct property * prop;
                    for (i = 0; 1; i++) {
                        prop = property_gen (plugins[num], i);
                        if (!prop) {
                            break;
                        }
                        char * comp = strdup_unescaped (argv[3]);
                        if (strncmp (comp, prop->key, strlen (comp)) == 0) {
                            if (j == iter) {
                                char * out = strdup (prop->key);
                                property_free (prop);
                                return out;
                            }
                            else {
                                j++;
                            }
                        }
                        property_free (prop);
                    }
                    return NULL;
                }
                else {
                    // find prop
                    struct property * prop_curr = 0;
                    {
                        int i;
                        struct property * prop;
                        for (i = 0; 1; i++) {
                            prop = property_gen (plugins[num], i);
                            if (!prop) {
                                break;
                            }
                            char * comp = strdup_unescaped (argv[3]);
                            if (strcmp (comp, prop->key) == 0) {
                                prop_curr = prop;
                                break;
                            }
                            property_free (prop);
                        }
                    }
                    if (!prop_curr) {
                        printf ("Property %s for plugin %s not found.\n", argv[3], plugin_names[num]);
                        return NULL;
                    }
                    // print option
                    char * value = "(no value)";
                    if (prop_curr->val) {
                        value = prop_curr->val;
                    }
                    printf ("%s = %s (default: %s)\n", prop_curr->key, value, prop_curr->def);
                    return NULL;
                }
            }
            return NULL;
        }
    }
    if (argc == 5) {
        if (strcmp (argv[2],"config") == 0) {
            if (plugins[num]->configdialog) {
                if (iter != -1) {
                    return NULL;
                }
                else {
                    // find prop
                    struct property * prop_curr = 0;
                    {
                        int i;
                        struct property * prop;
                        for (i = 0; 1; i++) {
                            prop = property_gen (plugins[num], i);
                            if (!prop) {
                                break;
                            }
                            char * comp = strdup_unescaped (argv[3]);
                            if (strcmp (comp, prop->key) == 0) {
                                prop_curr = prop;
                                break;
                            }
                            property_free (prop);
                        }
                    }
                    if (!prop_curr) {
                        printf ("Property %s for plugin %s not found.\n", argv[3], plugin_names[num]);
                        return NULL;
                    }
                    // todo evaluate prop_curr->type_string
                    char *config_argv[] = {"config_i", argv[3], "string", argv[4], NULL };
                    settings_config (4, config_argv, -1);
                    property_free (prop_curr);
                    //printf ("config option called, todo\n");
                }
            }
            return NULL;
        }
    }
    TAB_COMPLETION_END;
    return CMD_NOTFOUND;
}

char * cmd_settings (int argc, char * argv[], int iter) {
    TAB_COMPLETION_TABLE(1, main_s);
    if (argc >= 3 && iter != -1) {
        int num = main_num(argv[1]);
        if (num == -1) {
            return NULL;
        }
        return main_f[num] (argc-1, argv+1, iter);
    }
    TAB_COMPLETION_END
    if (argc == -1) {
        printf ("%s: customize player behavior\n", cmd_name(argv[0]));
        printf ("\t config - set config values manually\n");
        printf ("\t plugins - plugin specific options/information\n");
        // To be extended...
    }
    if (argc == 1) {
        return CMD_DIRECTORY;
    }

    if (argc >= 2) {
        int num = main_num(argv[1]);
        if (num != -1) {
            return main_f[num] (argc-1, argv+1, iter);
        }
    }
    return CMD_NOTFOUND;
}
