#include "../Dictionary/Dictionary.h"
#include "../list/list.h"
#include "../memory/word.h"
#include "label_handler.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define CODE 1
#define DATA 2

/* the label handler object conteins dictioneries of defined labels, labels defined as extern 
and reserved words (used for validating a label). the reserved word are stored in reserved.data
we also have a list of entrys and a list of uses of extern labels with there locations*/ 
struct Label_handler {
    Dictionary_pt defined_labels;
    List_pt entrys;
    List_pt externs;
    Dictionary_pt extern_labels;
    Dictionary_pt reserved_words;
};

struct Label_def {
    char* label;
    char type;
    word code;
};

struct Label_entry {
    char* label;
    word code;
};

/*-----------------------------------------------------------------------------------------*/
/* the functions set_value_reserved,delete_value_reserved,protect_value_reserved tell the abstract type "dictionary" 
how to handle the type "char*" used here, see dictionary/dictionary.h for further details */
void* set_value_reserved(void* param)
{
    char* s=(char*)malloc((strlen((char*)param)+1)*sizeof(char));
    strcpy(s,(char*)param);
    return s;
}

void* value_from_string_reserved(char* s)
{
    char* str=(char*)malloc((strlen(s)+1)*sizeof(char));
    strcpy(str,s);
    return str;
}

int delete_value_reserved(void* value){
    free(value);
    return 0;
}

void* protect_value_reserved(void* value)
{
    char* s=(char*)malloc((strlen((char*)value)+1)*sizeof(char));
    strcpy(s,(char*)value);
    return s;
}

/*-------------------------------------------------------------------------------*/


/*this function checks if a label was defined as extern, return 1 if yes and 0 if no*/

char is_extern(label_handler label_h,char* label)
{
    if (find_in_dictionary(label_h->extern_labels,label)==NULL)
        return 0;
    return 1;
}

/* this finction adds a label to the list of extern labels used for the extern file, the location of 
the labeel is past with "value"*/
char add_to_extern_list(label_handler label_h,char* label, int IC)
{
    struct Label_entry lab;
    lab.label=label;
    lab.code.value=IC;
    insert_to_list(label_h->externs,&lab);
    return 1;
}

/*this function adds a label to the dictionary of defind labels,
the name is in *label, its location in the code is in "count" and its type is given is *type.
valid types are "code" and "data"*/
int add_label(label_handler label_h,char* label,int count,char *type)
{
    struct Label_def lab;
    lab.label=label;
    lab.type = (strcmp(type, "data")==0) ? DATA : CODE;
    lab.code.value=count;
    insert_to_dictionary(label_h->defined_labels,label,&lab);
    return 0;
}

/*-----------------------------------------------------------------------------------------*/
/* the functions set_value_albel_def,delete_value_label_def,protect_value_label_def tell the abstract type "dictionary" 
how to handle the type "label_def" used here, see dictionary/dictionary.h for further details */
void* set_value_label_def(void* param){
    struct Label_def* label=(struct Label_def*)malloc(sizeof(struct Label_def));
    label->label=(char*)malloc(strlen((((struct Label_def*)param)->label)+1)*sizeof(char));
    strcpy(label->label,((struct Label_def*)param)->label);
    label->type=((struct Label_def*)param)->type;
    label->code.value=((struct Label_def*)param)->code.value;
    return label;
}

int delete_value_label_def(void* value)
{
    free(((struct Label_def*)value)->label);
    free(value);
    return 0;
}

void* protect_value_label_def(void* param)
{
    struct Label_def* label=(struct Label_def*)malloc(sizeof(struct Label_def));
    label->label=(char*)malloc(strlen((((struct Label_def*)param)->label)+1)*sizeof(char));
    strcpy(label->label,((struct Label_def*)param)->label);
    label->type=((struct Label_def*)param)->type;
    label->code.value=((struct Label_def*)param)->code.value;
    return label;
}

/*-----------------------------------------------------------------------------------------*/
/* the functions set_value_label_entry,delete_value_label_entry,protect_value_label_entry tell the abstract type "dictionary" 
how to handle the type "label_entry" used here, see dictionary/dictionary.h for further details */
void* set_value_label_entry(void* param)
{
    struct Label_entry* label=(struct Label_entry*)malloc(sizeof(struct Label_entry));
    label->label=(char*)malloc(strlen((((struct Label_entry*)param)->label)+1)*sizeof(char));
    strcpy(label->label,((struct Label_entry*)param)->label);
    label->code.value=((struct Label_entry*)param)->code.value;
    return label;
}

