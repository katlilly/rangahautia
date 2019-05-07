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


 struct file_pointers
  {
    FILE *postingsout;
    FILE *termsout;
    FILE *locationsout;
  } files;



void callback(struct file_pointers &files, char *key, Growablearray &data)
{
  //int offset = 0;
  

  // write string to terms file 
  char end = '\n';
  fwrite(key, 1, strlen(key), files.termsout);
  fwrite(&end, 1, 1, files.termsout);

  // write (compressed) postings list to file
  uint32_t *clist = data.to_uint32_array();

  // if (0 == strcmp(key, "rosenfield") || 0 == strcmp(key, "italy") )
  //   {
  //     printf("%s ", key);
  //     printf(" %d\n", data.itemcount);
  //     for (int i = 0; i < data.itemcount; i++)
  // 	printf("%d, ", clist[i]);
  //     printf("\n");
  //   }
  // postings list correct at this point

  
  int length = data.itemcount;
  VBcompress compressor;
  uint8_t *encoded = new uint8_t [5 * length];
  int compressed_length = compressor.compress(encoded, clist, length);
  fwrite(encoded, 1, compressed_length, files.postingsout);
 
  // write locations of postings lists to index
  int offset = ftell(files.locationsout);
  fwrite(&offset, 4, 1, files.locationsout);
  fwrite(&compressed_length, 4, 1, files.locationsout);
  //offset += length;
 }


int main(void)
{
  /* 
     Read in data to be indexed
   */
  const char *filename = "../431searchengine/wsj.xml";
  FILE *fp = fopen(filename, "r");
  if (!fp)
    exit(printf("couldn't open file: \"%s\"\n", filename));
  struct stat st;
  stat(filename, &st);
  char *input = (char *)malloc(st.st_size);
  if (!fread(input, 1, st.st_size, fp))
    exit(printf("failed to read in file: \"%s\"\n", filename));

  std::ofstream primarykeys;
  primarykeys.open("data/primarykeys.txt");
  Tokeniser_no_whitespace tok;
  Tokeniser::slice token = tok.get_first_token(input, st.st_size);
  Htable<Growablearray> ht = Htable<Growablearray>(1000000);
  
  int docno = 1;
  int doclength = 0;
  char **identifiers = (char **) malloc(NUMDOCS * sizeof(*identifiers));
  std::vector<int> doclengths;

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

	  if (found.itemcount >= 2 && found.items[found.itemcount - 2] == docno)
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


  struct file_pointers files;
  
  /*
    Write index to disk
   */
  files.postingsout = fopen("data/postings.bin", "w");
  files.termsout = fopen("data/terms.bin", "w");
  files.locationsout = fopen("data/locations.bin", "w");

  ht.iterate(files, callback);
  
  fclose(files.postingsout);
  fclose(files.locationsout);
  fclose(files.termsout);

  return 0;
}
