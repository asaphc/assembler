#ifndef ERROR_H
#define ERROR_H
#include <stdio.h>

/* this library is used for storing and handeling errors*/ 

/* an error hndler object*/
typedef struct Error_handler* error_handler;

/*used to report an error, the hndler then stores the error and will print it when asked.
the parameter line - refers to the line number and the string error shoul contein details about the error*/  
int report_error(error_handler err_h, int line, char* error);

/* a constructor for the error handler, all new error handlers must be ci=onstructed by this function*/
error_handler get_error_handler();

/* prints the errors to the file*/
void print_errors(error_handler err,FILE* out);

/* returns 1 if there are errors, 0 if not*/
char has_errors();
/*deletes the error handler and releases its memory*/
int delete_error_handler(error_handler err);

#endif