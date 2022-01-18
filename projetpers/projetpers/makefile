CC=gcc
CFLAGS=-Wall -o
SDLFLAGS=-lSDL -lSDLmain  
SOURCES=jdelavie.c 
OBJECTS=$(SOURCES:.c=.c)
EXECUTABLE=jd

all: $(SOURCES)  $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(CFLAGS) $@ $(OBJECTS) $(SDLFLAGS)

.c.o:
	$(CC) $(CFLAGS) $< -o $@ $(SDLFLAGS)
