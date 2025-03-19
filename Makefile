SRCDIR=./src
CC=gcc
INCLUDE = C:\msys64\mingw64\include
CFLAGS = -g -I$(INCLUDE) -c
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -mwindows

game:
	@echo "Building game"
	$(CC) $(CFLAGS) $(SRCDIR)/hello.c -o $@.o
	$(CC) game.o -o game $(LDFLAGS)

