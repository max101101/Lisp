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
	void create(Lexem* x);
private:
	void addDefun(char* function);
	void createDefun();
	void printDefun();
	void deleteDefun();
};

#endif
