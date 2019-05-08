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
#include "athtable.h"
#include "vbyte_compress.h"


struct result { int docid; double rsv; };


class list_location
{
public:
  int start;
  int length;
  
public:
  list_location() : start(0), length(0)
  {
    // nothing
  }
		   
};


int compare_rsvs(const void *a, const void *b)
{
  result *ra = (result *) a;
  result *rb = (result *) b;
  double rsva = ra->rsv;
  double rsvb = rb->rsv;
  return rsva < rsvb ? 1 : rsva == rsvb ? 0 : -1;
}


int main(void)
{
  /*
    Read postings list locations
   */
  const char *filename = "data/locations.bin";
  FILE *fp = fopen(filename, "r");
  if (!fp)
    exit(printf("couldn't open file: \"%s\"\n", filename));
  struct stat st;
  stat(filename, &st);
  int num_elements = st.st_size / sizeof(list_location);
  list_location *locations = new list_location [num_elements];

  if (!fread(locations, 1, st.st_size, fp))
    exit(printf("failed to read in list locations\n"));
  fclose(fp);

  /* 
     Get list of document lengths (zeroth element is number of docs);
  */
  fp = fopen("data/doclengths.bin", "r");
  if (!fp)
    exit(printf("couldn't open doc lengths file\n"));
  stat("doclengths.bin", &st);
  int *doclengths = (int *) malloc(st.st_size);
  if (!fread(doclengths, 1, st.st_size, fp))
    exit(printf("failed to read in documentlengths\n"));
  fclose(fp);
  int num_docs_in_index = *doclengths;
  
  /* 
     Read in the document identifiers
   */
  fp = fopen("data/primarykeys.txt", "r");
  if (!fp)
    exit(printf("couldn't open file: \"primarykeys.txt\"\n"));
  char **primarykeys = (char **) malloc(num_docs_in_index * sizeof(*primarykeys));
  char identifier[1024];
  int doccount = 1;
  
  while (fgets(identifier, 1024, fp) != NULL)
    {
      identifier[strlen(identifier)-1] = '\0';
      primarykeys[doccount] = (char *) malloc(1024);
      strcpy(primarykeys[doccount++], identifier);
    }
    
  if (doccount != num_docs_in_index)
    exit(printf("Expected %d documents, but read in %d identifiers\n",
		num_docs_in_index-1, doccount-1));
  fclose(fp);

  /*
    Get the terms in the index and insert them into a hashmap with list locations as value
  */
  Htable<list_location> index(1000000);
  filename = "data/terms.bin";
  fp = fopen(filename, "r");
  if (!fp)
    exit(printf("couldn't open file: \"%s\"\n", filename));
  char buffer[1024];
  int termcount = 0;
  while (fgets(buffer, 1024, fp) != NULL)
    {
      buffer[strlen(buffer) - 1] = '\0';
      list_location &current = index[buffer];
      current.start = locations[termcount].start;
      current.length = locations[termcount].length;
      termcount++;
    }
  fclose(fp);
  
  /*
    Read in the postings lists
   */
  filename = "data/postings.bin";
  fp = fopen(filename, "r");
  if (!fp)
    exit(printf("couldn't open file: \"%s\"\n", filename));
  stat(filename, &st);
  uint8_t *postings = (uint8_t *) malloc(st.st_size);
  if (!fread(postings, 1, st.st_size, fp))
    exit(printf("failed to read in postings lists\n"));
  fclose(fp);


  /* 
     Receive queries
   */
  int i, docid;
  int foundcount = 0;
  char query[1024];
  char *searchterm = NULL;
  result *results = (result *) malloc(num_docs_in_index * sizeof(*results));
  
  while (fgets(query, 1024, stdin))
    {
      for (i = 0; i < num_docs_in_index; i++)
	{
	  results[i].docid = i;
	  results[i].rsv = 0;
	}
      
      foundcount = 0;
      searchterm = strtok(query, " \n");
      if (searchterm)
	for (uint i = 0; i < strlen(searchterm); i++)
	  searchterm[i] = tolower(searchterm[i]);
      
      while (searchterm)
	{
	  list_location &found = index[searchterm];
	  if (found.length != 0)
	    {
	      foundcount++;
	      uint32_t *thislist = (uint32_t *) malloc(found.length * sizeof(*thislist));
	      VBcompress decompressor;
	      int length = decompressor.decompress(thislist, postings + found.start, found.length);
	      double idf = log((num_docs_in_index) / (length/2));
	      double epsilon = 1e-5;
	      
	      for (i = 0; i < length; i++)
		    if (i % 2 == 0) docid = thislist[i];
		    else
		      {
			double tf = (double) thislist[i] / doclengths[docid];
			results[docid].rsv += ((epsilon + idf) * tf);
		      }
	      free(thislist);
	    }

	  searchterm = strtok(NULL, " \n");
	  if (searchterm)
	    for (uint i = 0; i < strlen(searchterm); i++)
	      searchterm[i] = tolower(searchterm[i]);
      	  
	} // end current query

      
      /* 
	 Sort and output results
      */
      if (foundcount > 0)
	{
	  qsort(results, num_docs_in_index, sizeof(*results), compare_rsvs);
	  for (i = 0; i < num_docs_in_index; i++)
	    {
	      if (results[i].rsv == 0)
		break;
	      printf("%s %.8f\n", primarykeys[results[i].docid], results[i].rsv);
	    }
	  printf("\n");
	}
     } // end of queries

  
  free(results);
  free(searchterm);
  free(postings);
  for (i = 1; i < num_docs_in_index; i++)
    free(primarykeys[i]);
  free(primarykeys);
  free(doclengths);
  delete [] locations;
  
  return 0;
}
