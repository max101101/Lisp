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

void CallGraph::create(Lexem* first)
{
	firstLexem = first;
	createDefun();
	printDefun();
}

void CallGraph::addDefun(const char* function, int number)
{
	int i = 0;
	int len = strlen(function);
	(*currentDefun) = new Defun;
	(*currentDefun)->function = new char[len+1];
	for(i = 0; i <= len; ++i){
		((*currentDefun)->function)[i] = function[i];
	}
	(*currentDefun)->number = number;
	(*currentDefun)->next = NULL;
	currentDefun = &((*currentDefun)->next);
}

int CallGraph::paramDefun(Lexem* lexem, bool isSheme) const
{
	Lexem* tmp = lexem;
	int count = 0;
	int balance = 0;
	if(isSheme){
		balance = 1;
	}
	while(tmp){
		if(((strcmp(tmp->word, ")")) == 0) && (tmp->type == Splitter)){
			if(--balance == 0)
				return count;
		} else if(((strcmp(tmp->word, "(")) == 0) && (tmp->type == Splitter)){
			balance++;
		} else{
			if(tmp->word[0] != '&')
				count++;
		}
		tmp = tmp->next;
	}
	return count;
}

void CallGraph::createDefun()
{
	Lexem* tmp = firstLexem;
	while(tmp){
		if(((strcmp(tmp->word, "defun")) == 0) && (tmp->type == Word)){
			tmp = tmp->next;
			if(tmp == NULL)
				break;
			addDefun(tmp->word, paramDefun(tmp->next, false));
		}
		if(((strcmp(tmp->word, "define")) == 0) && (tmp->type == Word)){
			tmp = tmp->next;
			if(tmp == NULL)
				break;
			tmp = tmp->next;
			if(tmp == NULL)
				break;
			addDefun(tmp->word, paramDefun(tmp->next, true));
		}
		tmp = tmp->next;
	}
}

void CallGraph::printDefun() const
{
	Defun* tmp = firstDefun;
	while(tmp){
		printf("%s  %d\n", tmp->function, tmp->number);
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
