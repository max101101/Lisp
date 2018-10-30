#include <cstdio>
#include <cstring>
#include <lexem.hpp>
#include <lisp_lexer.hpp>

Lexer::Lexer()
{
  buf.buf = new char[32];
  buf.pos = 0;
  buf.len = 32;
  file = NULL;
  first = NULL;
  current = &first;
  splitter = " \r\n\t\'\"();";
}

Lexer::~Lexer()
{
  delete [] buf.buf;
}

Lexem* Lexer::start(FILE* input)
{
  file = input;
  while((c = getc(file)) != EOF){
    baseStatement();
  }
  return first;
}

int Lexer::cIsSplitter() const
{
  int i = 0;
  for(i = 0; i < (strlen(splitter)); ++i){
    if(c == splitter[i]){
      return 1;
    }
  }
  return 0;
}

void Lexer::increaseBuffer()
{
  int i = 0;
  char* str = new char[buf.len * 2];
  for(i = 0; i < buf.len; ++i){
    str[i] = buf.buf[i];
  }
  buf.len *= 2;
  delete [] buf.buf;
  buf.buf=str;
}

void Lexer::symbolToBuffer()
{
  if(buf.pos == buf.len){
    increaseBuffer();
  }
  buf.buf[buf.pos] = c;
  buf.pos++;
}

void Lexer::createLexem(statement type)
{
  int i = 0;
  (*current) = new Lexem;
  (*current)->word = new char[buf.pos+1];
  for(i = 0; i < buf.pos; ++i){
    ((*current)->word)[i] = buf.buf[i];
  }
  ((*current)->word)[buf.pos] = 0;
  (*current)->type = type;
  (*current)->next = NULL;
  buf.pos = 0;
  current = &((*current)->next);
}

void Lexer::baseStatement()
{
  if((c == ' ') || (c == '\t') || (c == '\n') || (c == '\r')){
    return;
  }
  if(c == '"'){
    stringStatement();
    return;
  }
  if(c == ';'){
    commentStatement();
    return;
  }
  if(cIsSplitter()){
    symbolToBuffer();
    createLexem(Splitter);
    return;
  }
  if(c == '#'){
	specialStatement();
  }
  wordStatement();
}

void Lexer::wordStatement()
{
  symbolToBuffer();
  for(;;){
    c = getc(file);
    if(c == EOF){
      createLexem(Word);
      return;
    }
    if(cIsSplitter()){
      ungetc(c, file);
      createLexem(Word);
      return;
    }
    symbolToBuffer();
  }
}

void Lexer::stringStatement()
{
  for(;;){
    c = getc(file);
    if((c == '"') || (c == EOF)){
      createLexem(String);
      break;
    }
    symbolToBuffer();
  }
}

void Lexer::commentStatement()
{
  for(;;){
    c = getc(file);
    if((c == '\n') || (c == EOF))
      break;
  }
}

void Lexer::specialStatement()
{
  symbolToBuffer();
  c = getc(file);
  symbolToBuffer();
  if(c == '\\'){
    c = getc(file);
    symbolToBuffer();
  }
  while(true){
      c = getc(file);
      if(c == EOF){
        break;
      }
      if(cIsSplitter()){
        ungetc(c, file);
		break;
      }
      symbolToBuffer();
  }
  createLexem(String);
}
