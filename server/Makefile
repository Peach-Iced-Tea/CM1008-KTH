SRCDIR = ./src
INCDIR = ./include
SHAREDINC = ../include
SHAREDSRC = ../src
CC = gcc
ifeq ($(OS),Windows_NT)
	INCLUDE = -IC:/msys64/mingw64/include/SDL2 -IC:/msys64/mingw64/include/libxml2 -I$(INCDIR) -I$(SHAREDINC)
	LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -mwindows -lm -mconsole -lxml2 -ljansson
	DELCMD = del
else
	INCLUDE = -I$(INCDIR) -I$(SHAREDINC)
	LDFLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lm
	DELCMD = rm
endif

CFLAGS = -g $(INCLUDE) -c
TARGET = server.exe


$(TARGET): server.o entity.o vmath.o physics.o utils.o player.o input.o mapParser.o
	$(CC) -o $(TARGET) server.o entity.o vmath.o physics.o utils.o player.o input.o mapParser.o $(LDFLAGS)

server.o: $(SRCDIR)/server.c $(INCDIR)/server.h
	$(CC) $(CFLAGS) $(SRCDIR)/server.c

entity.o: $(SHAREDSRC)/entity.c $(SHAREDINC)/entity.h
	$(CC) $(CFLAGS) $(SHAREDSRC)/entity.c

vmath.o: $(SHAREDSRC)/vmath.c $(SHAREDINC)/vmath.h
	$(CC) $(CFLAGS) $(SHAREDSRC)/vmath.c

physics.o: $(SHAREDSRC)/physics.c $(SHAREDINC)/physics.h
	$(CC) $(CFLAGS) $(SHAREDSRC)/physics.c

utils.o: $(SHAREDSRC)/utils.c $(SHAREDINC)/utils.h
	$(CC) $(CFLAGS) $(SHAREDSRC)/utils.c

player.o: $(SHAREDSRC)/player.c $(SHAREDINC)/player.h
	$(CC) $(CFLAGS) $(SHAREDSRC)/player.c

input.o: $(SHAREDSRC)/input.c $(SHAREDINC)/input.h
	$(CC) $(CFLAGS) $(SHAREDSRC)/input.c

mapParser.o: $(SHAREDSRC)/mapParser.c $(SHAREDINC)/mapParser.h
	$(CC) $(CFLAGS) $(SHAREDSRC)/mapParser.c


clean:
	$(DELCMD) *.o
	$(DELCMD) *.exe