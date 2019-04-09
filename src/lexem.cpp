#include <cstdio>
#include <lexem.hpp>

void printLexem(const Lexem* pointer, FILE* stream)
{
  while(pointer){
    fprintf(stream, "%s  %d\r\n", pointer->word, pointer->type);
    pointer = pointer->next;
  }
}

void deleteLexem(Lexem* pointer)
{
  while(pointer){
    Lexem* tmp = pointer;
    pointer = pointer->next;
    delete [] tmp->word;
    delete tmp;
  }
}
