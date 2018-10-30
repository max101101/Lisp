#ifndef CALL_GRAPH_HPP
#define CALL_GRAPH_HPP

#include <lexem.hpp>

struct Defun{
	char* function;
	int number;
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
	void addDefun(const char*, int);
	int paramDefun(Lexem*, bool) const;
	void createDefun();
	void printDefun() const;
	void deleteDefun();
};

#endif
