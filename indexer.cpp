#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <cstdint>
#include "growablearray.h"
#include "tokeniser.h"
#include "athtable.h"

typedef struct {
  int start;
  int length;
} listpointer;

int main(void)
{

  /* 
     Read in data to be indexed
   */
  const char *filename = "../431searchengine/shortwsj.xml";
  FILE *fp = fopen(filename, "r");
  if (!fp)
    exit(printf("couldn't open file\n"));
  struct stat st;
  stat(filename, &st);
  char *input = (char *)malloc(st.st_size);
  fread(input, 1, st.st_size, fp);

  
  Growablearray ga;
  Tokeniser_no_whitespace tok;
  Tokeniser::slice token = tok.get_first_token(input, st.st_size);
  Htable ht = Htable(1000000);
  int docno = 0;

  
  /*
    Build the dictionary
   */
  while (token.length != 0)
    {
      if (!(*token.start == '<'))
	{
	  char *word = (char *) malloc(1024);
	  strncpy(word, token.start, token.length);
	  
	  word[token.length] = '\0';
	  
	  void *found = ht.find(word);
	  if (found)
	    {
	      Growablearray *pl = (Growablearray *) found;
	      
	      if (pl->items[(pl->itemcount)-2] == docno)
		{ 
		  pl->items[(pl->itemcount)-1]++;
		}
	      else
		{
		  pl->append(docno);
		  pl->append(1);
		}
	    }
	  else
	    { 
	      Growablearray *newpl = new Growablearray(docno);
	      ht.add(word, newpl);
	    }
	}
      else if (tok.compare("</DOC>"))
	{
	  docno++;
	}
      else if (tok.compare("<DOCNO>"))
	{
	  ; // std O string stream?
	}
      token = tok.get_next_token();
    }
  
  printf("doc count: %d\n", docno);
  free(input);
  fclose(fp);

  /*
    Write index to disk
   */
  //ht.iterate();
  FILE *vocabout = fopen("vocab.txt", "w");
  FILE *listpointers = fopen("listpointers.bin", "w");
  FILE *postingsout = fopen("postings.bin", "w");

  Growablearray *currentlist;
  
  int offset = 0;
  int length = 0;
  for (int i = 0; i < ht.table_size; i++)
    {
      if (ht.table[i].key != NULL)
	{
	  fputs(ht.table[i].key, vocabout);
	  fputs("\n", vocabout);
	  
	  currentlist = (Growablearray *) ht.table[i].value;
	  length = currentlist->itemcount;
	  for (int j = 0; j < length; j++)
	    putw(currentlist->items[j], postingsout);
	  putw(offset, listpointers);
	  putw(length, listpointers);
	  offset += length;
	}
    }
  fclose(listpointers);
  fclose(postingsout);
  fclose(vocabout);
  return 0;
}
