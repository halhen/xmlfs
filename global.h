/* Useful definitions used throughout xmlfs */

#ifndef __GLOBAL_H_INCLUDED__
#define __GLOBAL_H_INCLUDED__

#define INDEX_BASE 1
#define CONTENT_FILENAME "#text"

/* Macro to make compilers not warn about unused parameters
 * http://sourcefrog.net/weblog/software/languages/C/unused.html */
#ifdef UNUSED
#elif defined(__GNUC__)
# define UNUSED(x) UNUSED_ ## x __attribute__((unused))
#elif defined(__LCLINT__)
# define UNUSED(x) /*@unused@*/ x
#else
# define UNUSED(x) x
#endif

#endif
