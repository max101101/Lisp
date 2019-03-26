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

struct Result{
	char* src_name;
	char* dst_name;
	double prob;
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
	vector<Result> Compare(Function*, int, double);
	void printGraph() const;
private:
	void initGraph();
	Lexem* skipQuote(Lexem*);
	void addCall(const char*, int);
	void parseFunction(int, Lexem*, bool);
	double paramStartProb(int);
	double callStartProb(int);
	double funcProb(FunctionCall, FunctionCall);
	Result funcCompare(Function, Function);
	void createGraph();
	void addDefun(const char*, int);
	int paramDefun(Lexem*, bool) const;
	void createDefun();
	int findDefun(const char*);
	void printDefun() const;
	void deleteDefun();
	void deleteGraph();
};

#endif
