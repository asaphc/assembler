assembler: cmd_handler/cmd_handler.c cmd_handler/cmd_table.c data_handler/data_handler.c Dictionary/Dictionary.c Dictionary/RB_tree.c error/error.c interperter/interperter.c label_handler/lable_handler.c list/list.c list/list.h compiler/compiler.c compiler/compiler.h assembler.c 
	gcc cmd_handler/cmd_handler.c cmd_handler/cmd_table.c data_handler/data_handler.c Dictionary/Dictionary.c Dictionary/RB_tree.c error/error.c interperter/interperter.c label_handler/lable_handler.c list/list.c list/list.h compiler/compiler.c compiler/compiler.h assembler.c -Wall -ansi -pedantic -o assembler
clean:
	rm *~
