#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <cstdint>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include "growablearray.h"
#include "tokeniser.h"
#include "athtable.h"
#include "vbyte_compress.h"

#define NUMDOCS 173253

typedef struct {
  int start;
  int length;
} listpointer;

int main(void)
{

  /* 
     Read in data to be indexed
   */

  const char *filename = "../431searchengine/wsj.xml";
  //const char *filename = "shortwsj.xml";
  FILE *fp = fopen(filename, "r");
  if (!fp)
    exit(printf("couldn't open file: \"%s\"\n", filename));
  struct stat st;
  stat(filename, &st);
  char *input = (char *)malloc(st.st_size);
  if (!fread(input, 1, st.st_size, fp))
    exit(printf("failed to read in file: \"%s\"\n", filename));

  std::ofstream primarykeys;
  primarykeys.open("primarykeys.txt");
  Growablearray ga;
  Tokeniser_no_whitespace tok;
  Tokeniser::slice token = tok.get_first_token(input, st.st_size);
  Htable ht = Htable(1000000);
  int docno = 1;
  int doclength = 0;
  char **identifiers = (char **) malloc(NUMDOCS * sizeof(*identifiers));
  std::vector <int> doclengths;

  //bool journal = false;
  /*
    Build the dictionary
   */
  while (token.length != 0)
    {
      if (!(*token.start == '<'))
	{
	  
	  doclength++;
	  char *word = tok.slice_to_lowercase_string();
	  //if (tok.compare("journal") || tok.compare("Journal") || tok.compare("JOURNAL"))
	  //  journal = true;
	  void *found = ht.find(word);
	  if (found)
	    {
	      Growablearray *pl = (Growablearray *) found;
	      if (pl->items[(pl->itemcount)-2] == docno)
		pl->items[(pl->itemcount)-1]++;
	      else
		pl->append_two(docno, 1);
	    }
	  else
	    { 
	      Growablearray *newpl = new Growablearray(docno);
	      ht.add(word, newpl);
	    }
	}
      else if (tok.compare("</DOC>"))
	{
	  //if (!journal)
	  //  printf("didn't find \"journal\" in this document: %d\n", docno);
	  //printf("docno: %d, length: %d\n", docno, doclength);
	  docno++;
	  doclengths.push_back(doclength);
	  doclength = 0;
	  //journal = false;
	}
      

      else if (tok.compare("<DOCNO>"))
	{
	  std::ostringstream primarykey;
	  token = tok.get_next_token();

	  while (!tok.compare("</DOCNO>"))
	    {
	      char *temp = tok.slice_to_string();
	      primarykey << temp;
	      token = tok.get_next_token();
	    }
	  
	  primarykeys << primarykey.str() << std::endl;
	}
      
      token = tok.get_next_token();
    }
  
  primarykeys.close();


  FILE *doclengthsout = fopen("doclengths.bin", "w");
  fwrite(&docno, sizeof(int), 1, doclengthsout);
  for (int i = 0; i < docno; i++)
    fwrite(&doclengths[i], sizeof(int), 1, doclengthsout);
    
  fclose(doclengthsout);
  free(input);
  fclose(fp);


  /*
    Write index to disk
   */
  FILE *postingsout = fopen("postings.bin", "w");
  FILE *termsout = fopen("terms.bin", "w");
  FILE *locationsout = fopen("locations.bin", "w");
    
  Growablearray *currentlist;
  int offset = 0;
  int length = 0;
  char end = '\n';
  for (int i = 0; i < ht.table_size; i++)
    {
      if (ht.table[i].key != NULL)
	{
	  // write string to terms file 
	  fwrite(ht.table[i].key, 1, strlen(ht.table[i].key), termsout);
	  fwrite(&end, 1, 1, termsout);

	  // write (compressed) postings list to file
	  currentlist = (Growablearray *) ht.table[i].value;
	  uint32_t *clist = currentlist->to_uint32_array();
	  length = currentlist->itemcount;
	  VBcompress *compressor = new VBcompress();
	  uint8_t *encoded = new uint8_t [5 * length];
	  int compressed_length = compressor->compress(encoded, clist, length);
	  fwrite(encoded, 1, compressed_length, postingsout);
 
	  // write locations of postings lists to index
	  fwrite(&offset, 4, 1, locationsout);
	  fwrite(&compressed_length, 4, 1, locationsout);
	  offset += compressed_length;
	  
	}
    }

  fclose(postingsout);
  fclose(locationsout);
  fclose(termsout);

  return 0;
}
