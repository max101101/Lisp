#ifndef LISP_LEXER_HPP
#define LISP_LEXER_HPP

#include <cstdio>
#include <lexem.hpp>

struct Buffer{
  char* buf;
  int pos;
  int len;
};

class Lexer{
  Buffer buf;
  int c;
  Lexem* first;
  Lexem** current;
  const char* splitter;
  char* file;
public:
  Lexer();
  ~Lexer();
  Lexem* start(char*);
private: 
  int cIsSplitter() const;
  void increaseBuffer();
  void symbolToBuffer();
  void createLexem(Statement);
  void baseStatement();
  void wordStatement();
  void stringStatement();
  void commentStatement();
  void specialStatement();
};

#endif
