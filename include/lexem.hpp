#ifndef LEXEM_HPP
#define LEXEM_HPP

enum Statement{
  WORD,
  SPLITTER,
  STRING
};

struct Lexem{
  char* word;
  Statement type;
  Lexem* next;
};

void printLexem(const Lexem*, FILE*);
void deleteLexem(Lexem*);

#endif
