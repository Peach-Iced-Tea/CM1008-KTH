CLIENTSRC = ./client/src
CLIENTINC = ./client/include

SERVERSRC = ./server/src
SERVERINC = ./server/include

LIBSRC = ./lib/src
LIBINC = ./lib/include

OBJDIR = ./obj
CC = gcc
ifeq ($(OS),Windows_NT)
	INCLUDE = -IC:/msys64/mingw64/include/SDL2 -IC:/msys64/mingw64/include/libxml2 -I$(CLIENTINC) -I$(SERVERINC) -I$(LIBINC)
	LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -mwindows -lm -mconsole -lxml2 -ljansson
	DELCMD = del
	DELOBJ = ".\obj\"
else
	INCLUDE = -I/usr/include/libxml2 -I$(CLIENTINC) -I$(SERVERINC) -I$(LIBINC)
	LDFLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lm -lxml2 -ljansson
	DELCMD = rm
	DELOBJ = ./obj/
endif

CFLAGS = -g $(INCLUDE) -c
TARGETCLIENT = hopless
TARGETSERVER = server

CLIENTOBJECTS = $(patsubst $(CLIENTSRC)/%.c, $(OBJDIR)/%.o, $(wildcard $(CLIENTSRC)/*.c))
SERVEROBJECTS = $(patsubst $(SERVERSRC)/%.c, $(OBJDIR)/%.o, $(wildcard $(SERVERSRC)/*.c))
LIBOBJECTS = $(patsubst $(LIBSRC)/%.c, $(OBJDIR)/%.o, $(wildcard $(LIBSRC)/*.c))


# Client source files
$(TARGETCLIENT): $(CLIENTOBJECTS) $(LIBOBJECTS)
	$(CC) -o $(TARGETCLIENT).exe $(CLIENTOBJECTS) $(LIBOBJECTS) $(LDFLAGS)

$(OBJDIR)/main.o: $(CLIENTSRC)/main.c $(CLIENTINC)/main.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/renderWindow.o: $(CLIENTSRC)/renderWindow.c $(CLIENTINC)/renderWindow.h 
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/camera.o: $(CLIENTSRC)/camera.c $(CLIENTINC)/camera.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/client.o: $(CLIENTSRC)/client.c $(CLIENTINC)/client.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/menu.o: $(CLIENTSRC)/menu.c $(CLIENTINC)/menu.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/mouse.o: $(CLIENTSRC)/mouse.c $(CLIENTINC)/mouse.h
	$(CC) $(CFLAGS) -o $@ $<


# Server source files
$(TARGETSERVER): $(SERVEROBJECTS) $(LIBOBJECTS)
	$(CC) -o $(TARGETSERVER).exe $(SERVEROBJECTS) $(LIBOBJECTS) $(LDFLAGS)

$(OBJDIR)/server.o: $(SERVERSRC)/server.c $(SERVERINC)/server.h
	$(CC) $(CFLAGS) -o $@ $<


# Library source files
$(OBJDIR)/entity.o: $(LIBSRC)/entity.c $(LIBINC)/entity.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/vmath.o: $(LIBSRC)/vmath.c $(LIBINC)/vmath.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/utils.o: $(LIBSRC)/utils.c $(LIBINC)/utils.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/player.o: $(LIBSRC)/player.c $(LIBINC)/player.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/input.o: $(LIBSRC)/input.c $(LIBINC)/input.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/tongue.o: $(LIBSRC)/tongue.c $(LIBINC)/tongue.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/networkData.o: $(LIBSRC)/networkData.c $(LIBINC)/networkData.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/mapParser.o: $(LIBSRC)/mapParser.c $(LIBINC)/mapParser.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/obstacle.o: $(LIBSRC)/obstacle.c $(LIBINC)/obstacle.h
	$(CC) $(CFLAGS) -o $@ $<

$(OBJDIR)/animation.o: $(LIBSRC)/animation.c $(LIBINC)/animation.h
	$(CC) $(CFLAGS) -o $@ $<

all: $(TARGETCLIENT) $(TARGETSERVER)


clean:
	$(DELCMD) $(DELOBJ)*.o
	$(DELCMD) *.exe