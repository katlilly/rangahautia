#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <math.h>
#include <stdint.h>
#include "athtable.h"
#include "growablearray.h"

#define IDLEN 15
#define MAPSIZE 1000000
#define MAXTERMLEN 1024
#define TOKLEN 1024
#define MAXQLEN 1024 * 1024
#define MAXQTERMS 1024
#define NUMDOCS 173254


char *primarykeys;
int *postings;
char *vocablist;


typedef struct
{
  int offset;
  int listlength;
} whereplist;


struct listpointer {
  int start;
  int length;
};

typedef struct
{
  int docid;
  double rsv;
} result;


struct hashmaprec
{
  int capacity;
  int num_keys;
  char **keys;
  int *offsets;
  int *listlengths;
  
};


size_t getFilesize(const char* filename)
{
  struct stat st;
  stat(filename, &st);
  return st.st_size;
}


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
  int index = docnum - 1;
  char *name = (char *) malloc(IDLEN);
  int offset = index * (IDLEN-1);
  for (int i = 0; i < IDLEN-1; i++) {
    name[i] = *(primarykeys + i + offset);
  }
  name[IDLEN-1] = '\0';
  return name;
}


int main(void)
{
  size_t starts_size = getFilesize("starts.bin");
  int fd = open("starts.bin", O_RDONLY, 0);
  int *starts = (int *) mmap(NULL, starts_size, PROT_READ, MAP_PRIVATE, fd, 0);
  
  size_t lengths_size = getFilesize("lengths.bin");
  fd = open("lengths.bin", O_RDONLY, 0);
  int *lengths = (int *) mmap(NULL, lengths_size, PROT_READ, MAP_PRIVATE, fd, 0);
  
  //size_t pkeys_size = getFilesize("primarykeys.txt");
  //fd = open("primarykeys.txt", O_RDONLY, 0);
  //primarykeys = (char *) mmap(NULL, pkeys_size, PROT_READ, MAP_PRIVATE, fd, 0);

  size_t index_size = getFilesize("postings.bin");
  fd = open("postings.bin", O_RDONLY, 0);
  postings = (int *) mmap(NULL, index_size, PROT_READ, MAP_PRIVATE, fd, 0);


  /*
    build the hashmap of the vocab with pointers to postings lists
   */
  //hashmap vocabmap = hashmap_new(MAPSIZE);
  Htable ht = Htable(MAPSIZE);
  FILE *vocab = fopen("vocab.txt","r");
  char buffer[MAXTERMLEN];
  int vcount = 0;
  while (fgets(buffer, TOKLEN, vocab) != NULL)
    {
      int bufflen = strlen(buffer);
      buffer[bufflen-1] = '\0';
      //int start = starts[vcount];
      //int length = lengths[vcount];
      listpointer lp;
      //printf("start: %d, length: %d\n", starts[vcount], lengths[vcount]);
      lp.start = starts[vcount];
      lp.length = lengths[vcount];
      ht.add(buffer, &lp);
      //hashmap_insert(vocabmap, buffer, start, length);
      vcount++;
    }


  /* 
     evaluate queries
   */
  int i;
  char query[MAXQLEN];
  char **queryterms = (char **) malloc(MAXQTERMS * sizeof(*queryterms));
  char *searchterm;
  result *results = (result *) malloc(NUMDOCS * sizeof(*results));
  int docid;

  while (fgets(query, MAXQLEN, stdin))
    {
      for (i = 0; i < NUMDOCS; i++)
	{
	  results[i].docid = i;
	  results[i].rsv = 0;
	}
      int foundcount = 0;
      searchterm = strtok(query, " \n");
      while (searchterm)
	{
	  Growablearray *pl = (Growablearray *) ht.find(searchterm);
	  //int index = hashmap_search(vocabmap, searchterm);
	  if (pl)
	    {
	      foundcount++;
	      
	      printf("yes\n");
	    }
	  else
	    {
	      printf("no\n");
	    }
	  //}

	  // if (index > 0)
	  //   {
	  //     foundcount++;
	  //     int length = vocabmap->listlengths[index];
	  //     int start = vocabmap->offsets[index];
	  //     double idf = log((NUMDOCS*2)/length);
	  //     for (int i = 0; i < length; i++)
	  // 	{
	  // 	  if (i%2==0)
	  // 	    {
	  // 	      docid = postings[i+start];
	  // 	    }
	  // 	  else
	  // 	    {
	  // 	      results[docid].rsv += (idf * postings[i+start]);
	  // 	    }
	  // 	}
      //}
	  // get next term in current query
	  searchterm = strtok(NULL, " \n");
	} // end current query

      
      //have now added contribution of each term to rsv accumulators
      if (foundcount > 0)
	{
	  qsort(results, NUMDOCS, sizeof(*results), compare_rsvs);
	  for (int i = 0; i < NUMDOCS; i++)
	    {
	      if (results[i].rsv == 0)
		break;
	      printf("%s %2.2f\n", get_pkey(results[i].docid), results[i].rsv);
	    }
	  printf("\n");
	}
      
    } // end of queries

  return 0;
}
