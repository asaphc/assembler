#include "cmd_table.h"
#include "../memory/word.h"
#include "../Dictionary/Dictionary.h"
#include <stdlib.h>
typedef struct Cmd_entry {
    word code;
    char allowed;
} cmd_entry;

struct Command_table{
    Dictionary_pt table;
    cmd_entry* current;
};


/* this fuctions assingns the entry to the dictionary - see Dictionary.h for more details*/
void* set_value_cmd_table(void* param){
    cmd_entry* entry=(cmd_entry*)malloc(sizeof(cmd_entry));
    entry->allowed=((cmd_entry*)param)->allowed;
    entry->code.value=((cmd_entry*)param)->code.value;
    return entry;    
}

/* this fuctions traslates from a string to an entry - see Dictionary.h for more details*/
void* value_from_string_cmd_table(char* s)
{
    cmd_entry* entry=(cmd_entry*)malloc(sizeof(cmd_entry));
    int i=0;
    long code=0;
    int num=0;
  
    entry->allowed=0;
    
    while (s[i]==' ')
        i++;
    while (s[i]!=' ')
    {
        num*=10;
        num+=s[i]-'0';
        i++;
    }
    code += num << 8;
    i++;
    num=0;
    while (s[i]!=' ')
    {
        num*=10;
        num+=s[i]-'0';
        i++;
    }
    code += num << 4;
    entry->code.value=code;
    
    i++;

    if (s[i]!= '#')
    {
        while (s[i]!=' ')
        {
            entry->allowed |= 1 << (s[i]-'0');
            i++;
        }
        
    }

    i++;
    
    while (s[i]==' ')
        i++;

    if (s[i]!= '#')
    {
        while (s[i]>='0' && s[i]<='9')
        {
	  entry->allowed |= 1 << ((s[i]-'0') + 4);
            i++;
        }
    }
    return entry;
    
}


/* frees memory for when the table is no longer in use -  see Dictionary.h for more details*/

int delete_value_cmd_table(void* value)
{
    free((cmd_entry*)value);
    return 0;
}

/* returns a copy of an entry - see Dictionary.h for more details*/
void* protect_value_cmd_table (void* param){
    cmd_entry* entry=malloc(sizeof(cmd_entry));
    entry->allowed=((cmd_entry*)param)->allowed;
    entry->code.value=((cmd_entry*)param)->code.value;
    return entry;    
}

/* this functin asks the table to look up the command cmd,
the function returns 0 if no command in found and 1 otherwise
*/
char select_cmd(command_table_pt table, char* cmd)
{
    table->current=(cmd_entry*)find_in_dictionary(table->table, cmd);
    if (table->current==NULL)
        return 0;
    return 1;
}

/* this function returns the opcode of the conmmand preivously selected by select_cmd()*/

word get_cmd_code(command_table_pt table){
    word code;
    if (table->current!=NULL)
        code.value=table->current->code.value;
    else 
        code.value=0;
    return code;
}

/* this function returns the allowed arguments of the conmmand preivously selected by select_cmd()
the first (right) 4 bits of allowed are for the first argument of the command, bit 1 is for adressing type 0, 
bit 2 for adressing type 1 and so on.
the last 4 bits are for the second argument

*/

char get_cmd_formats(command_table_pt table){
    if (table->current!=NULL)
        return table->current->allowed;
    return 0;
}

void value_to_string_table(void* value,char *s)
{
    sprintf(s,"%d %d",((cmd_entry*)value)->code.value, ((cmd_entry*)value)->allowed);
}


/* this function reads cmd_table.data and loads the table in to memory*/

command_table_pt install_table()
{
 
    FILE* file=fopen("cmd_handler/cmd_table.data", "r");
    command_table_pt table=(command_table_pt)malloc(sizeof(struct Command_table));

    if (file==NULL)
      {
	printf("cannot open cmd_table.data");
	exit(-1);
      }
    table->table=get_new_dictionary(&set_value_cmd_table,&delete_value_cmd_table,&protect_value_cmd_table);
    load_dictionary_from_file(table->table,file,&value_from_string_cmd_table);
    table->current=NULL;
    return table;
}

/* this function deletes a command hanler and frees its memory */

int delete_cmd_table(struct Command_table* table)
{
    delete_dictionary(table->table);
    free(table);
    return 0;
}
