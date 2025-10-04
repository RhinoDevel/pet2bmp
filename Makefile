# MT, 2017dec03
#
# - Original source: http://mrbook.org/blog/tutorials/make/

CC=gcc

#CFLAGS=-c -O3 -std=gnu11 -Wall -DNDEBUG
CFLAGS=-c -g -std=gnu11 -Wall
LDFLAGS=
SOURCES=main.c Sys.c Bmp.c Dim.c FileSys.c

OBJECTS=$(SOURCES:.c=.o)

# Hard-coded (also see main.c):
EXECUTABLE=pet2bmp

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
