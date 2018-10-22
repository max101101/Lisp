#ifndef CALL_GRAPH_HPP
#define CALL_GRAPH_HPP

#include <lexem.hpp>

struct Defun{
	char* function;
	Defun* next;
};

class CallGraph{
	Lexem* firstLexem;
	Defun* firstDefun;
	Defun** currentDefun;
public:
	CallGraph();
	~CallGraph();
	void create(Lexem*);
private:
	void addDefun(char*);
	void createDefun();
	void printDefun();
	void deleteDefun();
};

#endif
