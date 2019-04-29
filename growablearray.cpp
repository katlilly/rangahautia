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


void Growablearray::print()
{
  printf("capacity: %d\n", capacity);
  printf("itemcount: %d\n", itemcount);
  printf("items: ");
  for (int i = 0; i < itemcount; i++)
    {
      printf("%d, ", items[i]);
    }
  printf("\n");
}
