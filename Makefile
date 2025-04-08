
SRCDIR=./src
INCDIR=./include
CC=gcc
INCLUDE = -IC:/msys64/mingw64/include/SDL2 -I$(INCDIR) 
CFLAGS = -g $(INCLUDE) -c
LDFLAGS = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -mwindows -lm -mconsole


TARGET = collisionengine




$(TARGET): main.o renderWindow.o entity.o vmath.o
	$(CC) -o $(TARGET) main.o renderWindow.o entity.o vmath.o $(LDFLAGS)

main.o: $(SRCDIR)/main.c $(INCDIR)/renderWindow.h $(INCDIR)/entity.h $(INCDIR)/vmath.h
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

renderWindow.o: $(SRCDIR)/renderWindow.c $(INCDIR)/renderWindow.h $(INCDIR)/entity.h
	$(CC) $(CFLAGS) $(SRCDIR)/renderWindow.c

entity.o: $(SRCDIR)/entity.c $(INCDIR)/entity.h $(INCDIR)/vmath.h
	$(CC) $(CFLAGS) $(SRCDIR)/entity.c

vmath.o: $(SRCDIR)/vmath.c $(INCDIR)/vmath.h
	$(CC) $(CFLAGS) $(SRCDIR)/vmath.c


clean:
	del *.o
	del *.exe