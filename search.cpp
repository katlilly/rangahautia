#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <stdint.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <bits/stdc++.h> 
//#include <inttypes.h>
#include "athtable.h"
#include "vbyte_decompress.h"

//#define NUMDOCS 51
#define NUMDOCS 173253

struct list_location
{
  int start;
  int length;
};

struct result
{
  int docid;
  double rsv;
};


int compare_rsvs(const void *a, const void *b)
{
  result *ra = (result *) a;
  result *rb = (result *) b;
  double rsva = ra->rsv;
  double rsvb = rb->rsv;
  return rsva < rsvb ? 1 : rsva == rsvb ? 0 : -1;
}

/* 
   takes a docnum and returns a wsj-number as a string
 */
char *get_pkey(int docnum)
{
  //int index = docnum - 1;
  //char *name = (char *) malloc(IDLEN);
  //int offset = index * (IDLEN-1);
  //for (int i = 0; i < IDLEN-1; i++) {
  //name[i] = *(primarykeys + i + offset);
  //}
  //name[IDLEN-1] = '\0';
  return NULL;
}




int main(void)
{


  /*
    Read postings list locations
   */
  const char *filename = "locations.bin";
  FILE *fp = fopen(filename, "r");
  if (!fp)
    exit(printf("couldn't open file: \"%s\"\n", filename));
  struct stat st;
  stat(filename, &st);
  int num_elements = st.st_size / sizeof(list_location);
  list_location *locations = (list_location *) malloc(st.st_size);
  if (!fread(locations, 1, st.st_size, fp))
    exit(printf("failed to read in list locations\n"));
  fclose(fp);

  /* 
     Read in the document identifiers
   */
  fp = fopen("primarykeys.txt", "r");
  if (!fp)
    exit(printf("couldn't open file: \"primarykeys.txt\"\n"));
  stat("primarykeys.txt", &st);
  char identifier[20];
  std::vector <std::string> identifiers; 

  while (fgets(identifier, 20, fp) != NULL)
    {
      printf("%s", identifier);
      identifiers.push_back(identifier);
    }
  fclose(fp);
  printf("%d\n", identifiers.size());
  //std::count << identifiers[2] << std::endl;
  //printf("%s",identifiers[2]);

  
  /*
    Get indexed terms and insert them into hash map with locations as value
  */
  Htable *index = new Htable(1000000);
  filename = "terms.bin";
  fp = fopen(filename, "r");
  if (!fp)
    exit(printf("couldn't open file: \"%s\"\n", filename));
  char buffer[1024];
  int termcount = 0;
  while (fgets(buffer, 1024, fp) != NULL)
    {
      buffer[strlen(buffer)-1] = '\0';
      list_location *current = new list_location;
      current->start = locations[termcount].start;
      current->length = locations[termcount].length;
      index->add(buffer, current);
      termcount++;
    }
  fclose(fp);

 
  
  /*
    Read in the postings lists
   */
  filename = "postings.bin";
  fp = fopen(filename, "r");
  if (!fp)
    exit(printf("couldn't open file: \"%s\"\n", filename));
  stat(filename, &st);
  uint8_t *postings = (uint8_t *) malloc(st.st_size);
  if (!fread(postings, 1, st.st_size, fp))
    exit(printf("failed to read in postings lists\n"));
  fclose(fp);
  

  /* 
     some tests
   */
  //index->print();
  // char *testquery = (char *) "blair";
  // list_location *found = (list_location *) index->find(testquery);
  // if (found)
  //   {
  //     int length = found->length;
  //     uint8_t *compressed_list = (uint8_t *) malloc(length);
  //     for (int i = 0; i < length; i++)
  // 	compressed_list[i] = postings[found->start + i];
  //     for (int i = 0; i < length; i++)
  // 	printf("%d ", compressed_list[i]);
  //     printf("\n");

  //     VBdecompressor *decompressor = new VBdecompressor(compressed_list, length);
  //     decompressor->decompress_array();
  //     uint32_t *thislist = decompressor->output;
  //     length = decompressor->output_length;
  //     for (int i = 0; i < length; i++)
  // 	printf("%d ", thislist[i]);
  //     printf("\n");
  //   }


  int i;
  int docid;
  char query[1024];
  char **queryterms = (char **) malloc(100 * sizeof(*queryterms));
  char *searchterm;
  result *results = (result *) malloc(NUMDOCS * sizeof(*results));

  while (fgets(query, 1024, stdin))
    {
      
      for (int i = 0; i < NUMDOCS; i++)
	{
	  results[i].docid = i;
	  results[i].rsv = 0;
	}
      
      int foundcount = 0;
      searchterm = strtok(query, " \n");

      while (searchterm)
	{
	  list_location *found = (list_location *) index->find(searchterm);
	  if (found)
	    {
	      foundcount++;
	      int length = found->length;
	      uint8_t *compressed_list = (uint8_t *) malloc(length);
	      for (int i = 0; i < length; i++)
		compressed_list[i] = postings[found->start + i];
	      VBdecompressor *decompressor = new VBdecompressor(compressed_list, length);
	      decompressor->decompress_array();
	      uint32_t *thislist = decompressor->output;
	      length = decompressor->output_length;
	      double idf = log((NUMDOCS)/(length/2));
	      printf("found %s, list length: %d, idf: %.10f\n", searchterm, length, idf);
	      double epsilon = 1e-5;
	      for (int i = 0; i < length; i++)
		{
		  if (i%2==0)
		    docid = thislist[i];
		  else
		    results[docid].rsv += ((epsilon +idf) * thislist[i]);
		}
	    }
	  searchterm = strtok(NULL, " \n");
	} // end current query

      
      /* 
	 Sort and output results
      */
      if (foundcount > 0)
	{
	  qsort(results, NUMDOCS, sizeof(*results), compare_rsvs);
	  for (int i = 0; i < NUMDOCS; i++)
	    {
	      if (results[i].rsv == 0)
		break;
	      printf("%6d %2.4f\n", results[i].docid, results[i].rsv);
	      //printf("%s %2.2f\n", get_pkey(results[i].docid), results[i].rsv);
	    }
	  printf("\n");
	}
      
    } // end of queries
  

  //free(query);
  free(locations);
  return 0;
}
