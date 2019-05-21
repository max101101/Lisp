#pragma once

enum Statement{
  WORD1,
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
