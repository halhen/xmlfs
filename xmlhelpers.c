/*
 * Part of xmlfs
 * Released under GPL 2 - see LICENSE for details.
 * Very early version.
 *
 * Copyright (c) 2009 Henrik Hallberg <halhen@k2h.se>
 * Please report bugs of feature requests by email
 *
 * http://code.k2h.se
 */

#include "xmlhelpers.h"

xmlElement* _findxmlelement(const char* path, xmlNode* parent)
/* Recursively search parent for path. First try nodes,
 * then try attributes */
{
    size_t len;
    xmlNode *node;
    xmlAttr *attr;

    int counter;
    int index = INDEX_BASE;

    if (!path || !parent) 
        return (xmlElement*)parent;

    if (*path == '/')
        path++;
    if (*path == '\0')
        return (xmlElement*)parent;

    if (sscanf(path, "%d.", &index) == 1) {
        /* An indexed object. Set path to actual name */
        path = strstr(path, ".") + 1;
    }

    /* Search nodes - which are indexed */
    for (node = parent->children, counter = INDEX_BASE; node; node = node->next) {
        len = strlen(node->name);
        if (strncmp(node->name, path, len) == 0 &&
                (path[len] == '/' || path[len] == '\0')) {
            if (counter == index)
                break;
            counter += 1;
        }
    }

    if (node)
        return _findxmlelement(strstr(path, "/"), node);

    /* No node found, try attributes - which aren't indexed */
    for (attr = parent->properties; attr; attr = attr->next) {
        if (strcmp(attr->name, path) == 0)
            return (xmlElement*)attr;
    }

    /* No attribute either. How about content? */
    if (strncmp(CONTENT_FILENAME, path, strlen(CONTENT_FILENAME)) == 0) {
        if (has_content(parent)) {
            return (xmlElement*)parent->children;
        }
    }

    return NULL;
}

xmlElement* findxmlelement(const char* path, xmlNode* root)
{
    return _findxmlelement(path, root);
}

char* node_value(xmlElement* xel) 
{
    if (!xel)
        return NULL;

    if (xel->type == XML_ATTRIBUTE_NODE && xel->parent)
        return xmlGetProp((xmlNode*)xel->parent, xel->name);

    if (xel->type == XML_TEXT_NODE && xel->parent)
        return xmlNodeGetContent((xmlNode*)xel->parent);

    return NULL;
}


void count_twins(xmlElement* el, int *twins_before, int *twins_total) 
/* Count the number of equally named, equally typed elements.
 * twinsBefore is filled with number of twins before in list (0 for first)
 * twinsTotal is total number of twins, including @el) */
{
    int before = 0;
    int total = 0;
    xmlElement* tmp;

    if (!el || (!twins_before && !twins_total))
        return;

    for (tmp = (xmlElement*)el->prev; tmp; tmp = (xmlElement*)tmp->prev) {
        if (el->type == tmp->type && strcmp(el->name, tmp->name) == 0)
            before += 1;
    }
    total = before + 1;

    for (tmp = (xmlElement*)el->next; tmp; tmp = (xmlElement*)tmp->next) {
        if (el->type == tmp->type && strcmp(el->name, tmp->name) == 0)
            total += 1;
    }

    if (twins_before)
        *twins_before = before;
    if(twins_total)
        *twins_total = total;
}

int has_content(xmlNode* node)
/* Returns 1 if the node has text content
 * Rules are: children must be lonely child
 * of right type. libmlx adds text nodes
 * to parents that don't own them.
 */
{
    xmlNode* child = NULL;

    if (!node)
       return 0;

    child = node->children;
    if (child && !(child->next) && (child->type == XML_TEXT_NODE))
        return 1;

    return 0;
}
