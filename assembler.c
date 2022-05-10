#include <stdio.h>
#include <string.h>
#include "compiler/compiler.h"

/* main program: gets an assembly file from the command line (just the name of the file with out 
the .as extention) and compiles the file, if there are errrors, they are printed*/

int main (int argc, char *argv[])
{
    int i=1;
    FILE* file;
    char s[100];
    if (argc==1)
    {
        printf("\nfile for assembler must be given in command line");
        return 0;
    }
    else {
        for (i=1;i<argc;i++)
        {
	  strcpy(s,argv[i]);
	   if (strstr(s, ".as")!=NULL)
                printf("\nfile name must be given without extention");
	  else 
	  if ((file=fopen(strcat(s,".as"),"r"))==NULL)
                printf("\ncannot open file: %s", argv[i]);
            else
            {
                printf("compiling file %s\n", s);
                compile(file,stdout,s);
		 } 
            
        }

    }

    return 0;
}
