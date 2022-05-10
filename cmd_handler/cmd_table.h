#ifndef CMD_TABLE_H
#define CMD_TABLE_H

#include "../Dictionary/Dictionary.h"
#include "../memory/word.h"

/* this librey creates and managers a command table,
using a list of command stored in the file cmd_table.data
together with their opcodes and allowed arguments
*/


typedef struct Command_table* command_table_pt;

/* this functin asks the table to look up the command cmd,
the function returns 0 if no command in found and 1 otherwise
*/

char select_cmd(command_table_pt table, char* cmd);

/* this function returns the opcode of the conmmand preivously selected by select_cmd()*/

word get_cmd_code(command_table_pt table);

/* this function returns the allowed arguments of the conmmand preivously selected by select_cmd()
the first (right) 4 bits of allowed are for the first argument of the command, bit 1 is for adressing type 0, 
bit 2 for adressing type 1 and so on.
the last 4 bits are for the second argument

*/
char get_cmd_formats(command_table_pt table);

/* this function reads cmd_table.data and loads the table in to memory*/

command_table_pt install_table();

/* this function deletes a command table and frees the memory*/
int delete_cmd_table(struct Command_table* table);


#endif
