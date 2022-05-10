#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "data_handler.h"
#include "../list/list.h"
#include "../memory/word.h"

#define MAX_WORD 2047
#define MIN_WORD -2047

/* the data hanler object*/
struct Data_handler
{
    List_pt data;
    int size;
};

/* return the size of the data*/

int get_size_data(Data_handler_pt data_h)
{
    return data_h->size;
}

/* adds a word of memory to the data */
int add_word_to_data(Data_handler_pt data_h, long value);

/*-----------------------------------------------------------------------------------------*/
/* the functions set_value_data,delete_value_data,protect_value_data tell the abstract type "list" 
how to handle the type "word" used here, see list/list.h for further details */

void *set_value_data(void *param)
{
    word *data = (word *)malloc(sizeof(word));
    data->value = ((word *)param)->value;
    return data;
}

int delete_value_data(void *value)
{
    free(value);
    return 0;
}

void *protect_value_data(void *value)
{
    word *data = (word *)malloc(sizeof(word));
    data->value = ((word *)value)->value;
    return data;
}

/*-------------------------------------------------------------------*/

/* this is the data_handler constructor, a new data handler musy be created via this function.*/
Data_handler_pt get_new_data_handler()
{
  Data_handler_pt datah;
  datah = (Data_handler_pt)malloc(sizeof(struct Data_handler));
    datah->data = get_new_list(&set_value_data, &delete_value_data, &protect_value_data);
    datah->size = 0;
    return datah;
}

/*deletes the data_handler and frees the memory*/

int delete_data_handler(Data_handler_pt data_h)
{
    delete_list(data_h->data);
    free(data_h);
    return 0;
}

/* this function gets a line of code with a .data or .string command(without the label),
it anlayzes and stores the data. errors are returned throw *errors, momory for errors assumed.
the size of the data is return via *count, the function returns 1 if no errors accore or 0 if 
there are errors. */

short add_data(Data_handler_pt data_h, char *line, char *errors, int* count)
{
    char str[11];
    int i = 0;
    long num = 0;
    short sign=0;
    *count=data_h->size+1;

    while (line[i] != ' ' && line[i] != '\t' && i < 10)
      {
	str[i] = line[i];
	i++;
      }
    str[i] = '\0';

    if (strcmp(str, ".data") == 0)
    {
        while (line[i]!='\0')
        {
            sign=0;
            num=0;
            while (line[i]==' ' || line[i]=='\t')
                i++;
            if (line[i]=='\0')
            {
                strcpy(errors, "expected number");
                return 0;
            }
            if (line[i]=='+')
                i++;
            else if (line[i]=='-')
                {
                    sign=1;
                    i++;
                }
            if (line[i]<'0' || line[i]>'9')
            {
                strcpy(errors, "expected number");
                return 0;
            }
            num+=line[i]-'0';
            i++;
            while (line[i]>='0' && line[i]<='9')
            {
                num*=10;
                num+=line[i]-'0';
                i++;
            }

            if (num > MAX_WORD || num < MIN_WORD)
            {
                strcpy(errors, "number out of bounds");
                return 0;
            }
            else
            {
                if (sign==1)
                {
                    num ^=0xfff;
                    num++;
                }
                add_word_to_data(data_h, num);
            }
            while (line[i]==' ' || line[i]=='\t')
                i++;
            if (line[i]!='\0')
            {
                if (line[i]!=',')
                {
                    strcpy(errors, "expected comma");
                    return 0;
                }
                else i++;
            }
        }
    }

    
    else if (strcmp(str, ".string") == 0)
    {
        while (line[i] == ' ' || line[i] == '\t')
            i++;
        if (line[i] == '\0')
        {
            strcpy(errors, "string decleard but no string is given");
            return 0;
        }

        if (line[i] != '\"')
        {
            strcpy(errors, "string must start with \"");
            return 0;
        }
        else
        {
            i++;
            while (line[i] != '\"') 
            {
                if (line[i] == '\0')
                {
                    strcpy(errors, "string must end with \"");
                    return 0;
                }
                add_word_to_data(data_h, line[i]);
                i++;
            }
            add_word_to_data(data_h, '\0');
            i++;
            while (line[i] != '\0')
            {
                if (line[i] != ' ' && line[i] != '\t')
                {
                    strcpy(errors, "there must be no more stuff after the end of the string");
                    return 0;
                }
            }
        }
    }

    else
    {
        strcpy(errors, "unknown type of data");
        return 0;
    }
    return 1;
}

/* adds a word of memory to the data*/
int add_word_to_data(Data_handler_pt data_h, long value)
{
    word val;
    const long mask=0xfff;
    if(value<0)
    {
        value=-value;
        value^=mask;
        value++;
    }
    val.value=value;
    insert_to_list(data_h->data,&val);
    data_h->size++;
    return 0;
}

/* this function prints the data to a given file, used for creating the object file*/
int print_data(Data_handler_pt data_h,FILE* file,int ICF)
{
    word* data;
    int count=ICF;
    reset_list_to_start(data_h->data);
  
    while ((data=get_current(data_h->data))!=NULL)
    {
        fprintf(file,"%.4d %.3X A\n",count++,data->value);
        Next(data_h->data);
    }
    return 1;
}
