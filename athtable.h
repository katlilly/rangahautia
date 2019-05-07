#ifndef ATHTABLE_H_
#define ATHTABLE_H_


template <typename DATA>
class Htable
{
 private:
  struct entry {
    char *key;
    DATA value;
  } ;

 private:
  int table_size;
  entry *table;
  
 private:
  
  u_int32_t hash_string(const char *key)
  {
    u_int32_t value = 0;
    for (const char *ch = key; *ch != '\0'; ch++)
      value = value * 32 + *ch;
    return value % table_size;
  }

  
 public:
  
  Htable(u_int32_t size)
    {
      table_size = size;
      table = new entry[size];
      for (u_int32_t i = 0; i < size; i++)
	  table[i].key = NULL;
    }

  virtual ~Htable()
    {
      for (int i = 0; i < table_size; i++)
      	  free(table[i].key);
      delete [] table;
    }

  int get_table_size(void)
    {
      return table_size;
    }
  
  DATA &operator[](const char *key)
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

template <typename SOMETHING>
  void iterate(SOMETHING &thing, void (*callback)(SOMETHING &appdata, char *name, DATA &value))
  {
    for (int index = 0; index < table_size; index++)
      if (table[index].key != NULL)
	callback(thing, table[index].key, table[index].value);
  }
  
};

#endif
