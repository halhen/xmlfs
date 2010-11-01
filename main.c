/*
 * xmlfs - mount an xml file as a directory structure.
 * Very early version.
 *
 * Copyright (c) 2009 Henrik Hallberg <halhen@k2h.se>
 * Released under GPL 2 - see LICENSE for details.
 * Please report bugs of feature requests by email
 * http://github.com/halhen/xmlfs
 *
 * TODO: 
 * UTF-8 / LOCALE support
 * Writing to file structure / XML
 * XML namespaces
 * XPath?
 * Memory leaks?
 * Read comments and other types
 * #xml containing xml for folder?
 */

#define FUSE_USE_VERSION 26

#include <stdio.h>
#include <fuse.h>
#include <errno.h>
#include <string.h>
#include <math.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "global.h"
#include "xmlhelpers.h"


/* xml_rootparent contains only the actual root node. This node equals the path / */
xmlNode* xml_rootparent;

char* create_formatted_name(xmlElement* el)
{
    int ia, ib, ic;
    char* str = NULL;

    if (!el)
        return NULL;

    if (el->type == XML_ELEMENT_NODE)
    {
        count_twins(el, &ia, &ib);
        ia += INDEX_BASE;
        ic = (int)floor(log10(ia)) + 1;
        
        str = malloc(strlen((char*)el->name) + 2 + ic);
        sprintf(str, "%d.%s", ia, el->name);
    }
    else if (el->type == XML_TEXT_NODE) {
        str = malloc(strlen(CONTENT_FILENAME) + 1);
        strcpy(str, CONTENT_FILENAME);
    } 
    else {
        str = malloc(strlen((char*)el->name) + 1);
        strcpy(str, (char*)el->name);
    }

    return str;
}

void add_to_listing(void* buf, fuse_fill_dir_t filler, xmlElement* el)
{
    char* name = create_formatted_name(el);
    if (name) {
        filler(buf, name, NULL, 0);
        free(name);
    }
}

static int xmlfs_getattr(const char* path, struct stat* stbuf)
{
    char* str = NULL;
    xmlElement* node = findxmlelement(path, xml_rootparent);
    if (!node)
        return -ENOENT;

    memset(stbuf, 0, sizeof(struct stat));

    if (node->type == XML_ELEMENT_NODE) {
        stbuf->st_mode = S_IFDIR | 0555;
        stbuf->st_nlink = 2;
    } 
    else {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;

        str = node_value(node);
        if (str) {
            stbuf->st_size = strlen(str);
            xmlFree(str);
        }
    }

    return 0;
}

static int xmlfs_readdir(const char* path, void* buf, fuse_fill_dir_t filler, 
        off_t UNUSED(offset), struct fuse_file_info* UNUSED(fi))
{
    xmlAttr* attr;
    xmlNode* node = (xmlNode*)findxmlelement(path, xml_rootparent);
    if (!node) 
        return -ENOENT;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);

    for (attr = node->properties; attr; attr = attr->next) {
        add_to_listing(buf, filler, (xmlElement*)attr);
    }

    for (node = node->children ; node; node = node->next) {
        if (node->type == XML_ELEMENT_NODE) {
            add_to_listing(buf, filler, (xmlElement*)node);
        }
        else if (has_content(node->parent)) {
            add_to_listing(buf, filler, (xmlElement*)node);
        }
    }
    
    return 0;
}

static int xmlfs_open(const char *path, struct fuse_file_info *fi)
{
    xmlElement *xel = findxmlelement(path, xml_rootparent);

    if (!xel) 
        return -ENOENT;
    
    if ((fi->flags & 3) != O_RDONLY) 
        return -EACCES;

    return 0;
}

