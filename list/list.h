#ifndef LIST_H
#define LIST_H

/* this library defines a linked list. as this is an abstract data type the user must provide
information on how to deal with the data stored by defining the set_value,delete_value and protect_value
functions*/

/*NOTE: the list handels the memory, including the protected copy of the data (if used).
  attempts to free the memory given from the list may result in errors*/

typedef struct List* List_pt;

/*this function must be defind by the user of this data type, it recives some parameter about the object
which beeing inserted in to the list,
it must allocate memory for the object to be stored in the list and define its values based on
the parameter.
for example, if the values stored in the list are integers a functin might look like this:
void* set_value_int(void* param) {int *p=(int*)malloc(sizeof(int)); *p=*((int*)param); return p;}
*/
typedef void* (*set_value_list)(void* param);

/*this function must be defind by the user of this data type, it recives an entry object
beeing deleted from the list,
it must free the memory allocted in the set_value function.
for example, continuing the integer example:
void* delete_value_int(void* value) {free(value) return 0;}
*/
typedef int (*delete_value_list)(void* value);


/*this function is optional, if defind by the user of this data type, when asked for an object 
stored in the list this function will be called, it will recive the object asked for as
a parmeter and its return value will be given to the user istead of the original object.
it can be used to protect the user from changing the data inside the list.
note that if it is used, a copy of the data is kept by the list and the list will free the memory,
using the delete function.
the user must not try to free the memory himself.
for example, continuing the integer example:
void* protect_value_int(void* value) {int *p=(int*)malloc(sizeof(int)); *p=*((int*)param); return p;}
*/
typedef void* (*protect_value_list)(void* value);

/* this is a constructer for the list, a new list must be created using this function,
its inputs are the set_value, delete_value and protect_value functions. if the user does not wish
to protect the value he mey put in NULL istead*/
List_pt get_new_list(set_value_list sv,delete_value_list dv,protect_value_list protect);


/* inserts a value into the list, recives a list pointer and a parameter which will be
used to create the object using the set_value function. return a negetive number if there is an error*/
int insert_to_list(List_pt list, void* value_params);

/*return the current object the list is pointing at (via protect_value), or NULL*/
void* get_current(List_pt list); 

/*moves the current_value to the next on the list*/
int Next(List_pt list);

/*moves the current value to the previous object*/
int previous(List_pt list);

/*deletes the list, freeing the objects with delete_value*/
int delete_list(List_pt list);

/*resets the current object to the start of the list*/ 
int reset_list_to_start(List_pt list);

/*note: remove_from_list was not needed for the program so was not implemented*/

#endif
