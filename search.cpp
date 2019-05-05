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

#define NUMDOCS 173253


struct list_location { int start; int length; };
struct result { int docid; double rsv; };


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
     Get list of document lengths (zeroth value is number of docs);
  */
  fp = fopen("doclengths.bin", "r");
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
  fp = fopen("primarykeys.txt", "r");
  if (!fp)
    exit(printf("couldn't open file: \"primarykeys.txt\"\n"));
  char **primarykeys = (char **) malloc(NUMDOCS * sizeof(*primarykeys));
  char identifier[1024];
  int doccount = 1;

  while (fgets(identifier, 1024, fp) != NULL)
    {
      identifier[strlen(identifier)-1] = '\0';
      primarykeys[doccount] = (char *) malloc(1024);
      strcpy(primarykeys[doccount++], identifier);
      //printf("%s\n", primarykeys[doccount-1]);
    }
  fclose(fp);
  
  /*
    Get the terms in the index and insert them into hash map with list locations as value
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

  
  int i, docid, foundcount;
  char query[1024];
  char **queryterms = (char **) malloc(100 * sizeof(*queryterms));
  char *searchterm;
  result *results = (result *) malloc(NUMDOCS * sizeof(*results));
  list_location *found;

  while (fgets(query, 1024, stdin))
    {
      for (i = 0; i < NUMDOCS; i++)
	{
	  results[i].docid = i;
	  results[i].rsv = 0;
	}
      
      foundcount = 0;
      searchterm = strtok(query, " \n");

      while (searchterm)
	{
	  found = (list_location *) index->find(searchterm);
	  if (found)
	    {
	      foundcount++;
	      int length = found->length;
	      
	      uint8_t *compressed_list = (uint8_t *) malloc(length);
	      for (int i = 0; i < length; i++)
		compressed_list[i] = postings[found->start + i];
	      
	      uint32_t *thislist = (uint32_t *) malloc(length * sizeof(*thislist));
	      VBcompress *decompressor = new VBcompress();
	      length = decompressor->decompress(thislist, compressed_list, length);
	      
	      double idf = log((NUMDOCS)/(length/2));
	      printf("found %s, list length: %d, idf: %.10f\n", searchterm, length, idf);
	      double epsilon = 1e-3;
	      
	      for (i = 0; i < length; i++)
		{
		  if (i % 2 == 0)
		    {
		      docid = thislist[i];
		    }
		  else
		    {
		      double tf = (double)thislist[i] / doclengths[docid];
		      results[docid].rsv += ((epsilon + idf) * tf);
		    }
		  
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
	  for (i = 0; i < NUMDOCS; i++)
	    {
	      if (results[i].rsv == 0)
		break;
	      printf("%s %2.4f\n", primarykeys[results[i].docid], results[i].rsv);
	    }
	  printf("\n");
	}
      
    } // end of queries
  

  //free(query);
  free(locations);
  return 0;
}
