#ifndef CMD_HANDLER_H
#define CMD_HANDLER_H
#include "../memory/word.h"
#include <stdio.h>

/* this librey handles the commands of the assembly languge. its main uses are to:
1. translate a string into a code, get the code for the command used from the command table, analize the arguments
2. update labels in the second run of the assembler
*/

/* this is the command handler, it holds information such as the command table and the list of commands already read by the assembler*/
typedef struct cmd_handler* cmd_handler_pt;

/* intilizes and returns a new command hanler, all new command hndlers must be created throw this function*/
cmd_handler_pt get_new_cmd_handler();

/* analizes and adds command to list, inputs are:
command handler,
line of code - the command must be the first word on the line (no label)
an empty string for writing errors to, at least 30 chars long
the integer pointer ic is used to return the position of the command of the command (i.e. instruction counter)
the function returns 0 if an error is found and 1 if there is no error
*/
char add_command(cmd_handler_pt cmd_h, char* line, char* errors,int* ic);

/* 
this function is used in the second round of the assembler to update the values of the labels,
the assembler asks the command handler for the next label it needs to know the code for by calling this function.
the function returns the next label, or NULL if there is no more labels
*/
char* request_label_info(cmd_handler_pt cmd_h, int* ic);

/* this function is called as a response to the previous one, after reciving the request for the meaning of the label
the assembler sends the information contening the code the label must be replaced by
the arguments for the function are the command handler, the label (for error checking) and the code
*/
char send_label_info(cmd_handler_pt cmd_h, char* label, word code, char type);

/* this function must be called before replacing labels, it makes sure we start at the begining of the code*/
void reset_cmd_list(cmd_handler_pt cmd_h);

/* this function returns the amount words (memory) in the code*/
int get_size(cmd_handler_pt cmd_h);

/* this function return the IC (instruction counter) of the command holding the label currently looked at
this is used to update the extern labels
*/
int get_IC(cmd_handler_pt cmd_h);

/* this function prints the code into the given file*/
int print_cmd(cmd_handler_pt cmd_h,FILE* file);

/* this function constructs a command handler, new command handlers must be created by calling this function*/
cmd_handler_pt get_new_cmd_handler();

/* this function deletes a command hanler and frees its memory */
int delete_cmd_handler(cmd_handler_pt cmd_h);

/* when updating labels used to skip a label - in case of extern labels*/
int skip_label(cmd_handler_pt cmd_h,char* label);
#endif
