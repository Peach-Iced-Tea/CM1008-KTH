SRCDIR=./src
INCDIR=./include
CC=gcc
ifeq ($(OS),Windows_NT)
	INCLUDE = -IC:/msys64/mingw64/include/SDL2 -I$(INCDIR) 
	LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -mwindows -lm -mconsole
	DELCMD = del
else
	INCLUDE = -I$(INCDIR)
	LDFLAGS = -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lm
	DELCMD = rm
endif

CFLAGS = -g $(INCLUDE) -c
TARGET = ToTheTop.exe




$(TARGET): main.o renderWindow.o entity.o vmath.o physics.o
	$(CC) -o $(TARGET) main.o renderWindow.o entity.o vmath.o physics.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c $(INCDIR)/renderWindow.h $(INCDIR)/entity.h $(INCDIR)/vmath.h $(INCDIR)/physics.h
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

renderWindow.o: $(SRCDIR)/renderWindow.c $(INCDIR)/renderWindow.h $(INCDIR)/entity.h
	$(CC) $(CFLAGS) $(SRCDIR)/renderWindow.c

entity.o: $(SRCDIR)/entity.c $(INCDIR)/entity.h $(INCDIR)/vmath.h $(INCDIR)/physics.h
	$(CC) $(CFLAGS) $(SRCDIR)/entity.c

vmath.o: $(SRCDIR)/vmath.c $(INCDIR)/vmath.h
	$(CC) $(CFLAGS) $(SRCDIR)/vmath.c

physics.o: $(SRCDIR)/physics.c $(INCDIR)/physics.h
	$(CC) $(CFLAGS) $(SRCDIR)/physics.c


clean:
	$(DELCMD) *.o
	$(DELCMD) *.exe