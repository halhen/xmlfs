all:
	gcc `pkg-config --cflags --libs fuse` `xml2-config --cflags --libs` main.c xmlhelpers.c -o xmlfs
