SRCDIR=./src
CC=gcc
INCLUDE = C:\msys64\mingw64\include
CFLAGS = -g -I$(INCLUDE) -c
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -mwindows -lm -mconsole

Server: $(SRCDIR)/Server.c
	@echo "Building Server Application"
	$(CC) $(CFLAGS) $(SRCDIR)/Server.c -o $@.o
	$(CC) Server.o -o Server.exe $(LDFLAGS)