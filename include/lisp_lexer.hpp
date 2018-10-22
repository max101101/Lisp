#ifndef LISP_LEXER_HPP
#define LISP_LEXER_HPP

#include <cstdio>
#include <lexem.hpp>

struct Buffer{
  char *buf;
  int pos;
  int len;
};

class Lexer{
  Buffer buf;
  int c;
  Lexem* first;
  Lexem** current;
  const char* splitter;
  FILE* file;
public:
  Lexer();
  ~Lexer();
  Lexem* start(FILE*);
private: 
  int cIsSplitter();
  void increaseBuffer();
  void symbolToBuffer();
  void createLexem(statement);
  void baseStatement();
  void wordStatement();
  void stringStatement();
  void commentStatement();
};

#endif
