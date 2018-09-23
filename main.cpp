#include <cstdio>
#include <lisp_lexer.hpp>
#include <lexem.hpp>
#include <call_graph.hpp>

int main(int argc, char** argv){
	if(argc < 2){
		printf("Use: [prog] [inputFile]\n");
		return 1;
	}
	FILE* file = fopen(argv[1], "r");
	if(file == NULL){
		perror("Open file\n");
		return 1;
	}
	CallGraph().create(Lexer().start(file));
	fclose(file);
	return 0;
}