int delete_value_label_entry(void* value)
{
    free(((struct Label_entry*)value)->label);
    free(value);
    return 0;
}

void* protect_value_label_entry(void* param)
{
    struct Label_entry* label=(struct Label_entry*)malloc(sizeof(struct Label_entry));
    label->label=(char*)malloc(strlen((((struct Label_entry*)param)->label)+1)*sizeof(char));
    strcpy(label->label,((struct Label_entry*)param)->label);
    label->code.value=((struct Label_entry*)param)->code.value;
    return label;
}

/*----------------------------------------------------------------------------------*/

/*this is a label handler constructor, a new label handler must be created using this function*/
struct Label_handler* get_label_handler()
{
    struct Label_handler* label_h=(struct Label_handler*)malloc(sizeof(struct Label_handler));
    FILE* file=fopen("label_handler/reserved.data", "r");

    label_h->defined_labels=get_new_dictionary(&set_value_label_def,&delete_value_label_def,&protect_value_label_def);
    label_h->entrys=get_new_list(&set_value_label_entry,&delete_value_label_entry,NULL);
    label_h->externs=get_new_list(&set_value_label_entry,&delete_value_label_entry,&protect_value_label_entry);
    
    label_h->reserved_words=get_new_dictionary(&set_value_reserved,&delete_value_reserved,&protect_value_reserved);
    load_dictionary_from_file(label_h->reserved_words,file,&value_from_string_reserved);

    label_h->extern_labels=get_new_dictionary(&set_value_reserved,&delete_value_reserved,&protect_value_reserved);


    return label_h;
}

/*this function deletes the label handler and relaeses its memory*/

int delete_label_handler(label_handler label_h)
{
    delete_dictionary(label_h->defined_labels);
    delete_dictionary(label_h->extern_labels);
    delete_dictionary(label_h->reserved_words);
    delete_list(label_h->entrys);
    delete_list(label_h->externs);
    free(label_h);
    return 0;
}

/*this function cheks if a label is valid
it returns 1 for yes and 0 for no, if no an description of the error is put in serror*/
short is_valid_label(label_handler label_h,char *label,char* serror)
{
    int i=0;
    const int len = strlen(label);

    if (len > 31)
    {
        strcpy(serror,"label length can not be more then 31 charecters");
        return 0;
    }
    if (!isalpha(label[0]))
    {
        strcpy(serror,"label must start with a letter");
        return 0;
    }
    for (i=1;i<len;i++)
    {
        if ((!isalpha(label[i])) && (!isdigit(label[i])))
        {
            strcpy(serror,"label must contein only letters or digits");
            return 0;
        }
    }
    if (find_in_dictionary(label_h->defined_labels,label)!=NULL)
    {
        strcpy(serror,"label aleady defined");
        return 0;
    }
    
    if (find_in_dictionary(label_h->reserved_words,label)!=NULL)
    {
        strcpy(serror,"label cannot be a reserved word");
        return 0;
    }

    return 1;
}


/*this function cheks if a label is valid in the case the label is in an entry command
it returns 1 for yes and 0 for no, if no an description of the error is put in serror*/
short is_valid_label_entry(label_handler label_h,char *label,char* serror)
{
    int i=0;
    const int len = strlen(label);

    if (len > 31)
    {
        strcpy(serror,"label length can not be more then 31 charecters");
        return 0;
    }
    if (!isalpha(label[0]))
    {
        strcpy(serror,"label must start with a letter");
        return 0;
    }
    for (i=1;i<len;i++)
    {
        if ((!isalpha(label[i])) && (!isdigit(label[i])))
        {
            strcpy(serror,"label must contein only letters or digits");
            return 0;
        }
    }
    if (find_in_dictionary(label_h->reserved_words,label)!=NULL)
    {
        strcpy(serror,"label cannot be a reserved word");
        return 0;
    }

    return 1;
}

