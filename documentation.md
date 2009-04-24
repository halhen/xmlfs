# xmlfs
xmlfs is an userspace application that lets you represent an XML file as a directory structure for easy access. In this early version, xmlfs only supports read operations. This may be extended in the future.

* [latest release](http://www.k2h.se/code/dl/xmlfs-CURVERSION.tar.gz)
* [source repository](http://github.com/halhen/xmlfs/tree/master)

Distribution specific links:

* [archlinux AUR package](http://aur.archlinux.org/packages.php?ID=25905)

## Representation
* Nodes are represented as directories.
* Node attributes are represented as files, named as the attribute and contiaining the value.
* Node content is available in the `#text` file.

An example XML file and the corresponding directory structure is shown below.

    <root rattr="1">
        <node attribute="attrib_value" />
        <node attribute2="second node" />
        <other attribute="attrib_value" />
        <textcontent>some text</textcontent>
    </root>

Example operations:

    $ tree
    .
    `-- 1.root
        |-- 1.node
        |   `-- attribute
        |-- 1.other
        |   `-- attribute
        |-- 1.textcontent
        |   `-- #text
        |-- 2.node
        |   `-- attribute2
        `-- rattr
    
    $ cat 1.root/2.node/attribute2 
    second node
    
    $ cat 1.root/1.textcontent/#text 
    some text

As you can see in the example, node names are prefix with an index. This is nescessary as an XML node can have many children with the same name. 

## Installation
xmlfs depends on [FUSE 2.6+](http://fuse.sourceforge.net/) and [libxml2](http://www.xmlsoft.org/). Please install this before building xmlfs.

Edit `config.mk` to match your setup. Next run

    $ make

Finally, as root:

    # make install

## Usage
    $ mkdir mount_dir/
    $ xmlfs -o xmlfile=file.xml mount_dir/

Unmount with
    $ fusermount -u mount_dir

Fore more information, please run `xmlfs --help` or have a look at the [FUSE wiki](http://apps.sourceforge.net/mediawiki/fuse/index.php?title=Main_Page).

## Limitations
In this early version, xmlfs is a read-only filesystem. Also, it only reads basic XML and usage with non-ASCII characters has not been tested.

## License
Copyright (c) 2009 Henrik Hallberg <halhen@k2h.se>. Released under GPL 2, see LICENSE for details. Please report bugs or feature requests by email or at github.

* <http://code.k2h.se>
* <http://github.com/halhen/xmlfs/tree/master>
