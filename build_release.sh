if [ $# -ne 1 ]; then
    echo "Usage: build_release.sh <version number>"
    exit 1
fi

APPNAME="xmlfs"
RELEASENAME=$APPNAME-$1

mkdir -p $RELEASENAME

cat LICENSE | sed s/CURVERSION/$1/g > $RELEASENAME/LICENSE
cat README | sed s/CURVERSION/$1/g > $RELEASENAME/README
cat config.mk | sed s/CURVERSION/$1/g > $RELEASENAME/config.mk
cat Makefile | sed s/CURVERSION/$1/g > $RELEASENAME/Makefile
cat main.c | sed s/CURVERSION/$1/g > $RELEASENAME/main.c
cat xmlhelpers.h | sed s/CURVERSION/$1/g > $RELEASENAME/xmlhelpers.h
cat xmlhelpers.c | sed s/CURVERSION/$1/g > $RELEASENAME/xmlhelpers.c
cat global.h | sed s/CURVERSION/$1/g > $RELEASENAME/global.h

tar cvzf releases/${RELEASENAME}.tar.gz $RELEASENAME

find . -name 'build_pkg.sh' -exec {} $1 \;

rm -rf $RELEASENAME

echo "Now upload release to web and git-hub"
