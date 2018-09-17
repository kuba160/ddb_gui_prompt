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

#include <deadbeef/deadbeef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "common.h"
#include "cmd.h"

DB_gui_t plugin;
DB_functions_t *deadbeef;

#define trace(...) { deadbeef->log_detailed (&plugin.plugin, 0, __VA_ARGS__); }

int ui_running=1;

extern unsigned char greeter_big[];
extern unsigned char greeter_small[];

// show current playlist on prompt
#define PLAYLIST_AT_PROMPT

int intcount = 0;
time_t lastint = 0;
// intHandler handles Ctrl-C signal, it will close player after three signals within 2 seconds
void intHandler (int dummy) {
    // print prompt again
    readline_reset ();
    if (!lastint) {
        lastint = time (0);
    }
    time_t curr_time = time (0);

    if (curr_time > lastint+2) {
        intcount = 0;
        lastint = curr_time;
    }
    else if (intcount >= 2) {
        deadbeef->sendmessage (DB_EV_TERMINATE, 0, 0, 0);
        ui_running = 0;
        close (STDIN_FILENO);
    }
    intcount++;
    return;
}


static int
ui_start (void) {
    
    if (!isatty(fileno(stdin)) || !isatty(fileno(stdout))) {
        deadbeef->sendmessage (DB_EV_TERMINATE, 0, 0, 0);
        return 1;
    }
    
    trace ("ui_start\n");
    signal(SIGINT, intHandler);
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    if (w.ws_col >= 115)
        printf ("%s",greeter_big);
    else if (w.ws_col >= 80)
        printf ("%s",greeter_small);
    
    // readline completion function
    rl_attempted_completion_function = cmd_completion;
    rl_completer_quote_characters = "\"";

    char *buffer;
    char prompt[64];
    while (ui_running) {
        char * prompt_path = cmd_get_path ();
        if (prompt_path) {
            snprintf (prompt, 64, "%s >", prompt_path);
            prompt[0] = toupper (prompt[0]);
        }
        else {
            #ifdef PLAYLIST_AT_PROMPT
            ddb_playlist_t *plt;
            plt = deadbeef->plt_get_curr();
            char plt_name[32];
            deadbeef->plt_get_title (plt, plt_name, 32);
            deadbeef->plt_unref (plt);
            snprintf (prompt, 64, "(%s) >",plt_name);
            
            #else
            strcpy (prompt, ">");
            #endif
        }
        buffer = readline(prompt);

        if (!buffer) {
            if (ui_running){
                printf ("\n");
                continue;
            }
            else {
                break;
            }
        }
        int cmd_ret = -666;
        cmd_ret = cmd (buffer);

        if (strlen(buffer) > 0 && buffer[0] != ' ') {
            add_history(buffer);
        }
        if (cmd_ret == -1 && buffer[0] != 0) {
            char *p = strchr (buffer, ' ');
            if (p)
                *p = 0;
            if (strlen (buffer) != 0)
                fprintf (stderr, "%s: command not found\n",buffer);
        }
        free (buffer);
    }
    return 0;
}

static int
ui_connect (void) {
    return 0;
}

static int
ui_disconnect (void) {
    return 0;
}

void readline_reset (void) {
    rl_backward_kill_line (0, 0);
    fputc ('\n', stdout);
    fflush (stdout);
    rl_forced_update_display ();
}

void (*call_on_exit[8]) (void) = {NULL};

void call_on_exit_pop (void (*func)(void)) {
	int i;
	for (i = 0; call_on_exit[i]; i++);
	if (i >= 8-1)
	   return;
	call_on_exit[i] = func;
	call_on_exit[i+1] = NULL;
}

static int
ui_stop (void) {
    ui_running = 0;
    int i;
    for (i = 0; call_on_exit[i]; i++) {
        call_on_exit[i] ();
        call_on_exit[i] = NULL;
    }
    return 0;
}

static int
ui_message (uint32_t id, uintptr_t ctx, uint32_t p1, uint32_t p2) {
    switch (id) {
#if 0
    // todo? inform about trackchange
    case DB_EV_SONGCHANGED:
        break;
#endif
    case DB_EV_TERMINATE: 
        ui_running = 0;
        break;

    }
    return 0;
}

int
ui_run_dialog (ddb_dialog_t *conf, uint32_t buttons, int (*callback)(int button, void *ctx), void *ctx) {
    fprintf (stderr, "run_dialog: title=%s\n", conf->title);
    // TODO: implement
    return ddb_button_cancel;
}


DB_plugin_t *
ddb_gui_prompt_load (DB_functions_t *api) {
    deadbeef = api;
    return DB_PLUGIN (&plugin);
}

// define plugin interface
DB_gui_t plugin = {
    .plugin.api_vmajor = 1,
    .plugin.api_vminor = 10,
    .plugin.version_major = 0,
    .plugin.version_minor = 5,
    .plugin.type = DB_PLUGIN_GUI,
    .plugin.id = "prompt",
    .plugin.name = "Console user interface (prompt)",
    .plugin.descr = "User interface using standard input/output with support of readline",
    .plugin.copyright =
        "Console user interface for DeaDBeeF Player.\n"
    "Copyright (C) 2018 Jakub Wasylków <kuba_160@protonmail.com>\n"
    "\n"
    "This software is provided 'as-is', without any express or implied\n"
    "warranty.  In no event will the authors be held liable for any damages\n"
    "arising from the use of this software.\n"
    "\n"
    "Permission is granted to anyone to use this software for any purpose,\n"
    "including commercial applications, and to alter it and redistribute it\n"
    "freely, subject to the following restrictions:\n"
    "\n"
    "1. The origin of this software must not be misrepresented; you must not\n"
    " claim that you wrote the original software. If you use this software\n"
    " in a product, an acknowledgment in the product documentation would be\n"
    " appreciated but is not required.\n"
    "\n"
    "2. Altered source versions must be plainly marked as such, and must not be\n"
    " misrepresented as being the original software.\n"
    "\n"
    "3. This notice may not be removed or altered from any source distribution.\n",
    .plugin.website = "http://github.com/kuba160",
    .plugin.start = ui_start,
    .plugin.stop = ui_stop,
    .plugin.connect = ui_connect,
    .plugin.disconnect = ui_disconnect,
    .plugin.message = ui_message,
    .run_dialog = ui_run_dialog,
};
