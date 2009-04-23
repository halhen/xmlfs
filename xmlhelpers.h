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

#ifndef __XMLHELPERS_H_INCLUDED__
#define __XMLHELPERS_H_INCLUDED__

#include <libxml/tree.h>

char* node_value(xmlElement* xel);
void count_twins(xmlElement* el, int *twins_before, int *twins_total);
xmlElement* findxmlelement(const char *path, xmlNode *root);
int has_content(xmlNode* node);

#endif 
