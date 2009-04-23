CC = gcc
CFLAGS = -O2 -Wall -W
OBJECTS = main.o xmlhelpers.o

all : $(OBJECTS)
	$(CC) `pkg-config --libs fuse` `xml2-config --libs` $(OBJECTS) -o xmlfs -g

%.o : %.c
	$(CC) $(CFLAGS) `pkg-config --cflags fuse` `xml2-config --cflags` -c $<

clean:
	rm xmlfs $(OBJECTS)
