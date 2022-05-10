
#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdio.h>

/* this libery holds the abstract data type Dictionary which maps a string (key) to a value
which can be any type of data defind by the user. the implementation is done by a red-black tree,
see RB_Tree.h for further details*/

/*NOTE: the dictionary handels the memory, including the protected copy of the data (if used).
  attempts to free the memory given from the list may result in errors*/

/* this is the dictionary object */
typedef struct Dictionary* Dictionary_pt;

/*this function must be defind by the user of this data type, it recives some parameter about the object
which beeing inserted in to the dictionery,
it must allocate memory for the object to be stored in the dictionrey and define its values based on
the parameter.
for example, if the values stored in the dictionary are integers a functin mught look like this:
void* set_value_int(void* param) {int *p=(int*)malloc(sizeof(int)); *p=*((int*)param); return p;}
*/
typedef void* (*set_value)(void* param);

/*this function must be defind by the user of this data type, it recives an entry object
beeing deleted from the dictionery,
it must free the memory allocted in the set_value function.
for example, continuing the integer example:
void* delete_value_int(void* value) {free(value) return 0;}
*/
typedef int (*delete_value)(void* value);

/*this function is optional, if defind by the user of this data type, when asked for an object 
stored in the dictionary this function will be called, it will recive the object asked for as
a parmeter and its return value will be given to the user istead of the original object.
it can be used to protect the user from changing the data inside the dictionary.
note that if it is used, the dictionary will store and free (using the delete function)
the memory used for the copy created by the protect function, 
the user must not attempt to free the memory himself.
for example, continuing the integer example:
void* protect_value_int(void* value) {int *p=(int*)malloc(sizeof(int)); *p=*((int*)param); return p;}
*/
typedef void* (*protect_value)(void* value);


/* this is an optioal function the user might define, its main use is for saving the dictionary to a file*/
typedef void (*value_to_string)(void* value,char *s);

/*this is an optioal function the user might define, its main use is for loading a dictionary from a file*/
typedef void* (*value_from_string)(char* s);

/* this is a constructer for the dictionary, a new dictionary musr be created using this function,
its inputs are the set_value, delete_value and protect_value functions. if the user does not wish
to protect the value he mey put in NULL istead*/
Dictionary_pt get_new_dictionary(set_value sv,delete_value dv,protect_value protect);

/* inserts a value into the dictionary, recives a dictionary pointer, a key an a parameter which will be
used to create the object using the set_value function. return a negetive number if there is an error*/
int insert_to_dictionary(Dictionary_pt dictionary, char* key,void* value_params);

/*searches for an object an the dictionary.
if not found will return NULL,
if found and protect_value is defined will return the reult of protect_value with the object found as the
parameter.
otherwise will return a pointer to the object*/ 
void* find_in_dictionary(Dictionary_pt dictionary, char* key); 

/*deletes the dictionary object and frees all the memory*/
int delete_dictionary(Dictionary_pt dictionary);

/* saves the dictionary to a file in the following foramt:
key to_string(object)\n
the to_string function must be provided by the user, see above*/

int save_dictionary_to_file(Dictionary_pt dictionary,FILE* file,value_to_string to_string);

/* loads a dictionary from a file, the file must be a text file in the following format:
key paramters\n
the parametes are interperted by the from_string function provided by the user
note that this function must not be called before the get_new_dictionery function.
*/

int  load_dictionary_from_file (Dictionary_pt dictionary,FILE* file, value_from_string from_string);


/* note: we can also implement remove_from_dictionary, however this was not needed for this program*/

#endif
