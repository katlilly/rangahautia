#include <bits/stdc++.h>
#include <stdio.h>
#include "growablearray.h"


void Growablearray::append(int item)
{
  if (itemcount == capacity)
    {
      capacity *= 2;
      int *temp = (int *) realloc(items, capacity * sizeof(*items));
      if (temp)
	items = temp;
      else
	{
	  free(items);
	  exit(printf("realloc failed while appending to postings list\n"));
	}
    }
  items[itemcount++] = item;
}


void Growablearray::append_two(int item1, int item2)
{
  append(item1);
  append(item2);
}


void Growablearray::print()
{
  printf("capacity: %d, ", capacity);
  printf("itemcount: %d, ", itemcount);
  printf("items: ");
  for (int i = 0; i < itemcount; i++)
    {
      printf("%d, ", items[i]);
    }
  printf("\n");
}


uint32_t * Growablearray::to_uint32_array()
{
  uint32_t *result = new uint32_t [itemcount];
  for (int i = 0; i < itemcount; i++)
    {
      result[i] = items[i];
    }
  return result;
}
