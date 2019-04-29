#include <bits/stdc++.h>
#include <stdio.h>
#include <sys/stat.h>
#include "tokeniser.h"


Tokeniser::slice Tokeniser_no_whitespace::get_next_token()
{
while (true)
  {
    slice token = Tokeniser::get_next_token();
    if (token.length == 0)
      return token;
    else if (isalnum(*token.start))
      return token;
    else if (*token.start == '<')
      return token;
  }
}


bool Tokeniser::compare(const char *string)
{
  return !(strncmp(current_token.start, string, current_token.length));
}


void Tokeniser::print_token(void)
{
  int len = current_token.length;
  //printf("%*.*s\n", len, len, current_token.start);
  int i;
  for (i = 0; i < len; i++)
    {
      printf("%c", tolower(current_token.start[i]));
    }
  printf("\n");
}


bool Tokeniser::belongs(char *token, int length)
{
  if (length == 0)
    return true;
  else if (isalpha(*token))
    return isalpha(token[length]);
  else if (isdigit(*token))
    return isdigit(token[length]);
  else if (*token == '<')
    return !(token[length - 1] == '>');
  else
    return false;
}


Tokeniser::slice Tokeniser::get_next_token()
{
  current_token.start = current;
  current_token.length = 0;
    
  if (*current == '\0')
    return current_token;

  int len = 0;
  while (belongs(current, len))
    len++;
  current_token.length = len;
  current += len;
  return current_token;
}


Tokeniser::slice Tokeniser::get_first_token(char *start, size_t length)
{
  current = start;
  document = start;
  document_length = length;

  return get_next_token();
}


// int main()
// {
//   Tokeniser_no_whitespace lex;
  
//   const char *filename = "wsj.xml";
//   FILE *fp = fopen(filename, "r");
//   struct stat st;
//   stat(filename, &st);
//   char *input = (char *)malloc(st.st_size);
//   fread(input, 1, st.st_size, fp);
  
//   Tokeniser::slice token = lex.get_first_token(input, st.st_size);
//   lex.print_token();

//   while (token.length != 0)
//     {
//       token = lex.get_next_token();
//       lex.print_token();
//     }

//   fclose(fp);
//   free(input);
//   return 0;
// }
