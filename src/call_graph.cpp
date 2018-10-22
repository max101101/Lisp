#include <cstdio>
#include <cstring>
#include <lexem.hpp>
#include <call_graph.hpp>

CallGraph::CallGraph()
{
	firstLexem = NULL;
	firstDefun = NULL;
	currentDefun = &firstDefun;
}

CallGraph::~CallGraph()
{
	deleteDefun();
	deleteLexem(firstLexem);
}

void CallGraph::create(Lexem* x)
{
	firstLexem = x;
	createDefun();
	printDefun();
}

void CallGraph::addDefun(char* function)
{
	int i = 0;
	int len = strlen(function);
	(*currentDefun) = new Defun;
	(*currentDefun)->function = new char[len+1];
	for(i = 0; i <= len; ++i){
		((*currentDefun)->function)[i] = function[i];
	}
	(*currentDefun)->next = NULL;
	currentDefun = &((*currentDefun)->next);
}

void CallGraph::createDefun()
{
	Lexem* tmp = firstLexem;
	while(tmp){
		if(((strcmp(tmp->word, "defun")) == 0) && (tmp->type == Word)){
			tmp = tmp->next;
			if(tmp == NULL)
				break;
			addDefun(tmp->word);
		}
		tmp = tmp->next;
	}
}

void CallGraph::printDefun()
{
	Defun* tmp = firstDefun;
	while(tmp){
		printf("%s\n", tmp->function);
		tmp = tmp->next;
	}
}

void CallGraph::deleteDefun()
{
	while(firstDefun){
		Defun* tmp = firstDefun;
		firstDefun = firstDefun->next;
		delete [] tmp->function;
		delete tmp;
	}
	currentDefun = &firstDefun;
}
