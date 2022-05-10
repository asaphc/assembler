#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "RB_Tree.h"
#include "Dictionary.h"

#define MIN_JUMP 5
#define START_SIZE 100

/* the implementation of the tree is based on the book 
"introduction to Algorithims" by Cormen, Lesierson, Rivest & Stein.
hebrew traslation by the open university 2008. chapter 13.
futher details can be found there*/

enum COLOR{RED=0,BLACK=1};

typedef struct NODE {
  struct NODE* father;
  struct NODE* lson;
  struct NODE* rson;
  Entry_pt entry;
  enum COLOR color;
}Node;

struct RB_TREE {

Node* root;

Node** memory;
int size;
int pos;

Node* NIL;

compare_entrys comp;

};
/* this is a tree constructor, all new trees must use this function to be created*/
Tree_pt new_tree(compare_entrys comp)
{

  Tree_pt tree=(Tree_pt)malloc(sizeof(struct RB_TREE));

  if (tree==NULL)
    return NULL;

  tree->memory=(Node**)malloc(START_SIZE*sizeof(Node*));
  tree->size=START_SIZE;
  tree->pos=0;
  if (tree->memory==NULL)
    return NULL;

  tree->NIL=(Node*)malloc(sizeof(Node));
  tree->NIL->father=tree->NIL;
  tree->NIL->lson=tree->NIL;
  tree->NIL->rson=tree->NIL;
  tree->NIL->color=BLACK;

  tree->root=tree->NIL;
  
  tree->comp=comp;

  return tree;

}



int RB_insert_fixup(Tree_pt tree, Node* new);
int left_rotate(Tree_pt tree, Node* x);
int right_rotate(Tree_pt tree, Node* x);


/* inserts an object into the tree, 
see refrence book at the top of the file for more details*/
int RB_insert(Tree_pt tree,Entry_pt entry)
{
  Node* new;
  Node* x;
  Node* y;
  Node* NIL=tree->NIL;

  if (tree->pos==tree->size)
  {
    if ((tree->memory=(Node**) realloc(tree->memory,(tree->size+MIN_JUMP)*sizeof(Node*)))==NULL)
	    return -1;
    tree->size+=MIN_JUMP;
  }
    
  if (((*(tree->memory+tree->pos))=((Node*) malloc(sizeof(Node))))==NULL)
   return -1;

  new=*(tree->memory+tree->pos);
  new->entry=entry;

  tree->pos++;
  x=tree->root;
  y=NIL;

  while (x!=NIL)
    {


      y=x;
      if ((*(tree->comp))(new->entry,x->entry)<0)
	      x=x->lson;
      else
	      x=x->rson;
    }
  new->father=y;
  if (y==NIL)
    tree->root=new;
  else{
    if ((*(tree->comp))(new->entry,y->entry)<0)
      y->lson=new;
    else
      y->rson=new;
  }
  new->lson=NIL;
  new->rson=NIL;
  new->color=RED;
 
  RB_insert_fixup(tree,new);
  return 0;
}

/* makes sure the tree remains proper after insert.
see refrence book at the top of the file for more details*/
int RB_insert_fixup(Tree_pt tree, Node* new)
{
  Node* y;
  
  while (new->father->color==RED)
  {
    if (new->father==new->father->father->lson)	
    { 
	    y=new->father->father->rson;
      if (y->color==RED)
      {
	      new->father->color=BLACK;
	      y->color=BLACK;
	      new->father->father->color=RED;
	      new=new->father->father;
      }
      else
		  {
		    if (new==new->father->rson)
		    {
		      new=new->father;
		      left_rotate(tree,new);
		    }
		    new->father->color=BLACK;
		    new->father->father->color=RED;
		    right_rotate(tree,new->father->father);
		  
		  }
	  }
	
    else  
	  {
	    y=new->father->father->lson;
	    if (y->color==RED)
	    {
	      new->father->color=BLACK;
	      y->color=BLACK;
	      new->father->father->color=RED;
	      new=new->father->father;
	    }
	    else
		  {
		    if (new==new->father->lson)
		    {
		      new=new->father;
		      right_rotate(tree,new);
		    }
		    new->father->color=BLACK;
		    new->father->father->color=RED;
		    left_rotate(tree,new->father->father);
		  
		  }
	  }
  }
  

  tree->root->color=BLACK;
  return 0;
  
}

/* used for fixing the tree, see refrence book at the top of the file for more details*/
int left_rotate(Tree_pt tree, Node* x)
{
  Node* NIL=tree->NIL;
  Node* y;
  y=x->rson;
  x->rson=y->lson;
  if (y->lson!=NIL)
    y->lson->father=x;
  y->father=x->father;
  if (x->father==NIL)
    tree->root=y;
  else
    {
      if (x==x->father->lson)
	x->father->lson=y;
      else
	x->father->rson=y;
    }
  y->lson=x;
  x->father=y;

  return 0;
}
  
/* used for fixing the tree, see refrence book at the top of the file for more details*/
int right_rotate(Tree_pt tree, Node* x)
{
  Node* y;
  Node* NIL=tree->NIL;
  y=x->lson;
  x->lson=y->rson;
  if (y->rson!=NIL)
    y->rson->father=x;
  y->father=x->father;
  if (x->father==NIL)
    tree->root=y;
  else
    {
      if (x==x->father->rson)
	x->father->rson=y;
      else
	x->father->lson=y;
    }
  y->rson=x;
  x->father=y;
  return 0;
}  

/* return an entry for which the compare function will return 0 when matched with the key entry.
return NULL if no such entry is found */
Entry_pt find_in_tree(Tree_pt tree, Entry_pt key)
{
  /* the algorithim is the clasic binary tree search, i.e. for smaller values try the left son. for larger
  try the right son. */
  Node* pos=tree->root;
  Node* NIL=tree->NIL;
  int diff=0;

  if (tree->pos==0)
    return NULL;
    
  diff=(*(tree->comp))(key,pos->entry);

  while (diff!=0)
    {
      
      if (diff<0)
	{
	  if (pos->lson==NIL ||pos->lson==NULL)
	    return NULL;
	  pos=pos->lson;
	}
      else {
	if (pos->rson==NIL || pos->rson==NULL)
	  return NULL;
	pos=pos->rson;
      }
      diff=(*(tree->comp))(key,pos->entry);
      
    }
  return pos->entry;
}
/* deletes the tree and frees the memory*/

void delete_tree(Tree_pt tree)
{
  Node** node;
  int i=0;
  
  node=tree->memory+tree->pos-1;
  
  for (i=tree->pos-1;i>=0;i--)
  { 
	  free(*node);
    if (i>0)
      node--;
  }
  free(tree->memory);
  free(tree);

}
      
