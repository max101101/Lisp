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

Lexem* Lexer::start(char* input)
{
  file = input;
  while((c = file[0]) != 0){
	file += 1;
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

void Lexer::createLexem(Statement type)
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
    createLexem(SPLITTER);
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
    c = file[0];
	file += 1;
    if(c == 0){
      createLexem(WORD1);
      return;
    }
    if(cIsSplitter()){
      file -= 1;
      createLexem(WORD1);
      return;
    }
    symbolToBuffer();
  }
}

void Lexer::stringStatement()
{
  for(;;){
    c = file[0];
	file += 1;
    if((c == '"') || (c == 0)){
      createLexem(STRING);
      break;
    }
    symbolToBuffer();
  }
}

void Lexer::commentStatement()
{
  for(;;){
    c = file[0];
	file += 1;
    if((c == '\n') || (c == 0))
      break;
  }
}

void Lexer::specialStatement()
{
  symbolToBuffer();
  c = file[0];
  file += 1;
  symbolToBuffer();
  if(c == '\\'){
    c = file[0];
	file += 1;
    symbolToBuffer();
  }
  while(true){
      c = file[0];
      file += 1;
      if(c == 0){
        break;
      }
      if(cIsSplitter()){
        file -= 1;
		break;
      }
      symbolToBuffer();
  }
  createLexem(STRING);
}
