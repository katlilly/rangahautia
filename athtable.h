#pragma once

#include <iostream>


template <typename DATA>
class Htable
{
 public:
   typedef DATA value_type;
 
 private:
  struct entry {
    char *key;
    DATA value;
  } ;

  class iter
  {
  public:
    int i;
    Htable &source;
    
  public:
  iter(Htable &source, int i): i(i), source(source)
    {
      // nothing
    }
    
    auto operator*()
    {
      return std::pair<char *, DATA &>(source.table[i].key, source.table[i].value);
    }

    bool operator!=(iter &a)
    {
      return i != a.i;
    }

    iter &operator++(void)
    {
	for (i++; i < source.table_size; i++)
	  if (source.table[i].key != NULL)
	    return *this;
	
	return *this;
      }
  };

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

  auto begin(void)
  {
    for (int i = 0; i < table_size; i++)
      if (table[i].key != NULL)
	return iter(*this, i);

    return end();
  }

  auto end(void)
  {
    return iter(*this, table_size);
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

  static void unittest(void)
  {
    Htable<int> table(0x100);

    table["a"] = 'a';
    table["z"] = 'z';

    for (const auto &element : table)
      std::cout << element.first << " = " << element.second << std::endl;
  }
 
};
