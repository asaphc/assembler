#include <string.h>
#include <stdlib.h>
#include "cmd_handler.h"
#include "../Dictionary/Dictionary.h"
#include "../list/list.h"
#include "../memory/word.h"
#include "cmd_table.h"

#define MAX_WORD 2047
#define MIN_WORD -2047


/* the command handler holds: 1. a table of valid commands with their opcodes and their allowd arguments
    2. a connected list of commands in code form withthere arguments
    3. the amount of memory used for the codes (ICF)
*/
struct cmd_handler {
    command_table_pt cmd_table;
    List_pt cmd_list;
    int size;
};

/* the command struct holds a single command, with its opcode, its arguments and/or labels if they are used,
 the arg_num holds information about the amount and type of arguments in the following format:
 the first bit (from the right) in on if there is an argument in arg1,
 bit 2 - there is a label in label1
 bit 3 - an argument an arg2
 bit 4 - a label in label2
 */
typedef struct COMMAND{
    word code;
    char arg_num;
    word arg1;
    word arg2;
    char* label1;
    char* label2;
    int ic;
    char type1;
    char type2;
} command;


/*----------------------------------------------------------------------------------------------*/

/* the functions : set_value_cmd_list,delete_value_cmd_list tell the abstract type list how to handle
the type command used her, see list/list.h for further details */
void* set_value_cmd_list(void* param)
{
    command* cmd=(command*)malloc(sizeof(command));
    cmd->code.value=((command*)param)->code.value;
    cmd->arg_num=((command*)param)->arg_num;
    cmd->arg1.value=((command*)param)->arg1.value;
    cmd->arg2.value=((command*)param)->arg2.value;
    cmd->ic=((command*)param)->ic;
    cmd->type1='A';
    cmd->type2='A';

    if (((command*)param)->label1!=NULL)
    {
        cmd->label1=(char*)malloc((strlen(((command*)param)->label1)+1)*sizeof(char));
        strcpy(cmd->label1,((command*)param)->label1);
    }
    else cmd->label1=NULL;
    if (((command*)param)->label2!=NULL)
    {
        cmd->label2=(char*)malloc((strlen(((command*)param)->label2)+1)*sizeof(char));
        strcpy(cmd->label2,((command*)param)->label2);
    }
    else cmd->label2=NULL;
    return cmd;
}

int delete_value_cmd_list(void* value)
{
    if (((command*)value)->label1!=NULL)
        free(((command*)value)->label1);
    if (((command*)value)->label2!=NULL)
        free(((command*)value)->label2);
    free(value);
    return 0;
}

/*------------------------------------------------------------------------------------------*/

/* this function constructs a command handler, new command handlers must be created by calling this function*/

cmd_handler_pt get_new_cmd_handler()
{
  
    cmd_handler_pt cmd_h=(cmd_handler_pt)malloc(sizeof(struct cmd_handler));
    
    cmd_h->cmd_table=install_table();
    cmd_h->cmd_list=get_new_list(&set_value_cmd_list,&delete_value_cmd_list,NULL);
    cmd_h->size=99;
    return cmd_h;
}


int delete_cmd_handler(cmd_handler_pt cmd_h)
{
    delete_list(cmd_h->cmd_list);
    delete_cmd_table(cmd_h->cmd_table);
    free(cmd_h);
    return 0;
}

/* this fiction gets a line of code (string) and puts the first word into cmd*/
char get_command_name(const char* line, char* cmd)
{
    int i=0;
    while (i<10 && line[i]!=' ' && line[i]!='\t' && line[i]!='\0')
      {
        cmd[i]=line[i];
	i++;
      }
    return 1;
}

