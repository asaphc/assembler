#ifndef DATA_HANDLER_H
#define DATA_HANDLER_H

/* this librery handles the data defined in the assmbly code, (numbers and strings)*/

/* this is the data handling object*/
typedef struct Data_handler * Data_handler_pt;

/* this function gets a line of code with a .data or .string command(without the label),
it anlayzes and stores the data. errors are returned throw *errors, momory for errors assumed.
the size of the data is return via *count, the function returns 1 if no errors accore or 0 if 
there are errors. */
short add_data(Data_handler_pt data_h, char *line, char *errors,int* count);

/* this is the data_handler constructor, a new data handler musy be created via this function.*/
Data_handler_pt get_new_data_handler();

/* this function prints the data to a given file, used for creating the object file*/
int print_data(Data_handler_pt data_h,FILE* file,int ICF);

/* return the size of the data*/
int get_size_data(Data_handler_pt data_h);

/*deletes the data_handler and frees the memory*/
int delete_data_handler(Data_handler_pt data_h);

#endif