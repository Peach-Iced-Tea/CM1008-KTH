SRCDIR=./src
INCDIR=./include
CC=gcc
ifeq ($(OS),Windows_NT)
	INCLUDE = -IC:/msys64/mingw64/include/SDL2 -I$(INCDIR) 
	LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -mwindows -lm -mconsole
	DELCMD = del
else
	INCLUDE = -I$(INCDIR)
	LDFLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_net -lm
	DELCMD = rm
endif

CFLAGS = -g $(INCLUDE) -c
TARGET = ToTheTop.exe


$(TARGET): main.o renderWindow.o entity.o vmath.o physics.o camera.o utils.o player.o input.o client.o menu.o
	$(CC) -o $(TARGET) main.o renderWindow.o entity.o vmath.o physics.o camera.o utils.o player.o input.o client.o menu.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c $(INCDIR)/main.h
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

renderWindow.o: $(SRCDIR)/renderWindow.c $(INCDIR)/renderWindow.h
	$(CC) $(CFLAGS) $(SRCDIR)/renderWindow.c

entity.o: $(SRCDIR)/entity.c $(INCDIR)/entity.h
	$(CC) $(CFLAGS) $(SRCDIR)/entity.c

vmath.o: $(SRCDIR)/vmath.c $(INCDIR)/vmath.h
	$(CC) $(CFLAGS) $(SRCDIR)/vmath.c

physics.o: $(SRCDIR)/physics.c $(INCDIR)/physics.h
	$(CC) $(CFLAGS) $(SRCDIR)/physics.c

camera.o: $(SRCDIR)/camera.c $(INCDIR)/camera.h
	$(CC) $(CFLAGS) $(SRCDIR)/camera.c

utils.o: $(SRCDIR)/utils.c $(INCDIR)/utils.h
	$(CC) $(CFLAGS) $(SRCDIR)/utils.c

player.o: $(SRCDIR)/player.c $(INCDIR)/player.h
	$(CC) $(CFLAGS) $(SRCDIR)/player.c

input.o: $(SRCDIR)/input.c $(INCDIR)/input.h
	$(CC) $(CFLAGS) $(SRCDIR)/input.c

client.o: $(SRCDIR)/client.c $(INCDIR)/client.h
	$(CC) $(CFLAGS) $(SRCDIR)/client.c

menu.o: $(SRCDIR)/menu.c $(INCDIR)/menu.h
	$(CC) $(CFLAGS) $(SRCDIR)/menu.c


clean:
	$(DELCMD) *.o
	$(DELCMD) *.exe