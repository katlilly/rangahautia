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
#include "simple10avx.h"

#define NUMDOCS 173253

typedef struct {
  int start;
  int length;
} listpointer;


struct file_pointers
{
  FILE *postingsout;
  FILE *termsout;
  FILE *locationsout;
} files;

void callback(struct file_pointers &files, char *key, Growablearray &data)
{
  // write string to terms file 
  char end = '\n';
  fwrite(key, 1, strnlen(key, 128), files.termsout);
  fwrite(&end, 1, 1, files.termsout);

  // write (compressed) postings list to file
  uint32_t *clist = data.to_uint32_array();
  int length = data.itemcount;
  VBcompress compressor;
  uint8_t *encoded = new uint8_t [5 * length];
  int compressed_length = compressor.compress(encoded, clist, length);
  int offset = ftell(files.postingsout);
  fwrite(encoded, 1, compressed_length, files.postingsout);

  // write locations of postings lists to index
  fwrite(&offset, 4, 1, files.locationsout);
  fwrite(&compressed_length, 4, 1, files.locationsout);
}



/*
  Return largest value of two integers
*/
int max(int a, int b)
{
  return a > b ? a : b;
}


int main(int argc, char **argv)
{
  /* 
     Read in data to be indexed
  */
  if (argc < 2)
    exit(printf("provide filename as command line arg\n"));
  const char *filename = argv[1];
  FILE *fp = fopen(filename, "r");
  if (!fp)
    exit(printf("couldn't open file: \"%s\"\n", filename));
  struct stat st;
  stat(filename, &st);
  char *input = (char *)malloc(st.st_size);
  if (!fread(input, 1, st.st_size, fp))
    exit(printf("failed to read in file: \"%s\"\n", filename));

  Htable<Growablearray> ht = Htable<Growablearray>(1000000);
  Tokeniser_no_whitespace tok;
  Tokeniser::slice token = tok.get_first_token(input, st.st_size);
  std::ofstream primarykeys;
  primarykeys.open("data/primarykeys.txt");
  std::vector<int> doclengths;
  int docno = 1, doclength = 0;

  /*
    Build the dictionary
  */
  while (token.length != 0)
    {
      if (!(*token.start == '<'))
	{
	  doclength++;
	  char *word = tok.slice_to_lowercase_string();
	  Growablearray &found = ht[word];

	  if (found.itemcount > 0 && found.items[found.itemcount - 2] == docno)
	    found.items[found.itemcount - 1]++;
	  else
	    found.append_two(docno, 1);
	}
      else if (tok.compare("</DOC>"))
	{
	  docno++;
	  doclengths.push_back(doclength);
	  doclength = 0;
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

  FILE *doclengthsout = fopen("data/doclengths.bin", "w");
  fwrite(&docno, sizeof(int), 1, doclengthsout);
  for (int i = 0; i < docno; i++)
    fwrite(&doclengths[i], sizeof(int), 1, doclengthsout);
    
  fclose(doclengthsout);
  free(input);
  fclose(fp);
  
  /*
    Write index to disk
   */
  FILE *postingsout = fopen("data/postings.bin", "w");
  FILE *termsout = fopen("data/terms.bin", "w");
  FILE *locationsout = fopen("data/locations.bin", "w");

  for (const auto &term : ht)
    {
      char *key = term.first;
      Growablearray &data = term.second;
      
      // write string to terms file 
      char end = '\n';
      fwrite(key, 1, strnlen(key, 128), termsout);
      fwrite(&end, 1, 1, termsout);

      // write (compressed) postings list to file
      uint32_t *clist = data.to_uint32_array();
      int length = data.itemcount;
      //VBcompress compressor;
      //uint8_t *encoded = new uint8_t [5 * length];
      //int compressed_length = compressor.compress(encoded, clist, length);
      //int offset = ftell(postingsout);
      //fwrite(encoded, 1, compressed_length, postingsout);
      
      Simple10avx *compressor = new Simple10avx();
      uint32_t *encoded = new uint32_t[max(16, length)];
      uint8_t *selectors = new uint8_t[length];
      uint32_t num_dgaps_compressed;
      num_dgaps_compressed = compressor->encode(encoded, clist, clist+length, selectors);
      int compressed_length = compressor->num_compressed_512bit_words;
      int offset = ftell(postingsout);
      fwrite(selectors, 1, compressed_length, postingsout);
      fwrite(encoded, 64, compressed_length, postingsout);
 
      // write locations of postings lists to index
      fwrite(&offset, 4, 1, locationsout);
      fwrite(&compressed_length, 4, 1, locationsout);
    }
 
  fclose(postingsout);
  fclose(locationsout);
  fclose(termsout);

  return 0;
}
