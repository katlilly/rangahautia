#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <cstdint>
#include "athtable.h"
#include "growablearray.h"


void *& Htable::operator[](const char *key)
{
  u_int32_t index;
  u_int32_t initial;

  initial = index = hash_string(key);
  do
      if (table[index].key == NULL)
	{
	  table[index].key = strdup(key);
	  return table[index].value;
	}
      else if (strcmp(key, table[index].key) == 0)
	return table[index].value;
      else
	  index = (index + 1) % table_size;

  while (index != initial);
 
  exit(printf("Hash table full\n"));
}


//void iterate_callback(char *key, void *value);
//void iterate(iterate_callback callback)



u_int32_t Htable::hash_string(const char *key)
{
  u_int32_t value = 0;
  for (const char *ch = key; *ch != '\0'; ch++)
      value = value * 32 + *ch;
  return value % table_size;
}


void Htable::print()
{
  printf("size: %d\n", table_size);
  for (int i = 0; i < table_size; i++)
      if (table[i].key != NULL)
	  printf("index: %d, key: %s\n", i, table[i].key);
}


void Htable::iterate()
{
  for (int i = 0; i < table_size; i++)
      if (table[i].key != NULL)
	  write(table[i].key, table[i].value);
}


void Htable::write(char *key, void *value)
{
  Growablearray *list = (Growablearray *) value;
  int length = list->itemcount;
  printf("key: %s, list length: %d\n", key, length);
}
