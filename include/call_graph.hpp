#ifndef CALL_GRAPH_HPP
#define CALL_GRAPH_HPP

#include <lexem.hpp>
#include <vector>

using std::vector;

struct Defun{
	char* function;
	int number;
	Defun* next;
};

enum FunctionType{
	SELF,
	USER,
	STANDART
};

struct FunctionCall{
	char* name;
	FunctionType type;
	int paramNumber;
	int callNumber;
};

struct Function{
	char* name;
	int paramNumber;
	vector<FunctionCall> list;
};

class CallGraph{
	Lexem* firstLexem;
	Defun* firstDefun;
	Defun** currentDefun;
	int sizeGraph;
	Function* Graph;
public:
	CallGraph();
	~CallGraph();
	void create(Lexem*);
	int GetSize();
	Function* GetGraph();
private:
	void initGraph();
	Lexem* skipQuote(Lexem*);
	void addCall(const char*, int);
	void parseFunction(int, Lexem*, bool);
	void createGraph();
	void addDefun(const char*, int);
	int paramDefun(Lexem*, bool) const;
	void createDefun();
	int findDefun(const char*);
	void printDefun() const;
	void printGraph() const;
	void deleteDefun();
	void deleteGraph();
};

#endif
