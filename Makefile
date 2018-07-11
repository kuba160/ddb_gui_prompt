# Makefile for ddb_gui_prompt plugin
ifeq ($(OS),Windows_NT)
	SUFFIX = dll
else
	SUFFIX = so
endif

CC=gcc
STD=gnu99
CFLAGS=-fPIC -I /usr/local/include -O2 -Wall
ifeq ($(DEBUG),1)
CFLAGS +=-g -O0
endif
PREFIX=/usr/local/lib/deadbeef
PLUGNAME=prompt
PLUGNAME_PREFIX=ddb_gui
LIBS=-lreadline
C_FILES= assets.c cmd.c
C_FILES_OUT = $(C_FILES:.c=.o)
all:
	$(CC) -std=$(STD) -c $(CFLAGS) $(PLUGNAME).c $(C_FILES)
	$(CC) -std=$(STD) -shared $(CFLAGS) -o $(PLUGNAME_PREFIX)_$(PLUGNAME).$(SUFFIX) $(PLUGNAME).o $(C_FILES_OUT) $(LIBS)
install:
	cp $(PLUGNAME_PREFIX)_$(PLUGNAME).$(SUFFIX) $(PREFIX)
clean:
	rm -fv $(PLUGNAME).o $(C_FILES_OUT) $(PLUGNAME_PREFIX)_$(PLUGNAME).$(SUFFIX)