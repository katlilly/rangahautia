#ifndef ARRAY_H_
#define ARRAY_H_

class Array
{
 public:
  int *items;
  int size;

  
  Array(int *a=NULL, int s)
    {
      size = s;
      items = NULL;
      if (s != 0)
	{
	  items = new int[s];
	  for (int i = 0; i < s; i++)
	    items[i] = a[i];
	}
    }

 public:
  int & operator[](int index);
  void print() const;
  
};

#endif
