#ifndef LABEL_HANDLER_H
#define LABEL_HANDLER_H

#include "../memory/word.h"
#include "../error/error.h"
#include <stdio.h>

/* this librey handels labels, entrys and extern labels*/

typedef struct Label_handler* label_handler;

/*this is a label handler constructor, a new label handler must be created using this function*/
label_handler get_label_handler();

/*this function cheks if a label is valid
it returns 1 for yes and 0 for no, if no an description of the error is put in serror*/

short is_valid_label(label_handler label_h,char *label,char* serror);

/*this function returns the code of a label via *code,
it returns 0 if no label is found and 1 if a the label is found
it will give a 0 if a label is extern, for extern labels use is_extern*/
char get_label_code(label_handler label_h,char* label, word* code);

/* this finction adds a label to the list of extern labels used for the extern file, the location of 
the labeel is past with "value"*/
char add_to_extern_list(label_handler label_h,char* label,int value);

/*this function checks if a label was defined as extern, return 1 if yes and 0 if no*/
char is_extern(label_handler label_h,char* label);

/* this function gets a line of code starting with the .extern command, it adds the label to the 
dictionary of extern labels, if there is an error it will return 0 with the details stored in *serror*/
char add_extern(label_handler label_h, char* line,char* serror);

/* this function gets a line of code starting with the .entry command, it adds the label to the 
list of entry labels, if there is an error it will return 0 with the details stored in *serror*/
char add_entry(label_handler label_h,char* line,char*  serror);

/*this function adds a label to the dictionary of defind labels,
the name is in *label, its location in the code is in "count" and its type is given is *type.
valid types are "code" and "data"*/
int add_label(label_handler label_h,char* label,int count,char *type);

/*this function combines the information from the defined labels list with the list of entrys
and produced the list of entrys used for the entry file,
it will report errors to the error handler and will return 1 if no errors acure and 0 othewise*/
char update_entrys(label_handler label_h,int ICF, error_handler err);

/*this function prints a list of entrys with there location to a file*/
char print_entrys(label_handler label_h,FILE* file);

/*this function prints a list of uses of extern labels with there location to a file*/
char print_externs(label_handler label_h,FILE* file);

/*this function deletes the label handler and relaeses its memory*/
int delete_label_handler(label_handler label_h);


#endif
