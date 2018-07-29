char * configdialog = 
"tab sound;\n"
"property \"Output plugin:\" select_s[0] output_plugin \"ALSA output plugin\" ... ;\n"
"property \"Output device:\" select_s[0] ID_soundcard default ... ;\n"
"property \"Always convert 8 bit audio to 16 bit\" checkbox streamer.8_to_16 1;\n"
"property \"Always convert 16 bit audio to 24 bit\" checkbox streamer.16_to_24 0;\n"
"group_begin noname;\n"// override samplerate
    "group_require property \"Override samplerate\" checkbox streamer.override_samplerate 0;\n"
    "property \"Target samplerate:\" entry streamer.samplerate \"44100\";\n"
    "group_begin noname2;\n"// based on input samplerate
        "group_require property \"Based on input samplerate\" checkbox streamer.use_dependent_samplerate 0;\n"
        "property \"For multiples of 48KHz (96K, 192K, ...):\" entry streamer.samplerate_mult_48 \"48000\";\n"
        "property \"For multiples of 44.1KHz (88.2K, 176.4K, ...):\" entry streamer.samplerate_mult_44 \"44100\";\n"
    "group_end;\n"
"group_end;\n"

"tab playback;\n"
"group_begin Replaygain;\n"
    "property \"Source mode:\" select[3] replaygain.source_mode 0 \"By playback order\" \"Track\" \"Album\";\n"
    "property \"Processing:\" select[4] replaygain.processing_flags 0 \"None\" \"Apply gain\" \"Apply gain and prevent clipping according to peak\""
     " \"Only prevent clipping\";\n"
    // missing suffix (dB) from original in next 2
    "property \"Preamp with RG info:\" hscale[-12,12,1] replaygain.preamp_with_rg 0;\n"
    "property \"Preamp without RG info:\" hscale[-12,12,1] replaygain.preamp_without_rg 0;\n"
"group_end;\n"
// todo these two are merged in one
"property \"Add files from command line (or file manager) to this playlist:\" checkbox cli_add_to_specific_playlist 1;\n"
"property \"Add files from command line (or file manager) to this playlist:\" entry cli_add_playlist_name \"Default\";\n"
"property \"Resume previous session on startup\" checkbox resume_last_session 1;\n"
"property \"Don't add from archives when adding folders\" checkbox ignore_archives 1;\n"
// double "" ?
"property \"\"Stop after current track\" option will switch off after triggering\" checkbox playlist.stop_after_current_reset 0;\n"
"property \"\"Stop after current album\" option will switch off after triggering\" checkbox playlist.stop_after_album_reset 0;\n"

// DSP

// GUI/Misc
"tab gui_misc;\n"
//      Player
"property \"Close minimizes to tray\" checkbox close_send_to_tray 0;\n"
//"property \"Hide system tray icon\" checkbox gtkui.hide_tray_icon 0;\n"
"property \"Enable Japanese SHIFT-JIS detection and recoding\" checkbox junk.enable_shift_jis_detection 0;\n"
"property \"Enable Russian CP1251 detection and recoding\" checkbox junk.enable_cp1251_detection 0;\n"
"property \"Enable Chinese CP936 detection and recoding\" checkbox junk.enable_cp936_detection 0;\n"
//"property \"Interface refresh rate (times per second)\" hscale[1,30,1] gtkui.refresh_rate 10;\n"
//"property \"Titlebar text while playing\" entry gtk.titlebar_playing_tf \"%artist% - %title% - DeaDBeeF-%_deadbeef_version%\";"
//"property \"Titlebar text while stopped\" entry gtk.titlebar_stopped_tf \"DeaDBeeF-%_deadbeef_version%\";\n"
"property \"GUI Plugin (changing requires restart)\" select[0] gui_plugin \"GTK2\" ...;\n"
//"property \"Display selection playback time in statusbar\" checkbox gtkui.statusbar_seltime 0;\n"
//      Playlist
// MISSING

// Apperance
// MISSING

// Network
"tab network;\n"
"property \"Enable Proxy Server\" checkbox network.proxy 0;\n"
"property \"Proxy Server Address:\" entry network.proxy.address \"\";\n"
"property \"Proxy Server Port\" entry network.proxy.port \"8080\";\n"
// network.proxy.type should be a string "property \"Proxy Type:\" select[6] network.proxy.type 0 HTTP HTTP_1_0 SOCKS4 SOCKS5 SOCKS4A SOCKS5_HOSTNAME;\n"
"property \"Proxy Username:\" entry network.proxy.username \"\";\n"
"property \"Proxy Password:\" password network.proxy.password \"\";\n"
"separator;\n"
"property \"HTTP User Agent\" entry network.http_user_agent \"deadbeef\";\n"
// content type mapping

// Hotkeys
// MISSING

// Plugins
// MISSING
;
