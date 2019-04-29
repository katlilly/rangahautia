#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <cstdint>
#include "growablearray.h"
#include "tokeniser.h"
#include "athtable.h"


int main(void)
{
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

  Htable ht = Htable(10);
  //Growablearray plist;
  
  ht.print();
  
  int docno = 0;
    
  while (token.length != 0)
    {
      if (!(*token.start == '<'))
	{
	  char *word = (char *) malloc(1024);
	  strncpy(word, token.start, token.length);
	  word[token.length] = '\0';
	  void *found = ht.find(word);
	  if (found)
	    printf("found %s\n", word);
	  else
	    {
	      printf("didn't find %s, adding it now\n", word);
	      ht.add(word, NULL);
	    }



	  
	  // add to or create postings list
	  // ht.add(token)
	  //tok.print_token();
	}
      else if (tok.compare("</DOC>"))
	{
	  docno++;
	}
      else if (tok.compare("<DOCNO>"))
	{
	  ;
	}
      
      token = tok.get_next_token();
    }
  
  
  printf("doc count: %d\n", docno);
  return 0;
}
