#ifndef TOKENISER_H_
#define TOKENISER_H_

class Tokeniser
{
private:
  char *document;
  size_t document_length;
  char *current;

public:
  struct slice {
    char *start;
    int length;
  } current_token;

private:
  bool belongs(char *token, int length);

public:
  void print_token(void);
  bool compare(const char *string);
  virtual slice get_first_token(char *start, size_t length);
  virtual slice get_next_token();
};

class Tokeniser_no_whitespace : public Tokeniser
{
public:
  virtual slice get_next_token();
};



#endif
