 #include "../list/list.h"
 #include "error.h"
 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 
 struct Error_handler{
    List_pt list;
 };


struct Error_entry{
    int line;
    char* error;
};

/*-----------------------------------------------------------------------------------------*/
/* the functions set_value_error,delete_value_error,protect_value_error tell the abstract type "list" 
how to handle the type "error_entry" used here, see list/list.h for further details */
void* set_value_error(void* param)
{
    struct Error_entry * err;
    err=(struct Error_entry*)malloc(sizeof(struct Error_entry));
    err->line=((struct Error_entry*)param)->line;
    err->error=(char*)malloc((strlen(((struct Error_entry*)param)->error)+1)*sizeof(char));
    strcpy(err->error,((struct Error_entry*)param)->error);
    return err;
}

int delete_value_error(void* value){
    free(((struct Error_entry*)value)->error);
    free((struct Error_entry*)value);
    return 0;
}

void* protect_value_error(void* param)
{
    struct Error_entry * err;
    err=(struct Error_entry*)malloc(sizeof(struct Error_entry));
    err->line=((struct Error_entry*)param)->line;
    err->error=(char*)malloc((strlen(((struct Error_entry*)param)->error)+1)*sizeof(char));
    strcpy(err->error,((struct Error_entry*)param)->error);
    return err;
}

/*---------------------------------------------------------------------------------------*/

/*used to report an error, the hndler then stores the error and will print it when asked.
the parameter line - refers to the line number and the string error shoul contein details about the error*/  
int report_error(error_handler err_h, int line, char* error)
{
    struct Error_entry err;
    err.line = line;
    err.error=(char*)malloc((strlen(error)+1)*sizeof(char));
    strcpy(err.error,error);
    insert_to_list(err_h->list,&err);
    return 0;
}

/* a constructor for the error handler, all new error handlers must be ci=onstructed by this function*/

error_handler get_error_handler()
{
    error_handler errh=(error_handler)malloc(sizeof(struct Error_handler));
    errh->list=get_new_list(&set_value_error,&delete_value_error,&protect_value_error);
    return errh;
}

/*deletes the error handler and releases its memory*/

int delete_error_handler(error_handler err)
{
    delete_list(err->list);
    free(err);
    return 0;
}

/* prints the errors to the file*/

void print_errors(error_handler err,FILE* out)
{
    struct Error_entry* error;
    reset_list_to_start(err->list);
    while ((error=get_current(err->list))!=NULL)
    {
        fprintf(out,"error on line %d: %s\n",error->line,error->error);
        Next(err->list);
    }
}

/* returns 1 if there are errors, 0 if not*/
char has_errors(error_handler err)
{
    reset_list_to_start(err->list);
    if (get_current(err->list)==NULL)
        return 0;
    return 1;
}