/* this function gets a line of code starting with the .extern command, it adds the label to the 
dictionary of extern labels, if there is an error it will return 0 with the details stored in *serror*/
char add_extern(label_handler label_h,char* line,char*  serror)
{
    char label[32];
    char* COMMAND=".extern";
    int i=0,j=0;
    while (line[i]==' ' || line[i]=='\t')
        i++;
    while (line[i]==COMMAND[j])
    {
        i++;
        j++;
    }
    if (line[i]!=' ' && line[i]!='\t')
    {
        strcpy(serror,"no space after command");
        return 0;
    }
    while (line[i]==' ' || line[i]=='\t')
        i++;
    j=0;
    while (j<32 && line[i]!=' ' && line[i]!='\t' && line[i]!='\n' && line[i]!='\0')
        label[j++]=line[i++];
    label[j]='\0';
    if (j==32 && line[i]!=' ' && line[i]!='\t' && line[i]!='\0')
    {
        strcpy(serror,"label cannot be longer then 31 charecters");
        return 0;
    }
    while (line[i]==' ' || line[i]=='\t')
        i++;
    if (line[i]!='\0' && line[i]!='\n')
    {
        strcpy(serror,"there cannot be chrecters after lebels name");
        return 0;
    }
    if (!is_valid_label(label_h,label,serror))
        return 0;
    insert_to_dictionary(label_h->extern_labels,label,label);
    return 1;
}

/* this function gets a line of code starting with the .entry command, it adds the label to the 
list of entry labels, if there is an error it will return 0 with the details stored in *serror*/
char add_entry(label_handler label_h,char* line,char*  serror)
{
    char label[32];
    char* COMMAND=".entry";
    int i=0,j=0;
    struct Label_entry entry;
    while (line[i]==' ' || line[i]=='\t')
        i++;
    while (line[i]==COMMAND[j])
    {
        i++;
        j++;
    }
    if (line[i]!=' ' && line[i]!='\t')
    {
        strcpy(serror,"no space after command");
        return 0;
    }
    while (line[i]==' ' || line[i]=='\t')
        i++;
    j=0;
    while (j<32 && line[i]!=' ' && line[i]!='\t' && line[i]!='\0' &&line[i]!= '\n')
        label[j++]=line[i++];
    label[j]='\0';
    if (j==32 && line[i]!=' ' && line[i]!='\t' && line[i]!='\0')
    {
        strcpy(serror,"label cannot be longer then 31 charecters");
        return 0;
    }
    while (line[i]==' ' || line[i]=='\t')
        i++;
    if (line[i]!='\0')
    {
        strcpy(serror,"there cannot be chrecters after lebels name");
        return 0;
    }

    
     if (!is_valid_label_entry(label_h,label,serror))
        return 0;
    
    entry.label=label;
    entry.code.value=0;
    
    insert_to_list(label_h->entrys,&entry);
    return 1;
}

/*this function returns the code of a label via *code,
it returns 0 if no label is found and 1 if a the label is found
it will give a 0 if a label is extern, for extern labels use is_extern*/
char get_label_code(label_handler label_h,char* label, word* code)
{
    struct Label_def *label_def=find_in_dictionary(label_h->defined_labels,label);
    char type;
    if (label==NULL || label_def==NULL)
        return 0;
    code->value=(label_def->code).value;
    type=label_def->type;
    /* delete_value_label_def(label_def); */
    return type;
}

/*this function combines the information from the defined labels list with the list of entrys
and produced the list of entrys used for the entry file,
it will report errors to the error handler and will return 1 if no errors acure and 0 othewise*/
char update_entrys(label_handler label_h,int ICF, error_handler err)
{
    struct Label_entry* entry;
    struct Label_def* label;
    reset_list_to_start(label_h->entrys);
    
    while ((entry=get_current(label_h->entrys))!=NULL)
    {
        label=find_in_dictionary(label_h->defined_labels,entry->label);
        if (label==NULL)
        {
            report_error(err, 0, "label not defined");
            return 0;
        }
        entry->code.value= (label->type==CODE) ? label->code.value : ICF+label->code.value;
        Next(label_h->entrys);
    }
    return 1;
}

/*this function prints a list of entrys with there location to a file*/

char print_entrys(label_handler label_h,FILE* file)
{
    struct Label_entry* entry;
    reset_list_to_start(label_h->entrys);

    while ((entry=get_current(label_h->entrys))!=NULL)
    {
        fprintf(file,"%s %.4d\n",entry->label,entry->code.value);
        Next(label_h->entrys);
    }
    return 0;
}

/*this function prints a list of uses of extern labels with there location to a file*/
char print_externs(label_handler label_h,FILE* file)
{
    struct Label_entry* entry;
    reset_list_to_start(label_h->externs);
    
    while ((entry=get_current(label_h->externs))!=NULL)
    {
        fprintf(file,"%s %.4d\n",entry->label,entry->code.value);
        Next(label_h->externs);
    }
    return 0;
}