command* get_command(char *line ,word code,char allowed,char* errors)
{
    command* cmd=(command*)malloc(sizeof(command));
    int i=0;
    char len=strlen(line);
    char label[32];
    int j=0;
    long num=0;
    char sign=0;
    cmd->code=code;
    cmd->arg_num=0;
    cmd->label1=NULL;
    cmd->label2=NULL;
    cmd->ic=1;


    while (i<len && line[i]!=' ' && line[i]!='\t')
        i++;
    while (i<len && (line[i]==' ' || line[i]=='\t'))
        i++;
    if (i==len)
    {
        if (allowed==0)
            return cmd;
        else {
            strcpy(errors, "missing arguments");
            return NULL;
        }
    }

    /* if first argument expected*/

    if (allowed & 0xF)
    {
        /*check for immediate addressing*/
        if (line[i]=='#')
        {
            if (!(allowed & 0x1))
            {
                strcpy(errors, "immediate addressing not allowed");
                return NULL;
            }
            
            i++;
            num=0;
            sign=0;
            if (line[i]=='+')
                i++;
            if (line[i]=='-')
            {
                sign=1;
                i++;
            }
            if (line[i]<'0' || line[i]>'9')
            {
                strcpy(errors, "must give number after #");
                return NULL;
            }
            while (line[i]>='0' && line[i]<='9')
            {
                num*=10;
                num+=line[i]-'0';
                i++;
            }

            if (num>MAX_WORD)
            {
                strcpy(errors, "word overflow");
                return NULL;
            }
            if (sign == 1)
            {
                num^=0x7ff;
                num++;
            }
            cmd->arg1.value=num;
            cmd->arg_num|=1;
        }

        /* check for relative adressing */
        else if (line[i]=='%')
        {
            if (!(allowed & 0x4))
            {
                strcpy(errors, "relative adressing not allowed");
                return NULL;
            }
            j=0;
            while (j<32 && line[i]!=' ' && line[i]!='\t' && line[i]!=',' && line[i]!='\0')
                label[j++]=line[i++];
            label[j]='\0';
            cmd->label1=(char*)malloc(j*sizeof(char));
            strcpy(cmd->label1,label);
            cmd->code.value |=(1<<3);
            cmd->arg_num|= 2 ;
        }

        /*check for register direct adressing*/
        else if (line[i]=='r' && line[i+1]>='0' && line[i+1]<='7' && (line[i+2]==' ' || line[i+2]=='\t' || line[i+2]==',' || line[i+2]=='\0'))
        {
            if (!(allowed & 0x8))
            {
                strcpy(errors, "register direct adressing not allowed");
                return NULL;
            }
            cmd->arg1.value= 1 << (line[i+1]-'0');
            cmd->code.value |= (3<<2);
            cmd->arg_num |= 1;
            i+=2;
        }
        else {
            if (!(allowed & 2))
            {
                strcpy(errors, "direct adressing not allowed");
                return NULL;
            }
            j=0;
            while (j<32 && line[i]!=' ' && line[i]!='\t' && line[i]!=',' && line[i]!='\0')
                label[j++]=line[i++];
            label[j]='\0';
            cmd->label1=(char*)malloc(j*sizeof(char));
            strcpy(cmd->label1,label);
            cmd->code.value |= (1<<2);
            cmd->arg_num |=2;

        }
        cmd->ic++;
        while (line[i]==' ' || line[i]=='\t')
            i++;
        if (line[i]==',')
        {
            i++;
            while (line[i]==' ' || line[i]=='\t')
                i++;
        }
    }

    /*chek for second argument - this part is symetrical to the first argument*/
    allowed >>= 4;
    if (allowed & 0xF)
    {
        if (line[i]=='#')
        {
            if (!(allowed & 0x1))
            {
                strcpy(errors, "immediate addressing not allowed");
                return NULL;
            }
            i++;
            num=0;
            sign=0;
            if (line[i]=='+')
                i++;
            if (line[i]=='-')
            {
                sign=1;
                i++;
            }
            if (line[i]<'0' || line[i]>'9')
            {
                strcpy(errors, "must give number after #");
                return NULL;
            }
            while (line[i]>='0' && line[i]<='9')
            {
                num*=10;
                num+=line[i]-'0';
                i++;
            }

            if (num>MAX_WORD)
            {
                strcpy(errors, "word overflow");
                return NULL;
            }
            if (sign == 1)
            {
                num^=0xfff;
                num++;
            }
            cmd->arg2.value=num;
            cmd->arg_num|=4;
            
        }
        else if (line[i]=='%')
        {
            if (!(allowed & 0x4))
            {
                strcpy(errors, "relative adressing not allowed");
                return NULL;
            }
            j=0;
            while (j<32 && line[i]!=' ' && line[i]!='\t' && line[i]!=',' && line[i]!='\0')
                label[j++]=line[i++];
            label[j]='\0';
            cmd->label2=(char*)malloc(j*sizeof(char));
            strcpy(cmd->label2,label);
            cmd->code.value |= (1<<1);
            cmd->arg_num|=8;
        }

        else if (line[i]=='r' && line[i+1]>='0' && line[i+1]<='7' && (line[i+2]==' ' || line[i+2]=='\t' || line[i+2]==',' || line[i+2]=='\0'))
        {
            if (!(allowed & 0x8))
            {
                strcpy(errors, "register direct adressing not allowed");
                return NULL;
            }
            cmd->arg2.value= 1 << (line[i+1]-'0');
            cmd->code.value |= 0x3;
            cmd->arg_num|=4;
            i+=2;

        }
        else {

            if (!(allowed & 2))
            {
                strcpy(errors, "direct adressing not allowed");
                return NULL;
            }
            j=0;
            while (j<32 && line[i]!=' ' && line[i]!='\t' && line[i]!=',' && line[i]!='\0')
                label[j++]=line[i++];
            label[j]='\0';
            cmd->label2=(char*)malloc(j*sizeof(char));
            strcpy(cmd->label2,label);
            cmd->code.value |= 1;
            cmd->arg_num|=8;
            

        while (line[i]==' ' || line[i]=='\t')
            i++;
        }
    cmd->ic++; 
    }
    return cmd;
} 

