#include <stdlib.h>
#include "list.h"

#define INIT_SIZE 100
#define EXTENTION_SIZE 5

typedef struct NODE {

struct NODE* previous;
struct NODE* next;
void* value;

}Node;

struct List{

Node* current_node;

void* current_protected;
  
Node** memory;
int size;
int pos;

set_value_list set;

delete_value_list delete;

protect_value_list protect;


};

/* this is a constructer for the list, a new list must be created using this function,
its inputs are the set_value, delete_value and protect_value functions. if the user does not wish
to protect the value he mey put in NULL istead*/
List_pt get_new_list(set_value_list sv,delete_value_list dv,protect_value_list protect)
{
    List_pt list=(List_pt)malloc(sizeof(struct List));
    list->set=sv;
    list->delete=dv;
    list->protect=protect;

    list->memory=(Node**)malloc(INIT_SIZE*sizeof(Node*));
    list->size=INIT_SIZE;
    list->pos=0;
    list->current_node=NULL;
    list->current_protected=NULL;
    return list;
}

/* inserts a value into the list, recives a list pointer and a parameter which will be
used to create the object using the set_value function. return a negetive number if there is an error*/
int insert_to_list(List_pt list,void* value_params)
{
    if (list->pos==list->size)
    {
         list->memory=(Node**)realloc(list->memory,(list->size+EXTENTION_SIZE)*sizeof(Node*));
         list->size+=EXTENTION_SIZE;
    }   
    *(list->memory+list->pos)=(Node*)malloc(sizeof(Node));
    if (list->pos > 0)
    {
        (*(list->memory+list->pos-1))->next=*(list->memory+list->pos);
        (*(list->memory+list->pos))->previous=*(list->memory+list->pos-1);
    }
    (*(list->memory+list->pos))->value=(*(list->set))(value_params);
    (*(list->memory+list->pos))->next=NULL;
    list->pos++;
    if (list->current_node==NULL)
        list->current_node=*(list->memory);
    return 0;
}

/*return the current object the list is pointing at (via protect_value), or NULL*/

void* get_current(List_pt list)
{
    if (list->pos==0)
        return NULL;
    if (list->current_node==NULL)
        return NULL; 
    if (list->protect==NULL)
        return list->current_node->value;
    if (list->current_protected==NULL)
      list->current_protected=(*(list->protect))(list->current_node->value);
    return list->current_protected;
}

/*moves the current_value to the next on the list*/

int Next(List_pt list)
{
    if (list->current_node==NULL)
        return -2;
    if (list->current_node==*(list->memory+list->pos-1))
    {
      if (list->current_protected!=NULL)
	{
	  (*(list->delete))(list->current_protected);
	  list->current_protected=NULL;
	}
        list->current_node=NULL;
        return -1;
    }
    if (list->current_protected!=NULL)
	{
	  (*(list->delete))(list->current_protected);
	  list->current_protected=NULL;
	}
    list->current_node=list->current_node->next;
    return 0;
}

/*moves the current value to the previous object*/

int previous(List_pt list)
{
    if (list->current_node==NULL)
        return -2;
    if (list->current_node==*(list->memory))
        return -1;
    list->current_node=list->current_node->previous;
    if (list->current_protected!=NULL)
      {
	(*(list->delete))(list->current_protected);
	list->current_protected=NULL;
      }
    return 0;
}

/*deletes the list, freeing the objects with delete_value*/

int delete_list(List_pt list)
{
    int i=0;
    if (list->current_protected!=NULL)
      {
	(*(list->delete))(list->current_protected);
	list->current_protected=NULL;
      }
    for (i=list->pos-1;i>=0;i--)
        {
            (*(list->delete))(((*(list->memory+i))->value));
            free(*(list->memory+i));
        }
    free(list->memory);
    return 0;
}

/*resets the current object to the start of the list*/ 

int reset_list_to_start(List_pt list)
{
    list->current_node=*(list->memory);
    if (list->current_protected!=NULL)
      {
	(*(list->delete))(list->current_protected);
	list->current_protected=NULL;
      }
    return 0;
}
