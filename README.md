# assembler - C
this project was submitted as a final project in the "lab in ststem programming" couse for The Open University.  
the program reads an assembly file, checks for errors and creates files ready for linking.  
for full explination of the project see "assembler project.pdf".  
  
/---------------------------------------------------------  
a breif explination on the strucutre of the program.  
/---------------------------------------------------------  
  
1. data structures  
  
the program uses two data structures:  
  
a. a linked list  
b. a dictionary - maps strings (keys) to some kind of data (implemeted with a red-black tree)  
  
both structures are abstract, so the user must define three functions:  
  
I. set_value: tells the structure how to create a new instace of data based on the given parameters  
   	      this function is charged with allocting the memory for the data stored.  
II. delete: tells the structure how to free the memory allocated in set_value.  
III. protect_value: this is optinal - if the user chooses to implement this function, when the user  
          askes for the data, it will recive a copy created by protect_value,  
          and thus will not be able to change the memory of the structure itself.  
		      note that the structure will delete the copy when it is no longer in use.  
		      so the user must not delete it himself.  
further deatails + example can be found in list.h and dictionary.h  
  
2. handlers  
  
there are four main objects designed to handell the diffrent aspects of the prgram:  
  
a. command handler - interperts assembly command lines, stores them, and prints the interpertetion  
   	   	     to a given file. also include a protocol for updating labels (second round)  
  
b. data handler - interperts and stores the data (.data and .string commands) and print them to a file  
c. label handler - deals with all aspects to do with labels - defined labels, entrys and externs  
   	 	   checks the validity and stores the labels. also used to update labels, entrys externs.  
d. error handler - all errors are past to the error handler, it stores and later prints them.  
  
3. main flow  
  
there are three parts to the main flow of the program:  
  
a. assembler.c - this is the bgening, it reads the command given, opens the file asked to compile  
   	       	 and passes it on to the compiler.  
b. compiler.c - creates (and later deletes) the handlers, passes the file to the interperter (first round)  
   	      	updates the labels, externs and entrys (second round) and creates the output files.  
c. interperter.c - this is the first round of the assembler. it reads the file and fills the handler  
   		   with the information they need.  
  
4. important files  
  
there are two files that are important that are not .c or .h files:  
a. cmd_table.data - this file includes all the information about the commands of the assembly language.  
each line is a command and has the format:  
  
command opcode funct allowed_addressing_for_arg1 allowed_addressing_for_arg2  
  
when a command handler is created this file is read and a command table is created in memory  
(see cmd_table.h)  
  
b. reserved.data - a list of reserved word of the assembly language. used for validating labels.  
read when a label andler is created.  
  
  
