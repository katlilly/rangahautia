#include <bits/stdc++.h>
#include <stdio.h>
#include "growablearray.h"


void Growablearray::append(int item)
{
  if (itemcount == capacity)
    {
      capacity *= 2;
      items = (int *) realloc(items, capacity * sizeof(*items));
    }
  items[itemcount++] = item;
}


void Growablearray::append_two(int item1, int item2)
{
  if (itemcount == capacity)
    {
      capacity *= 2;
      items = (int *) realloc(items, capacity * sizeof(*items));
    }
  items[itemcount++] = item1;
  
  if (itemcount == capacity)
    {
      capacity *= 2;
      items = (int *) realloc(items, capacity * sizeof(*items));
    }
  items[itemcount++] = item2;
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
