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
#include "props.h"

extern DB_functions_t *deadbeef;

const char * main_s[] = {"plugins", "config", "sound", NULL};
char * (*main_f[])(int, char *[], int) = {settings_plugins, settings_config, settings_sound, NULL};

// Settings
// - Sound
//      Output plugin selection
//      Soundcard (output device) selection
//      /
//      Convert 8 to 16
//      Convert 16 to 24
//      / Samplerate
//      Samplerate override
//          Direct/Dependent samplerate
//          Direct samplerate
//          48 kHz Samplerate
//          44.1 kHz Samplerate
// - Playback
//      Replayagin
//          Replaygain mode
//          Replaygain processing
//          Replaygain preamp
//          Preamp without replaygain
//      Add to playlist cli_playlist_name
//      Resume session on startup
//      


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
    if (argc == -1) {
        printf ("config: show or edit config value (useful if you know what you're doing)\n");
        printf ("\t1: key\n");
        printf ("\t2: type [string, int, int64, float, delete()]\n");
        printf ("\t3: value (optional)\n");
    }
    if (argc == 1) {
        return CMD_DIRECTORY;
    }
    else if (argc == 2) {
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
                if (strcmp (item2->key, argv[1]) != 0) {
                    printf ("Config option %s not found.\n", argv[1]);
                    return NULL;
                }
            }
        }
        printf ("%s = %s\n", item->key, item->value);
        return NULL;
    }
    else if (argc == 3) {
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
    else if (argc == 4) {
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
                if (strcmp (item2->key, argv[1]) != 0) {
                    printf ("Config option %s not found, creating.\n", argv[1]);
                }
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

// MAX_PLUGINS = 100 (defined in deadbeef/plugins.c)
struct DB_plugin_s * plugins[101] = {NULL};
const char * plugin_names[101];
property_t ** plugin_properties[101];
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
            // properties
            if (plgs[i]->configdialog) {
                plugin_properties[i] = properties_alloc (plgs[i]->configdialog);
            }
            else {
                plugin_properties[i] = NULL;
            }
            pos++;
            //}
        }
        plugins[i] = NULL;
        plugin_names[i] = NULL;
        plug_table_generated = 1;
        call_on_exit_pop (plug_table_destroy);
    }
}

void plug_table_destroy () {
    if (plug_table_generated) {
        int i;
        for (i = 0; plugins[i] != 0; i++) {
            if (i >= 100) {
                printf ("plug list max!\n");
                break;
            }
            if (plugins[i]->configdialog) {
                properties_free (plugin_properties[i]);
            }
        }
        plugin_properties[0] = NULL;
        plugin_names[0] = NULL;
        plugins[0] = NULL;
        plug_table_generated = 0;
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
                properties_print (plugin_properties[num]);
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
        else {
            printf ("Unknown option \"%s\"\n", argv[2]);
        }
        return NULL;
    }
    if (argc == 4) {
        if (strcmp (argv[2],"config") == 0) {
            if (plugins[num]->configdialog) {
                TAB_COMPLETION_PROPERTIES(3,plugin_properties[num]);
                property_t * prop = property_get (plugin_properties[num], argv[3]);
                if (!prop) {
                    printf ("Property %s for plugin %s not found.\n", argv[3], plugin_names[num]);
                    return NULL;
                }
                property_print (prop);
                return NULL;
            }
            else {
                return CMD_NOTFOUND;
            }
            return NULL;
        }
    }
    if (argc == 5) {
        if (strcmp (argv[2],"config") == 0) {
            if (plugins[num]->configdialog) {
                TAB_COMPLETION_PROPERTIES_OPTION(4,3,plugin_properties[num]) 

                else {
                    // find prop
                    struct property * prop_curr = property_get (plugin_properties[num], argv[3]);
                    if (!prop_curr) {
                        printf ("Property %s for plugin %s not found.\n", argv[3], plugin_names[num]);
                        return NULL;
                    }
                    property_set (prop_curr, argv[4]);
                }
            }
            return NULL;
        }
    }
    TAB_COMPLETION_END;
    return CMD_NOTFOUND;
}

// output
struct DB_output_s * plugins_output[8] = {NULL};
const char * plugins_output_names[8] = {NULL};
char plugins_output_initiated = 0;

struct soundcards_userdata {
    int i;
    int size;
    char **out;
    char **out_names;
};

void
enum_soundcards_callback (const char *name, const char *desc, void *userdata) {
    struct soundcards_userdata * u = (struct soundcards_userdata *) userdata;
    if (u->i == -1) {
        return;
    }
    if (u->i >= u->size) {
        u->size *= 2;
        u->out = realloc (u->out, u->size * sizeof (char *));
        u->out_names = realloc (u->out_names, u->size * sizeof (char *));
        if (!u->out || !u->out_names) {
            printf ("reallocation failed!\n");
            u->i = -1;
            return;
        }
    }
    u->out[u->i] = strdup (desc);
    u->out_names[u->i] = strdup (name);
    u->i++;
    return;
}

char **soundcards = NULL;

