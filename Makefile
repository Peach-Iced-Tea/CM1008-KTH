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
else
	INCLUDE = -I/usr/include/libxml2 -I$(CLIENTINC) -I$(SERVERINC) -I$(LIBINC)
	LDFLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lm -lxml2 -ljansson
	DELCMD = rm
endif

CFLAGS = -g $(INCLUDE) -c
TARGETCLIENT = hopless
TARGETSERVER = server

CLIENTOBJECTS = $(OBJDIR)/main.o $(OBJDIR)/renderWindow.o $(OBJDIR)/camera.o $(OBJDIR)/client.o $(OBJDIR)/menu.o
SERVEROBJECTS = $(OBJDIR)/server.o
LIBOBJECTS = $(OBJDIR)/entity.o $(OBJDIR)/vmath.o $(OBJDIR)/physics.o $(OBJDIR)/utils.o $(OBJDIR)/input.o $(OBJDIR)/player.o $(OBJDIR)/tongue.o $(OBJDIR)/networkData.o $(OBJDIR)/mapParser.o

$(TARGETCLIENT): $(CLIENTOBJECTS) $(LIBOBJECTS)
	$(CC) -o $(TARGETCLIENT).exe $(CLIENTOBJECTS) $(LIBOBJECTS) $(LDFLAGS)

$(OBJDIR)/main.o: $(CLIENTSRC)/main.c $(CLIENTINC)/main.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/main.o $(CLIENTSRC)/main.c

$(OBJDIR)/renderWindow.o: $(CLIENTSRC)/renderWindow.c $(CLIENTINC)/renderWindow.h 
	$(CC) $(CFLAGS) -o $(OBJDIR)/renderWindow.o $(CLIENTSRC)/renderWindow.c

$(OBJDIR)/camera.o: $(CLIENTSRC)/camera.c $(CLIENTINC)/camera.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/camera.o $(CLIENTSRC)/camera.c

$(OBJDIR)/client.o: $(CLIENTSRC)/client.c $(CLIENTINC)/client.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/client.o $(CLIENTSRC)/client.c

$(OBJDIR)/menu.o: $(CLIENTSRC)/menu.c $(CLIENTINC)/menu.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/menu.o $(CLIENTSRC)/menu.c


$(TARGETSERVER): $(SERVEROBJECTS) $(LIBOBJECTS)
	$(CC) -o $(TARGETSERVER).exe $(SERVEROBJECTS) $(LIBOBJECTS) $(LDFLAGS)

$(OBJDIR)/server.o: $(SERVERSRC)/server.c $(SERVERINC)/server.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/server.o $(SERVERSRC)/server.c


$(OBJDIR)/entity.o: $(LIBSRC)/entity.c $(LIBINC)/entity.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/entity.o $(LIBSRC)/entity.c

$(OBJDIR)/vmath.o: $(LIBSRC)/vmath.c $(LIBINC)/vmath.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/vmath.o $(LIBSRC)/vmath.c

$(OBJDIR)/physics.o: $(LIBSRC)/physics.c $(LIBINC)/physics.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/physics.o $(LIBSRC)/physics.c

$(OBJDIR)/utils.o: $(LIBSRC)/utils.c $(LIBINC)/utils.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/utils.o $(LIBSRC)/utils.c

$(OBJDIR)/player.o: $(LIBSRC)/player.c $(LIBINC)/player.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/player.o $(LIBSRC)/player.c

$(OBJDIR)/input.o: $(LIBSRC)/input.c $(LIBINC)/input.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/input.o $(LIBSRC)/input.c

$(OBJDIR)/tongue.o: $(LIBSRC)/tongue.c $(LIBINC)/tongue.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/tongue.o $(LIBSRC)/tongue.c

$(OBJDIR)/networkData.o: $(LIBSRC)/networkData.c $(LIBINC)/networkData.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/networkData.o $(LIBSRC)/networkData.c

$(OBJDIR)/mapParser.o: $(LIBSRC)/mapParser.c $(LIBINC)/mapParser.h
	$(CC) $(CFLAGS) -o $(OBJDIR)/mapParser.o $(LIBSRC)/mapParser.c


clean:
	$(DELCMD) *.o
	$(DELCMD) *.exe