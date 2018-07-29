#ifndef _COMMON_H_
#define _COMMON_H_

#define USE_COLORS
#ifdef USE_COLORS
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KBLD  "\x1B[1m" // bold
#define KDIM  "\x1B[2m" // dim
#else
#define KNRM ""
#define KRED ""
#define KGRN ""
#define KYEL ""
#define KBLU ""
#define KMAG ""
#define KCYN ""
#define KWHT ""
#define KBLD ""
#define KDIM ""
#endif

//#define USE_UNICODE_SYMBOLS
#ifdef USE_UNICODE_SYMBOLS
#define SYMBOL_PLAY "▶"
#define SYMBOL_PAUSED "⏸"
#define SYMBOL_STOPPED "◾"
#else
#define SYMBOL_PLAY ">"
#define SYMBOL_PAUSED "/"
#define SYMBOL_STOPPED ">"
#endif

#define ENABLE_NLS
#ifdef ENABLE_NLS
#include <libintl.h>
#define _(s) gettext(s)
#else
#define _(s) (s)
#endif


void call_on_exit_pop (void (*func)(void));

void readline_reset (void);

#endif
