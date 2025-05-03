SRCDIR = ./src
INCDIR = ./include
LIBSRC = ../lib/src
LIBINC = ../lib/include
CC = gcc
ifeq ($(OS),Windows_NT)
	INCLUDE = -IC:/msys64/mingw64/include/SDL2 -IC:/msys64/mingw64/include/libxml2 -I$(INCDIR) -I$(LIBINC)
	LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -mwindows -lm -mconsole -lxml2 -ljansson
	DELCMD = del
else
	INCLUDE = -I/usr/include/libxml2 -I$(INCDIR) -I$(LIBINC)
	LDFLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lm -lxml2 -ljansson
	DELCMD = rm
endif

CFLAGS = -g $(INCLUDE) -c
TARGET = Hopless.exe


$(TARGET): main.o renderWindow.o entity.o vmath.o physics.o camera.o utils.o player.o input.o client.o menu.o tongue.o networkData.o mapParser.o
	$(CC) -o $(TARGET) main.o renderWindow.o entity.o vmath.o physics.o camera.o utils.o player.o input.o client.o menu.o tongue.o networkData.o mapParser.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c $(INCDIR)/main.h
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

renderWindow.o: $(SRCDIR)/renderWindow.c $(INCDIR)/renderWindow.h 
	$(CC) $(CFLAGS) $(SRCDIR)/renderWindow.c

camera.o: $(SRCDIR)/camera.c $(INCDIR)/camera.h
	$(CC) $(CFLAGS) $(SRCDIR)/camera.c

client.o: $(SRCDIR)/client.c $(INCDIR)/client.h
	$(CC) $(CFLAGS) $(SRCDIR)/client.c

menu.o: $(SRCDIR)/menu.c $(INCDIR)/menu.h
	$(CC) $(CFLAGS) $(SRCDIR)/menu.c


entity.o: $(LIBSRC)/entity.c $(LIBINC)/entity.h
	$(CC) $(CFLAGS) $(LIBSRC)/entity.c

vmath.o: $(LIBSRC)/vmath.c $(LIBINC)/vmath.h
	$(CC) $(CFLAGS) $(LIBSRC)/vmath.c

physics.o: $(LIBSRC)/physics.c $(LIBINC)/physics.h
	$(CC) $(CFLAGS) $(LIBSRC)/physics.c

utils.o: $(LIBSRC)/utils.c $(LIBINC)/utils.h
	$(CC) $(CFLAGS) $(LIBSRC)/utils.c

player.o: $(LIBSRC)/player.c $(LIBINC)/player.h
	$(CC) $(CFLAGS) $(LIBSRC)/player.c

input.o: $(LIBSRC)/input.c $(LIBINC)/input.h
	$(CC) $(CFLAGS) $(LIBSRC)/input.c

tongue.o: $(LIBSRC)/tongue.c $(LIBINC)/tongue.h
	$(CC) $(CFLAGS) $(LIBSRC)/tongue.c

networkData.o: $(LIBSRC)/networkData.c $(LIBINC)/networkData.h
	$(CC) $(CFLAGS) $(LIBSRC)/networkData.c

mapParser.o: $(LIBSRC)/mapParser.c $(LIBINC)/mapParser.h
	$(CC) $(CFLAGS) $(LIBSRC)/mapParser.c


clean:
	$(DELCMD) *.o
	$(DELCMD) *.exe