include config.mk

CC = gcc
CFLAGS = -O2 -Wall -W
OBJECTS = main.o xmlhelpers.o

all: $(OBJECTS)
	$(CC) `pkg-config --libs fuse` `xml2-config --libs` $(OBJECTS) -o xmlfs -g

%.o: %.c
	$(CC) $(CFLAGS) `pkg-config --cflags fuse` `xml2-config --cflags` -c $<

clean:
	rm xmlfs $(OBJECTS)

install: all
	@echo installing executable to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f xmlfs ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/xmlfs

uninstall:
	@echo removing executable from ${DESTDIR}${PREFIX}/bin
	@rm ${DESTDIR}${PREFIX}/bin/xmlfs
