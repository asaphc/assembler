#include "../interperter/interperter.h"
#include "../cmd_handler/cmd_handler.h"
#include "../data_handler/data_handler.h"
#include "../label_handler/label_handler.h"
#include "../error/error.h"
#include "compiler.h"

#include <stdio.h>
#include <string.h>

#define CODE 1
#define DATA 2

/* this function updates the labels in the code after the initial interpertation of the file*/
void update_labels(cmd_handler_pt cmd_h,Data_handler_pt data_h,label_handler label_h,error_handler err);

/*this function creates and writes the object, entry and extern files*/

int create_files(char* name,cmd_handler_pt cmd_h,Data_handler_pt data_h,label_handler label_h);

/* this is the main function of the prgram,
it recives a file, and produces the object, entry and extern files, with the name *name
if there are errors the are written to the "error" file (or stream).
*/
int compile(FILE* file,FILE* error,char* name)
{

    /*create the hndlers*/
     
     Data_handler_pt data_h;
     cmd_handler_pt cmd_h;
     error_handler err;
     label_handler label_h;
     
     data_h=get_new_data_handler();
     cmd_h=get_new_cmd_handler();
     err=get_error_handler();
     label_h=get_label_handler();
   
     
    /*first round, stores the information from the file in the handlers*/
    interpert(file,cmd_h,data_h,label_h,err);

   
    /*second round - updates the labels*/
    update_labels(cmd_h,data_h,label_h,err);

    
    /*updates the entrys using the information in the label handler (the function is from label_handler.h)*/
    update_entrys(label_h, get_size(cmd_h), err);

  
    
    if (has_errors(err))
    {
        print_errors(err,stdout);
    }

    else create_files(name,cmd_h,data_h,label_h);

    delete_cmd_handler(cmd_h);
    delete_data_handler(data_h);
    delete_label_handler(label_h);
    delete_error_handler(err);

    return 0;
}


/* this function updates the labels in the code after the initial interpertation of the file*/

void update_labels(cmd_handler_pt cmd_h,Data_handler_pt data_h,label_handler label_h,error_handler err)
{
    /* the function requests the next label from the command handler, it then checks its value and type 
    with the label handler and updated the command handler and if needed the extern list*/
    /*the functions request_label_info and send_label_info are the way of talking to the command handler,
    they can be found in cmd_handler.h*/

    
    char* label;
    word code;
    char type=0;
    char relative=0;
    int IC;
    int ICF=get_size(cmd_h);
    long num=0;
  
    reset_cmd_list(cmd_h);
   
    
    while ((label=request_label_info(cmd_h,&IC))!=NULL)
    { 
        relative=0;
        if (label[0]=='%')
        {
            relative=1;
            label++;
        }
        if ((type=get_label_code(label_h,label,&code)))
        {
            if (type==DATA)
                code.value+=ICF;
            if (relative==1)
            {
                if (IC>code.value)
                {
                    num = IC - code.value;
                    num^=0xfff;
                    num++;
                    code.value=num;
                    
                }
                else code.value -= IC;
                label--;
            }

            send_label_info(cmd_h,label,code, (relative==1) ? 'A' : 'R');
        }
        else if (is_extern(label_h,label))
            {
                code.value=0;
                send_label_info(cmd_h,label,code,'E');
                add_to_extern_list(label_h,label,IC);
            }
        else {
            report_error(err,IC,"undefined label");
            skip_label(cmd_h,label);
        }
    }
}


/*this function creates and writes the object, entry and extern files*/

 int create_files(char* name,cmd_handler_pt cmd_h,Data_handler_pt data_h,label_handler label_h)
 {
    int len;
    
    char* filename;
    FILE* file;
    int ICF=0;

    len=strlen(name);
    name[len-3]='\0';
    filename=strcat(name,".ob");
    file=fopen(filename,"w");
    
    fprintf(file,"%d %d\n",get_size(cmd_h)-99,get_size_data(data_h));
    ICF=print_cmd(cmd_h,file);
    print_data(data_h,file,ICF);

    fclose(file);
    name[len-3]='\0';
    filename=strcat(name,".ent");
    file=fopen(filename,"w");
    print_entrys(label_h,file);

    fclose(file);
    name[len-3]='\0';
    filename=strcat(name,".ext");
    file=fopen(filename,"w");
    print_externs(label_h,file);
    
    return 0;
 }
