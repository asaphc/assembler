
#ifndef _RB_TREE_H_
#define _RB_TREE_H_

#include "entry.h"

/* this is an implemintaion of a red-black tree,
while in this program this is only used by the dictionery, the implemintation is abstruct and 
can be used more widley,
note: it is the user who is resposible for handiling the memory of the entrys (in this case the
dictionery is the user)*/



typedef struct RB_TREE* Tree_pt;

/*this function must be provided by the user, it tell the tree how to compare between two entries*/
typedef int (*compare_entrys)(Entry_pt left, Entry_pt right);

/* this is a tree constructor, all new trees must use this function to be created*/
Tree_pt new_tree(compare_entrys comp);

/* return an entry for which the compare function will return 0 when matched with the key entry.
return NULL if no such entry is found */
Entry_pt find_in_tree(Tree_pt tree, Entry_pt key);

/* inserts an entry in to the tree*/

int RB_insert(Tree_pt tree, Entry_pt entry);

/* deletes the tree and frees the memory*/
void delete_tree(Tree_pt tree);

#endif
