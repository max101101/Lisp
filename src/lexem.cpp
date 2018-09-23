#include <cstdio>
#include <lexem.hpp>

void printLexem(Lexem *pointer)
{
  while(pointer){
    printf("%s  %d\n", pointer->word, pointer->type);
    pointer = pointer->next;
  }
}

void deleteLexem(Lexem *pointer)
{
  while(pointer){
    Lexem* tmp = pointer;
    pointer = pointer->next;
    delete [] tmp->word;
    delete tmp;
  }
}
