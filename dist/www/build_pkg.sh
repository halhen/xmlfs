#!/bin/sh
#
# Generate package for aur.archlinux.org
# Called by ../../build_release.sh and depends on its actions
# Will also run in it's directory
# $1 is version number

APPNAME="xmlfs"
RELEASENAME=$APPNAME-$1

echo "Generating www for $APPNAME"

mkdir -p releases/www/dl/
cp releases/${RELEASENAME}.tar.gz releases/www/dl/

(cat dist/www/_header.html | sed s/PAGETITLE/$APPNAME/; cat documentation.md | sed s/CURVERSION/$1/g | smu; cat dist/www/_footer.html) > releases/www/$APPNAME.html
cp dist/www/styles.css releases/www/