void settings_sound_update () {
    struct property * p;
    if (!plugins_output_initiated) {
        struct DB_output_s ** plgs = deadbeef->plug_get_output_list ();
        int i;
        for (i = 0; plgs[i]; i++) {
            if (i >= 8) {
                printf ("plugins_output max!\n");
                break;
            }
            plugins_output[i] = plgs[i];
            if (plgs[i]->plugin.id)
                plugins_output_names[i] = plgs[i]->plugin.id;
            else
                plugins_output_names[i] = plgs[i]->plugin.name;
        }
        plugins_output[i] = NULL;
        plugins_output_names[i] = NULL;

        // translations
        #ifdef ENABLE_NLS
        for (i = 0; p_sound[i]; i++) {
            p_sound[i]->name = _(p_sound[i]->name);
        }
        #endif
        // output plugins
        p = p_sound[0];
        if (1) {
            int c;
            for (c = 0; plugins_output[c]; c++);
            p->type_count = c;
            p->val_possible = malloc ((c+1) * sizeof (char *));
            int d;
            for (d = 0; plugins_output[d]; d++) {
                p->val_possible[d] = (char *) plugins_output[d]->plugin.name;
            }
            p->val_possible[d] = 0;
        }
        call_on_exit_pop (settings_sound_destroy);
    }
    // soundcard update
    p = (struct property *) p_sound[1];
    struct DB_plugin_s *output = (struct DB_plugin_s *) (struct DB_output_s *) deadbeef->get_output ();
    if (p && output && strncmp (p->key, output->id, strlen(output->id)) != 0) {
        if (plugins_output_initiated) {
            free (p->key);
            free (p->val);
            if (p->type_count) {
                int a;
                for (a = 0; p->val_possible[a]; a++) {
                    free (p->val_possible[a]);
                    free (soundcards[a]);
                }
                free (p->val_possible);
                if (soundcards) {
                    free (soundcards);
                    soundcards = NULL;
                }

            }
        }
        char buf[strlen(output->id) + 10 + 1];
        strcpy (buf, output->id);
        strcat (buf,"_soundcard");
        p->key = strdup (buf);
        if (((struct DB_output_s *) output)->enum_soundcards) {
            struct soundcards_userdata scards = {0, 0, NULL};
            scards.out = malloc (16 * sizeof (char *));
            scards.out_names = malloc (16 * sizeof (char *));
            scards.size = 16;
            ((struct DB_output_s *) output)->enum_soundcards (enum_soundcards_callback, &scards);
            if (scards.i >= scards.size) {
                scards.size += 2;
                scards.out = realloc (scards.out, scards.size * sizeof (char *));
                scards.out_names = realloc (scards.out_names, scards.size * sizeof (char *));
            }
            else {
                scards.size = scards.i + 2;
                scards.out = realloc (scards.out, scards.size * sizeof (char *));
                scards.out_names = realloc (scards.out_names, scards.size * sizeof (char *));
            }
            scards.out[scards.i] = strdup ("default");
            scards.out_names[scards.i++] = strdup ("default");
            scards.out[scards.i] = NULL;
            scards.out_names[scards.i] = NULL;
            p->val_possible = scards.out;
            p->type_count = scards.i;
            soundcards = scards.out_names;
            p->val = 0;
        }
        else {
            p->type_count = 0;
            p->val = strdup ("default");
        }
    }
    plugins_output_initiated = 1;
}

void settings_sound_destroy () {
        if (plugins_output_initiated) {
            property_t *p = p_sound[0];
            if (p->val_possible)
                free (p->val_possible);
            p = p_sound[1];
            free (p->key);
            free (p->val);
            if (p->type_count) {
                int a;
                for (a = 0; p->val_possible[a]; a++) {
                    free (p->val_possible[a]);
                    free (soundcards[a]);
                }
                free (p->val_possible);
                if (soundcards) {
                    free (soundcards);
                    soundcards = NULL;
                }
            }
        }
}

char * settings_sound (int argc, char * argv[], int iter) {
    // TODO
    settings_sound_update ();
    TAB_COMPLETION_PROPERTIES(1,p_sound);
    TAB_COMPLETION_PROPERTIES_OPTION(2,1,p_sound);
    TAB_COMPLETION_END;
    if (argc == -1) {
        printf ("sound: customize output sound\n");
    }
    else if (argc == 1) {
        properties_print (p_sound);
        return NULL;
    }
    else if (argc == 2) {
        property_t * prop = property_get (p_sound, argv[1]);
        if (!prop) {
            printf ("Property %s not found.\n", argv[1]);
            return NULL;
        }
        property_print (prop);
        return NULL;
    }
    else if (argc == 3) {
        property_t * prop = property_get (p_sound, argv[1]);
        if (prop) {
            int ret = 0;
            if (prop == p_sound[1]) {
                int i;
                char * unescaped = strdup_unescaped (argv[2]);
                char * name = NULL;
                for (i = 0; prop->val_possible[i]; i++) {
                    if (strcmp(prop->val_possible[i], unescaped) == 0) {
                        name = soundcards[i];
                        break;
                    }
                }
                free (unescaped);
                if (name) {
                    deadbeef->conf_set_str (prop->key, name);
                    ret = 0;
                }
                else {
                    printf ("Unknown soundcard.\n");
                    return NULL;
                }
            }
            else {
                ret = property_set (prop, argv[2]);
            }
            if (!ret && strcmp (prop->key, "output_plugin") == 0) {
                deadbeef->sendmessage (DB_EV_REINIT_SOUND, 0, 0, 0);
                readline_reset ();
            }
            return NULL;
        }
        else {
            printf ("Property %s not found\n",argv[1]);
            return NULL;
        }
    }
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
        if (argv[1]) {
            int num = main_num(argv[1]);
            if (num != -1)
                return main_f[num] (-1, argv+1, -1);
            else
                return CMD_NOTFOUND;
        }
        printf ("%s: customize player behavior\n", cmd_name(argv[0]));
        printf ("\t %-8s %-2s %s", "config",  "-", "set config values manually\n");
        printf ("\t %-8s %-2s %s", "plugins", "-", "plugin specific options/information\n");
        printf ("\t %-8s %-2s %s", "sound",   "-", "various output settings\n");
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
