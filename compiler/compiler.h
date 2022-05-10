#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>

/* this is the main function of the prgram,
it recives a file, and produces the object, entry and extern files, with the name *name
if there are errors the are written to the "error" file (or stream).
*/
int compile(FILE *file,FILE* error,char* name);

#endif
