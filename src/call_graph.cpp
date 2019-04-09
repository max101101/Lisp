#include <cstdio>
#include <cstring>
#include <lexem.hpp>
#include <call_graph.hpp>

CallGraph::CallGraph()
{
	firstLexem = NULL;
	firstDefun = NULL;
	currentDefun = &firstDefun;
	sizeGraph = 0;
	Graph = NULL;
}

CallGraph::~CallGraph()
{
	deleteDefun();
	deleteGraph();
	deleteLexem(firstLexem);
}

void CallGraph::create(Lexem* first)
{
	firstLexem = first;
	createDefun();
	initGraph();
	createGraph();
	//printGraph();
}

int CallGraph::GetSize()
{
	return sizeGraph;
}

Function* CallGraph::GetGraph()
{
	return Graph;
}

double CallGraph::paramStartProb(int a)
{
	if(a < 0){
		a = 0 - a;
	}
	switch(a){
	case 0:
		return 1;
	case 1:
		return 0.8;
	default:
		return 0;
	}
}

double CallGraph::callStartProb(int a)
{
	if(a < 0){
		a = 0 - a;
	}
	switch(a){
	case 0:
		return 1;
	case 1:
		return 0.9;
	case 2:
		return 0.7;
	default:
		return 0;
	}
}

double CallGraph::funcProb(FunctionCall src, FunctionCall dst)
{
	if(src.type != dst.type){
		return 0;
	}
	if(src.type == STANDART){
		if(strcmp(src.name, dst.name) != 0){
			return 0;
		}
	}
	double startProb = paramStartProb(src.paramNumber - dst.paramNumber);
	double a = src.callNumber;
	double b = dst.callNumber;
	double mult = a < b ? a/b : b/a;
	return startProb*mult;
}

Result CallGraph::funcCompare(Function src, Function dst)
{
	Result r;
	r.src_name = src.name;
	r.dst_name = dst.name;
	r.prob = 0;
	double startProb = paramStartProb(src.paramNumber - dst.paramNumber);
	double callProb = callStartProb(src.list.size() - dst.list.size());
	startProb *= callProb;
	startProb /= src.list.size();
	for(int i = 0; i < src.list.size(); i++){
		double maxProb = 0;
		for(int j = 0; j < dst.list.size(); j++){
			double res = funcProb(src.list[i], dst.list[j]);
			maxProb = res > maxProb ? res : maxProb;
		}
		r.prob += maxProb * startProb;
	}
	return r;
}

vector<Result> CallGraph::Compare(Function* src, int size, double threshold)
{
	vector<Result> res;
	for(int i = 0; i < size; i++){
		Result r;
		r.prob = 0;
		for(int j = 0; j < sizeGraph; j++){
			Result tmp = funcCompare(src[i], Graph[j]);
			if(tmp.prob > r.prob){
				r = tmp;
			}	
		}
		if(r.prob > threshold){
			res.push_back(r);
		}
	}
	return res;
}

void CallGraph::initGraph()
{
	Defun* tmp = firstDefun;
	Graph = new Function[sizeGraph];
	for(int i = 0; i < sizeGraph; i++){
		int len = strlen(tmp->function);
		Graph[i].name = new char[len+1];
		strcpy(Graph[i].name, tmp->function);
		Graph[i].paramNumber = tmp->number;
		tmp = tmp->next;
	}
}

void CallGraph::addCall(const char* name, int i)
{
	for(int j = 0; j < Graph[i].list.size(); j++){
		if(strcmp(Graph[i].list[j].name, name) == 0){
			Graph[i].list[j].callNumber++;
			return;
		}
	}
	bool isMe = strcmp(name, Graph[i].name) == 0;
	int paramNumber = findDefun(name);
	FunctionCall func;
	func.paramNumber = paramNumber;
	func.callNumber = 1;
	if(isMe){
		func.type = SELF;
	} else if(paramNumber != -1){
		func.type = USER;
	} else {
		func.type = STANDART;
	}
	int len = strlen(name);
	func.name = new char[len+1];
	strcpy(func.name, name);
	Graph[i].list.push_back(func);
}

