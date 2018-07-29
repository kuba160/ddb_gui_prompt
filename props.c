// File generated by properties_gen (2018-07-29 22:48:46)
#include <deadbeef/deadbeef.h>
#include "cmd_tools.h"
#include "common.h"

struct property sound_00 = {
	.name = "Output plugin:",
	.type = 8,
	.type_string = "select_s",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "output_plugin",
	.val = 0,
	.val_possible = 0,
	.def = "ALSA output plugin",
	.group = { NULL },
	.requires = NULL
};

struct property sound_01 = {
	.name = "Output device:",
	.type = 8,
	.type_string = "select_s",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "ID_soundcard",
	.val = 0,
	.val_possible = 0,
	.def = "default",
	.group = { NULL },
	.requires = NULL
};

struct property sound_02 = {
	.name = "Always convert 8 bit audio to 16 bit",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "streamer.8_to_16",
	.val = 0,
	.val_possible = 0,
	.def = "1",
	.group = { NULL },
	.requires = NULL
};

struct property sound_03 = {
	.name = "Always convert 16 bit audio to 24 bit",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "streamer.16_to_24",
	.val = 0,
	.val_possible = 0,
	.def = "0",
	.group = { NULL },
	.requires = NULL
};

struct property sound_04 = {
	.name = "Override samplerate",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "streamer.override_samplerate",
	.val = 0,
	.val_possible = 0,
	.def = "0",
	.group = { "noname", NULL },
	.requires = NULL
};

struct property sound_05 = {
	.name = "Target samplerate:",
	.type = 0,
	.type_string = "entry",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "streamer.samplerate",
	.val = 0,
	.val_possible = 0,
	.def = "44100",
	.group = { "noname", NULL },
	.requires = &sound_04
};

struct property sound_06 = {
	.name = "Based on input samplerate",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "streamer.use_dependent_samplerate",
	.val = 0,
	.val_possible = 0,
	.def = "0",
	.group = { "noname", "noname2", NULL },
	.requires = &sound_04
};

struct property sound_07 = {
	.name = "For multiples of 48KHz (96K, 192K, ...):",
	.type = 0,
	.type_string = "entry",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "streamer.samplerate_mult_48",
	.val = 0,
	.val_possible = 0,
	.def = "48000",
	.group = { "noname", "noname2", NULL },
	.requires = &sound_06
};

struct property sound_08 = {
	.name = "For multiples of 44.1KHz (88.2K, 176.4K, ...):",
	.type = 0,
	.type_string = "entry",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "streamer.samplerate_mult_44",
	.val = 0,
	.val_possible = 0,
	.def = "44100",
	.group = { "noname", "noname2", NULL },
	.requires = &sound_06
};

struct property * p_sound[10] = {
	&sound_00, &sound_01, &sound_02, &sound_03, &sound_04, &sound_05, 
	&sound_06, &sound_07, &sound_08, NULL
};

struct property playback_00 = {
	.name = "Source mode:",
	.type = 7,
	.type_string = "select",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 3,
	.key = "replaygain.source_mode",
	.val = 0,
	.val_possible = (char *[]) {
		"By playback order",
		"Track",
		"Album"
		},
	.def = "By playback order",
	.group = { "Replaygain", NULL },
	.requires = NULL
};

struct property playback_01 = {
	.name = "Processing:",
	.type = 7,
	.type_string = "select",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 4,
	.key = "replaygain.processing_flags",
	.val = 0,
	.val_possible = (char *[]) {
		"None",
		"Apply gain",
		"Apply gain and prevent clipping according to peak",
		"Only prevent clipping"
		},
	.def = "None",
	.group = { "Replaygain", NULL },
	.requires = NULL
};

struct property playback_02 = {
	.name = "Preamp with RG info:",
	.type = 4,
	.type_string = "hscale",
	.type_min = -12,
	.type_max = 12,
	.type_step = 1,
	.type_count = 0,
	.key = "replaygain.preamp_with_rg",
	.val = 0,
	.val_possible = 0,
	.def = "0",
	.group = { "Replaygain", NULL },
	.requires = NULL
};

struct property playback_03 = {
	.name = "Preamp without RG info:",
	.type = 4,
	.type_string = "hscale",
	.type_min = -12,
	.type_max = 12,
	.type_step = 1,
	.type_count = 0,
	.key = "replaygain.preamp_without_rg",
	.val = 0,
	.val_possible = 0,
	.def = "0",
	.group = { "Replaygain", NULL },
	.requires = NULL
};

struct property playback_04 = {
	.name = "Add files from command line (or file manager) to this playlist:",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "cli_add_to_specific_playlist",
	.val = 0,
	.val_possible = 0,
	.def = "1",
	.group = { NULL },
	.requires = NULL
};