static int xmlfs_read(const char *path, char *buf, size_t size,
        off_t offset, struct fuse_file_info *UNUSED(fi))
{
    size_t len;
    char* content = NULL;
    xmlElement *xel = findxmlelement(path, xml_rootparent);

    if (!xel)
        return -ENOENT;

    content = node_value(xel);

    if (!content) {
        fprintf(stderr, "Error: File %s gives NULL content.", path);
        return -EINVAL; /* Which code is correct? Can this happen? */
    }

    len = strlen(content);
    if (offset < len) {
        if (offset + size > len) {
            size = len - offset;
        }
        memcpy(buf, content + offset, size);
    } else
        size = 0;

    xmlFree(content);
    return size;
}

static struct fuse_operations fileops = {
    .getattr = xmlfs_getattr,
    .readdir = xmlfs_readdir, 
    .open = xmlfs_open,
    .read = xmlfs_read,
};

/* Command line parsing. Should be cleaned up a bit */
struct xmlfs_config {
    char* xmlfile;
};

enum {
    KEY_HELP, 
    KEY_VERSION,
};

static struct fuse_opt xmlfs_opts[] = {
    {"xmlfile=%s", 0, 0},
    FUSE_OPT_KEY("-v", KEY_VERSION),
    FUSE_OPT_KEY("--version", KEY_VERSION),
    FUSE_OPT_KEY("-h", KEY_HELP),
    FUSE_OPT_KEY("--help", KEY_HELP),
    FUSE_OPT_END
};

static int xmlfs_opt_proc(void* UNUSED(data), const char* UNUSED(arg), int key, struct fuse_args *outargs)
{
    switch(key) {
    case KEY_HELP:
        fprintf(stderr, "xmlfs mounts an xml file as a directory structure.\n"
                        "Nodes become directories, with an index in front of the name\n"
                        "Attributes become files, name accordingly and contains the value\n"
                        "Node content text is stored in a file named '#text'\n"
                        "\n"
                        "Currently works in best case - simple XML and ASCII characters.\n"
                        "\n"
                        "usage: %s mountpoint [options]\n"
                        "\n"
                        "general options:\n"
                        "    -o opt,[opt...]  mount options\n"
                        "    -h   --help      print help\n"
                        "    -v   --version   print version\n"
                        "\n"
                        "xmlfs options:\n"
                        "    -o xmlfile=FILENAME    stdin if not specified\n",
                        outargs->argv[0]);
        fuse_opt_add_arg(outargs, "-ho");
        fuse_main(outargs->argc, outargs->argv, &fileops, NULL);
        exit(1);
    case KEY_VERSION:
        fprintf(stderr, "xmlfs version 0.0.2\n");
        fuse_opt_add_arg(outargs, "--version");
        fuse_main(outargs->argc, outargs->argv, &fileops, NULL);
        exit(0);
    }

    return 1;
}

int main(int argc, char* argv[])
{
    int ret = 0;
    struct xmlfs_config conf;
    struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

    xmlDoc* xmldoc = NULL;
    const char* xmlfile = "/dev/stdin";

    memset(&conf, 0, sizeof(conf));
    fuse_opt_parse(&args, &conf, xmlfs_opts, xmlfs_opt_proc);

    if (conf.xmlfile)
        xmlfile = conf.xmlfile;

    xmldoc = xmlReadFile(xmlfile, NULL, 0);
    
    if (xmldoc == NULL) {
        fprintf(stderr, "Could not read XML\n");
        return 1;
    }

    /* FIXME: better way of having root node as top directory
     * This way causes some strange free's at exit according to valgrind */
    xml_rootparent = xmlNewNode(NULL, (xmlChar*)"xmlfs_root");
    if (!xmlAddChild(xml_rootparent, xmlDocGetRootElement(xmldoc))) {
        fprintf(stderr, "Error creating XML node\n");
        return 1;
    }

    ret = fuse_main(args.argc, args.argv, &fileops, NULL);
    
    xmlFreeDoc(xmldoc);
    xmlCleanupParser();
    if (conf.xmlfile)
        free(conf.xmlfile);
    fuse_opt_free_args(&args);

    return ret;
}
