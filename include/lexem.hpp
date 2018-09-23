#ifndef LEXEM_HPP
#define LEXEM_HPP

enum statement{
  Word,
  Splitter,
  String
};

struct Lexem{
  char* word;
  statement type;
  Lexem* next;
};

void printLexem(Lexem* pointer);
void deleteLexem(Lexem* pointer);

#endif