/* 
this function is used in the second round of the assembler to update the values of the labels,
the assembler asks the command handler for the next label it needs to know the code for by calling this function.
the function returns the next label, or NULL if there is no more labels
*/

char* request_label_info(cmd_handler_pt cmd_h,int* ic)
{
    command* cmd;
    while ((cmd=get_current(cmd_h->cmd_list))!=NULL)
    {
        if (cmd->label1!=NULL)
        {
            *ic=cmd->ic+1;
            return cmd->label1;
        }
        if (cmd->label2!=NULL)
        {
            *ic= (cmd->arg_num & 3) ? cmd->ic+2: cmd->ic+1;
            return cmd->label2;
        }
        Next(cmd_h->cmd_list);
    }
    return NULL;
}


/* this function is called as a response to the previous one, after reciving the request for the meaning of the label
the assembler sends the information contening the code the label must be replaced by
the arguments for the function are the command handler, the label (for error checking) and the code
*/
char send_label_info(cmd_handler_pt cmd_h, char* label, word code,char type)
{
  
    command* cmd=get_current(cmd_h->cmd_list);
    
     
    if (cmd==NULL)
        return 0;
    if (cmd->label1!=NULL)
    {
        if (strcmp(cmd->label1,label)!=0)
	  {
	  
	    return 0;
	  }
        cmd->arg1.value=code.value;
        cmd->label1=NULL;
        cmd->type1=type;
        return 1;
    }   
    if (cmd->label2!=NULL)
    {
        if (strcmp(cmd->label2,label)!=0)
	  {
             return 0;
	  }
        cmd->arg2.value=code.value;
        cmd->label2=NULL;
        cmd->type2=type;
        return 1;
    }
    return 0;
}



/* this function gets a line of code from the interperter,
assumption - the line begins with the command (label already removed)
the function then reads the command, looks it up in the command table,
gets its code and allowed arguments, then it analizes the line of code and creates a command which is added
to list in the command handler,
if there is an error it will wright the error to "errors" and return 0, otherwis it will return 1
it as assumed that appropriate memory is allocated for "errors".
the ic of the command is return via *ic
*/

char add_command(cmd_handler_pt cmd_h, char* line, char* errors,int* ic)
{
    char cmd[10];
    char allowed=0;
    word code;
    command* com;
    if (!get_command_name(line,cmd))
    {
        strcpy(errors, "unknown error");
        return 0;
    }

    if (!select_cmd(cmd_h->cmd_table,cmd))
    {
        strcpy(errors, "unknown command");
        return 0;
    }

    code= get_cmd_code(cmd_h->cmd_table);
    
    allowed=get_cmd_formats(cmd_h->cmd_table);

    if (!(com=get_command(line ,code, allowed,errors)))
        return 0;

    *ic=cmd_h->size+1;
    cmd_h->size+=com->ic;
    com->ic=*ic;
    insert_to_list(cmd_h->cmd_list,com);
    free(com);

    return 1;
}


/* when updating labels used to skip a label - in case of extern labels*/
int skip_label(cmd_handler_pt cmd_h,char* label)
{
    word code;
    code.value=0;
    send_label_info(cmd_h,label, code, 'E');
    return 1;
}

/*
this is the main function of this part:
it recives a line of code together with the opcode of the command and the allowed argumnts in the following format:
the first (right) 4 bits of allowed are for the first argument of the command, bit 1 is for adressing type 0, 
bit 2 for adressing type 1 and so on.
the last 4 bits are for the second argument

the finction analizes the arguments and if they are valid will ctrate a command struct instance with all the 
information on the command.
*/


/* this function must be called before replacing labels, it makes sure we start at the begining of the code*/

void reset_cmd_list(cmd_handler_pt cmd_h)
{
    reset_list_to_start(cmd_h->cmd_list);
}

/* this function returns the amount words (memory) in the code*/

int get_size(cmd_handler_pt cmd_h)
{
    return cmd_h->size;
}
/* this function return the IC (instruction counter) of the command holding the label currently looked at
this is used to update the extern labels
*/
 int get_IC(cmd_handler_pt cmd_h)
 {
     return ((command*)get_current(cmd_h->cmd_list))->ic+100;
 }
/* this function prints the code into the given file*/

int print_cmd(cmd_handler_pt cmd_h,FILE* file)
{
    
    command* cmd;
    int count=100;

    reset_list_to_start(cmd_h->cmd_list);
    while ((cmd=get_current(cmd_h->cmd_list))!=NULL)
    {
        fprintf(file,"%.4d %.3X A\n",count++,cmd->code.value);
        if ((cmd->arg_num & 1) || (cmd->arg_num & 2))
            fprintf(file,"%.4d %.3X %c\n",count++,cmd->arg1.value, cmd->type1);
        if ((cmd->arg_num & 4) || (cmd->arg_num & 8))
            fprintf(file,"%.4d %.3X %c\n",count++,cmd->arg2.value, cmd->type2);

        Next(cmd_h->cmd_list);
    }
    return count;
}
