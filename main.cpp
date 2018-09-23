#include <cstdio>
#include <lisp_lexer.hpp>
#include <lexem.hpp>
#include <call_graph.hpp>

int main(int argc, char** argv){
	if(argc < 2){
		printf("Use: [prog] [inputFile]\n");
		return 1;
	}
	Lexer a;
	FILE* file = fopen(argv[1], "r");
	Lexem* first = a.start(file);
	CallGraph b;
	b.create(first);
	deleteLexem(first);
	fclose(file);
	return 0;
}