Lexem* CallGraph::skipQuote(Lexem* lexem)
{
	int balance = 0;
	while(lexem){
		if(((strcmp(lexem->word, ")")) == 0) && (lexem->type == SPLITTER)){
			if(--balance == 0){
				return lexem;
			}
		} else if(((strcmp(lexem->word, "(")) == 0) && (lexem->type == SPLITTER)){
			balance++;
		}
		lexem = lexem->next;
	}
	return NULL;
}

void CallGraph::parseFunction(int i, Lexem* lexem, bool isScheme)
{
	Lexem* tmp = lexem;
	int balance = 0;
	if(isScheme){
		balance = 1;
	}
	//go to body
	while(tmp){
		if(((strcmp(tmp->word, ")")) == 0) && (tmp->type == SPLITTER)){
			if(--balance == 0){
				tmp = tmp->next;
				break;
			}
		} else if(((strcmp(tmp->word, "(")) == 0) && (tmp->type == SPLITTER)){
			balance++;
		}
		tmp = tmp->next;
	}
	//body
	while(tmp){
		if(((strcmp(tmp->word, ")")) == 0) && (tmp->type == SPLITTER)){
			if(--balance == 0)
				return;
		} else if(((strcmp(tmp->word, "(")) == 0) && (tmp->type == SPLITTER)){
			balance++;
			if(tmp->next == NULL)
				return;
			if(tmp->next->type == WORD){
				addCall(tmp->next->word, i);
			}
		} else if((((strcmp(tmp->word, "'")) == 0) && (tmp->type == SPLITTER))||
				(((strcmp(tmp->word, "quote")) == 0) && (tmp->type == WORD))){
			addCall("quote", i);
			tmp = skipQuote(tmp->next);
			balance++;
			continue;
		}
		tmp = tmp->next;
	}
}

void CallGraph::createGraph()
{
	Lexem* tmp = firstLexem;
	int i = 0;
	while(tmp){
		if(((strcmp(tmp->word, "defun")) == 0) && (tmp->type == WORD)){
			tmp = tmp->next;
			if(tmp == NULL)
				break;
			parseFunction(i, tmp->next, false);
			i++;
		}
		if(((strcmp(tmp->word, "define")) == 0) && (tmp->type == WORD)){
			tmp = tmp->next;
			if(tmp == NULL)
				break;
			tmp = tmp->next;
			if(tmp == NULL)
				break;
			parseFunction(i, tmp->next, true);
			i++;
		}
		tmp = tmp->next;
	}
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
	sizeGraph++;
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
		if(((strcmp(tmp->word, ")")) == 0) && (tmp->type == SPLITTER)){
			if(--balance == 0)
				return count;
		} else if(((strcmp(tmp->word, "(")) == 0) && (tmp->type == SPLITTER)){
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
		if(((strcmp(tmp->word, "defun")) == 0) && (tmp->type == WORD)){
			tmp = tmp->next;
			if(tmp == NULL)
				break;
			addDefun(tmp->word, paramDefun(tmp->next, false));
		}
		if(((strcmp(tmp->word, "define")) == 0) && (tmp->type == WORD)){
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

int CallGraph::findDefun(const char* name)
{
	Defun* tmp = firstDefun;
	while(tmp){
		if(strcmp(tmp->function, name) == 0)
			return tmp->number;
		tmp = tmp->next;
	}
	return -1;
}

void CallGraph::printDefun() const
{
	Defun* tmp = firstDefun;
	while(tmp){
		printf("%s  %d\r\n", tmp->function, tmp->number);
		tmp = tmp->next;
	}
}

void CallGraph::printGraph() const
{
	for(int i = 0; i < sizeGraph; i++){
		printf("%s:\r\n", Graph[i].name);
		/*for(int j = 0; j < Graph[i].list.size(); j++){
			printf("\t%s %d %d\n", Graph[i].list[j].name,
								Graph[i].list[j].callNumber,
								Graph[i].list[j].type);
		}*/
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

void CallGraph::deleteGraph()
{
	for(int i = 0; i < sizeGraph; i++){
		for(int j = 0; j < Graph[i].list.size(); j++){
			delete [] Graph[i].list[j].name;
		}
		delete [] Graph[i].name;
	}
	delete [] Graph;
}
