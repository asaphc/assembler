
#ifndef _INTERPERTER_H_
#define _INTERPERTER_H_

#include <stdio.h>
#include "../interperter/interperter.h"
#include "../cmd_handler/cmd_handler.h"
#include "../data_handler/data_handler.h"
#include "../error/error.h"
#include "../label_handler/label_handler.h"

/* this function interperts code file, it stores the results in the command hndler, the data handler 
and label handler acordingly, if there are errors they are given to the error handler,
this in equvilent to the first round of the assembler*/

void interpert(FILE* file,cmd_handler_pt cmd_h,Data_handler_pt data_h,
                        label_handler lable_h,error_handler err);

#endif