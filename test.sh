#!/bin/sh


# Mount filesystem
mkdir -p mnt/
./xmlfs mnt/ -o xmlfile=test.xml

# Do basic tests
cd mnt/
if [ `ls | wc -l` -ne 1 ]; then
    echo "Wrong number of elements in root directory";
fi

if [ `find 1.root/ -name '*.node' | wc -l` -ne 2 ]; then
    echo "Too many %d.nodes in 1.root";
fi

if [ `find 1.root/1.node/ -name 'attribute' | wc -l` -ne 1 ]; then
    echo "attribute error in 1.root/1.node/";
fi
if [ `find 1.root/1.node/ -name 'attribute2' | wc -l` -ne 0 ]; then
    echo "attribute2 should not be in 1.root/1.node";
fi
if [ `cat 1.root/1.node/attribute` != "attrib_value" ]; then
    echo "1.root/1.node/attribute does not contain 'attrib_value'";
fi

if [ `find 1.root/2.node/ -name 'attribute' | wc -l` -ne 0 ]; then
    echo "attribute should not be in 1.root/1.node/";
fi
if [ `find 1.root/2.node/ -name 'attribute2' | wc -l` -ne 1 ]; then
    echo "attribute2 error in 1.root/1.node";
fi

if [ "`cat 1.root/1.textcontent/#text`" != "some text" ]; then
    echo "Failed to read 1.root/1.textcontent/#text properly";
fi

cd ..
fusermount -u mnt
echo "Done"