struct property playback_05 = {
	.name = "Add files from command line (or file manager) to this playlist:",
	.type = 0,
	.type_string = "entry",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "cli_add_playlist_name",
	.val = 0,
	.val_possible = 0,
	.def = "Default",
	.group = { NULL },
	.requires = NULL
};

struct property playback_06 = {
	.name = "Resume previous session on startup",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "resume_last_session",
	.val = 0,
	.val_possible = 0,
	.def = "1",
	.group = { NULL },
	.requires = NULL
};

struct property playback_07 = {
	.name = "Don't add from archives when adding folders",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "ignore_archives",
	.val = 0,
	.val_possible = 0,
	.def = "1",
	.group = { NULL },
	.requires = NULL
};

struct property playback_08 = {
	.name = "\"Stop after current track\" option will switch off after triggering",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "playlist.stop_after_current_reset",
	.val = 0,
	.val_possible = 0,
	.def = "0",
	.group = { NULL },
	.requires = NULL
};

struct property playback_09 = {
	.name = "\"Stop after current album\" option will switch off after triggering",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "playlist.stop_after_album_reset",
	.val = 0,
	.val_possible = 0,
	.def = "0",
	.group = { NULL },
	.requires = NULL
};

struct property * p_playback[11] = {
	&playback_00, &playback_01, &playback_02, &playback_03, &playback_04, &playback_05, 
	&playback_06, &playback_07, &playback_08, &playback_09, NULL
};

struct property gui_misc_00 = {
	.name = "Close minimizes to tray",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "close_send_to_tray",
	.val = 0,
	.val_possible = 0,
	.def = "0",
	.group = { NULL },
	.requires = NULL
};

struct property gui_misc_01 = {
	.name = "Enable Japanese SHIFT-JIS detection and recoding",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "junk.enable_shift_jis_detection",
	.val = 0,
	.val_possible = 0,
	.def = "0",
	.group = { NULL },
	.requires = NULL
};

struct property gui_misc_02 = {
	.name = "Enable Russian CP1251 detection and recoding",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "junk.enable_cp1251_detection",
	.val = 0,
	.val_possible = 0,
	.def = "0",
	.group = { NULL },
	.requires = NULL
};

struct property gui_misc_03 = {
	.name = "Enable Chinese CP936 detection and recoding",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "junk.enable_cp936_detection",
	.val = 0,
	.val_possible = 0,
	.def = "0",
	.group = { NULL },
	.requires = NULL
};

struct property gui_misc_04 = {
	.name = "GUI Plugin (changing requires restart)",
	.type = 7,
	.type_string = "select",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "gui_plugin",
	.val = 0,
	.val_possible = 0,
	.def = "GTK2",
	.group = { NULL },
	.requires = NULL
};

struct property * p_gui_misc[6] = {
	&gui_misc_00, &gui_misc_01, &gui_misc_02, &gui_misc_03, &gui_misc_04, NULL
};

struct property network_00 = {
	.name = "Enable Proxy Server",
	.type = 3,
	.type_string = "checkbox",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "network.proxy",
	.val = 0,
	.val_possible = 0,
	.def = "0",
	.group = { NULL },
	.requires = NULL
};

struct property network_01 = {
	.name = "Proxy Server Address:",
	.type = 0,
	.type_string = "entry",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "network.proxy.address",
	.val = 0,
	.val_possible = 0,
	.def = "",
	.group = { NULL },
	.requires = NULL
};

struct property network_02 = {
	.name = "Proxy Server Port",
	.type = 0,
	.type_string = "entry",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "network.proxy.port",
	.val = 0,
	.val_possible = 0,
	.def = "8080",
	.group = { NULL },
	.requires = NULL
};

struct property network_03 = {
	.name = "Proxy Username:",
	.type = 0,
	.type_string = "entry",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "network.proxy.username",
	.val = 0,
	.val_possible = 0,
	.def = "",
	.group = { NULL },
	.requires = NULL
};

struct property network_04 = {
	.name = "Proxy Password:",
	.type = 1,
	.type_string = "password",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "network.proxy.password",
	.val = 0,
	.val_possible = 0,
	.def = "",
	.group = { NULL },
	.requires = NULL
};

struct property network_05 = {
	.name = "HTTP User Agent",
	.type = 0,
	.type_string = "entry",
	.type_min = 0,
	.type_max = 0,
	.type_step = 0,
	.type_count = 0,
	.key = "network.http_user_agent",
	.val = 0,
	.val_possible = 0,
	.def = "deadbeef",
	.group = { NULL },
	.requires = NULL
};

struct property * p_network[7] = {
	&network_00, &network_01, &network_02, &network_03, &network_04, &network_05, NULL
};

