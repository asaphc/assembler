#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "../error/error.h"
#include "../data_handler/data_handler.h"
#include "../cmd_handler/cmd_handler.h"
#include "../label_handler/label_handler.h"

/* this function checks if a line is a comment*/
short is_comment(const char* line);

/* this function checks if a label was defind at the start of the line,
if there is one, the label will be stored in *label and the function will return 1,
otherwise the function returns 0*/
short get_label_defined(const char *line,char * label);

/*checks if the line is in istruction (.data, .string, .extern, .entry)
if so, the type of instruction is stored in *command and the function will return 1,
otherwise it will return 0*/
short is_instraction(const char* line, char* command);


/* this function interperts code file, it stores the results in the command hndler, the data handler 
and label handler acordingly, if there are errors they are given to the error handler,
this in equvilent to the first round of the assembler
*/
void interpert(FILE* file,cmd_handler_pt cmd_h,Data_handler_pt data_h,
                        label_handler label_h,error_handler err)
{
    char line[82];
    char label[32];
    char serror[100];
    char command[10];
    char ret_v = 0;
    int line_count = 0;
    char def_label = 0;
    unsigned char pos = 0;
    int count=0;
    char* type;
    int i=0;

    while (fgets(line, 81, file) != NULL)
    {
      i=0;
      while (i<82 && line[i]!='\n' && line[i]!='\0')
	{
	  i++;

	}
     
      line[i]='\0';
    
      
        line_count++;

        if (strlen(line)>80)
            {
                report_error(err,line_count,"line must not be longer then 80 charecters");
            }
           
        pos = 0;

        if (!is_comment(line))
        {
	  
            def_label = get_label_defined(line, label);
            if (def_label == 1)
            {
                if (!is_valid_label(label_h, label, serror))
                    report_error(err, line_count, serror);
                pos += strlen(label)+1;
            }


            while (line[pos]==' ' || line[pos]=='\t')
                pos++;

            ret_v = is_instraction(line + pos, command);
            /* if is istruction */
            if (ret_v) 
            {
                if (strcmp(command, "data")==0)
                    {
                        if (!add_data(data_h, line + pos, serror ,&count))
                            report_error(err, line_count, serror);
                        else type="data";
                    }
                else if (strcmp(command, "extern")==0)
                    {
                        if (!add_extern(label_h, line + pos, serror))
                            report_error(err, line_count, serror);
                    }
                else if (strcmp(command, "entry")==0)
                    {
		      
                        if (!add_entry(label_h, line + pos, serror))
                            report_error(err, line_count, serror);
                    }
            } 
            else{
                if (!add_command(cmd_h, line+pos,serror, &count))
                    report_error(err, line_count, serror);
                else type="cmd";
            }
            if (def_label==1)
                add_label(label_h,label,count,type);
        }
    }
}

/* this function checks if a line is a comment*/

short is_comment(const char* line)
{
    int i=0;
    
    if (line==NULL)
    return 0;
    
    while (line[i]==' ' || line[i]=='\t')
        i++;
    if (line[i]==';')
        return 1;
    return 0;
}

/* this function checks if a label was defind at the start of the line,
if there is one, the label will be stored in *label and the function will return 1,
otherwise the function returns 0*/
short get_label_defined(const char *line,char * label)
{
    int start=0;
    int end=0;
    int len=0;
    int i=0;
    if (line==NULL || label==NULL)
        return -1;
    len=strlen(line);
    while (line[start]==' ' || line[start]=='\t')
        start++;
    end=start;
    while (end<len && line[end]!=' ' && line[end]!='\t' && line[end]!=':')
        end++;
    if (end<len && line[end]==':')
    {
        i=0;
        for(;start<end;start++)
            label[i++]=line[start];
        label[i]='\0';
        return 1;
    }
    return 0;

}

/*checks if the line is in istruction (.data, .string, .extern, .entry)
if so, the type of instruction is stored in *command and the function will return 1,
otherwise it will return 0*/
short is_instraction(const char* line, char* command)
{
    char cmd[10];
    int i=0;
    while (i<10 && line[i]!=' ' && line[i]!='\t' && line[i]!='\0')
      {
        cmd[i]=line[i];
	i++;
      }
    cmd[i]='\0';
    if ((strcmp(cmd,".data")==0) || (strcmp(cmd,".string")==0))
        {
            strcpy(command, "data");
            return 1;
        }
    if (strcmp(cmd,".entry")==0)
    {
        strcpy(command, "entry");
            return 1;        
    }
    if (strcmp(cmd,".extern")==0)
    {
        strcpy(command, "extern");
            return 1;        
    }
    return 0;
    
}
