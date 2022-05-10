#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dictionary.h"
#include "RB_Tree.h"
#include "entry.h"

#define MIN_JUMP 5
#define START_SIZE 100

/* an entry object for the dictionery*/
typedef struct ENTRY
{
  char *key;
  void *value;
} Entry;


/* a dictionery object. conteins the red-black tree, and memory for the entry (only the entrys not the tree)
pos - the amount of memory used, size - the amount of memory allocated.
it also saves the set_value, get_value and protect value functions used, (see dictionary.h).
*/
struct Dictionary
{
  Tree_pt tree;
  Entry **memory;
  int pos;
  int size;
  set_value sv;
  delete_value dv;
  protect_value protect;
  void* current_protected;
};

/* this function is used for the tree object, 
it tells the tree how to compare objects when searching for an entry*/
int compare(Entry_pt left, Entry_pt right)
{
  return strcmp(left->key, right->key);
}

/* this is a constructer for the dictionary, a new dictionary musr be created using this function,
its inputs are the set_value, delete_value and protect_value functions. if the user does not wish
to protect the value he mey put in NULL istead*/
Dictionary_pt get_new_dictionary(set_value sv, delete_value dv, protect_value protect)
{
  Dictionary_pt dictionary = (Dictionary_pt)malloc(sizeof(struct Dictionary));
  if (dictionary == NULL)
    return NULL;

  dictionary->memory = (Entry **)malloc(START_SIZE * sizeof(Entry *));
  if (dictionary->memory == NULL)
    return NULL;
    
  dictionary->size = START_SIZE;
  dictionary->pos = 0;

  dictionary->sv = sv;
  dictionary->dv = dv;

  dictionary->protect = protect;

  dictionary->tree = new_tree(&compare);

  dictionary->current_protected=NULL;

  return dictionary;

  
}


/* inserts a value into the dictionary, recives a dictionary pointer, a key an a parameter which will be
used to create the object using the set_value function. return a negetive number if there is an error*/
int insert_to_dictionary(Dictionary_pt dictionary, char *key, void *value_params)
{

  Entry_pt new_entry;

  if (dictionary == NULL || dictionary->memory == NULL)
    return -1;

  /* if out of memory */
  if (dictionary->pos == dictionary->size)
  {
    if ((dictionary->memory = (Entry **)realloc(dictionary->memory, (dictionary->size + MIN_JUMP) * sizeof(Entry *))) == NULL)
      return -1;
    dictionary->size += MIN_JUMP;
  }

  /* add entry to memory */
  if (((*(dictionary->memory + dictionary->pos)) = ((Entry *)malloc(sizeof(Entry)))) == NULL)
    return -1;

  new_entry = *(dictionary->memory + dictionary->pos);
  dictionary->pos++;
  new_entry->key = (char *)malloc((strlen(key) + 1) * sizeof(char));
  strcpy(new_entry->key, key);

  if ((new_entry->value = (*(dictionary->sv))(value_params)) == NULL)
    return -3;

  /* add to tree*/
  RB_insert(dictionary->tree, new_entry);
  return 0;
}


/*searches for an object an the dictionary.
if not found will return NULL,
if found and protect_value is defined will return the reult of protect_value with the object found as the
parameter.
otherwise will return a pointer to the object*/ 
void *find_in_dictionary(Dictionary_pt dictionary, char *key)
{
  Entry key_entry;
  Entry_pt entry;
  key_entry.key=key;
  entry = find_in_tree(dictionary->tree, &key_entry);
  if (entry == NULL)
    return NULL;

  if (dictionary->protect == NULL)
      return entry->value;
  if (dictionary->current_protected!=NULL)
      (*(dictionary->dv))(dictionary->current_protected);
  dictionary->current_protected= (*(dictionary->protect))(entry->value);
  return dictionary->current_protected;
}

/*deletes the dictionary object and frees all the memory*/
int delete_dictionary(Dictionary_pt dictionary)
{

  Entry **entry;
  int i = 0;
  if (dictionary->current_protected!=NULL)
    {
      (*(dictionary->dv))(dictionary->current_protected);
      dictionary->current_protected=NULL;
    }
  entry = (dictionary->memory + dictionary->pos - 1);
 
  for (i = dictionary->pos - 1; i >= 0; i--)
  {
           

      (*(dictionary->dv))((*entry)->value);
      free(*entry);
    if (i!=0)
      entry--;
  }
  free(dictionary->memory);
  delete_tree(dictionary->tree);
  free(dictionary);
  return 0;
}

/* saves the dictionary to a file in the following foramt:
key to_string(object)\n
the to_string function must be provided by the user, see above*/

int save_dictionary_to_file(Dictionary_pt dictionary, FILE *file, value_to_string to_string)
{
  char string[200];
  char value[100];
  int i = 0, j = 0;
  Entry_pt entry;
  if (to_string == NULL)
    return -1;
  for (j = 0; j < dictionary->pos; j++)
  {
    entry = *(dictionary->memory + j);
    i=0;
    while (*(entry->key + i) != '\0')
      {
	string[i] = *(entry->key + i);
	i++;
      }
    string[i++] = ' ';
    to_string(entry->value, value);
    strcpy(&string[i], value);
    fprintf(file, "%s\n", string);
  }
  return 0;
}

/* loads a dictionary from a file, the file must be a text file in the following format:
key paramters\n
the parametes are interperted by the from_string function provided by the user
note that this function must not be called before the get_new_dictionery function.
*/

int load_dictionary_from_file(Dictionary_pt dictionary, FILE *file, value_from_string from_string)
{
  char key[100];
  char value[200];
  char line[301];
  int i = 0, j = 0;
  while (fgets(line, 300, file) != NULL)
  {
    i=0;
    j=0;
    while (line[i] != ' ')
    {
      if (line[i] == '\0')
        return -1;
      key[i] = line[i];
      i++;
    }
    key[i]='\0';

    while (line[i] != '\0')
    {
      value[j] = line[i];
      i++;
      j++;
    }
    value[j]='\0';
  
  insert_to_dictionary(dictionary,key,(*from_string)(value));
  }
  return 0;
}
