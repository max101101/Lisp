#include <cstdio>
#include <lisp_lexer.hpp>
#include <lexem.hpp>
#include <call_graph.hpp>

int main(int argc, char** argv){
	if(argc < 2){
		printf("Use: [prog] [inputFile]\n");
		return 1;
	}
	for(int i = 1; i < argc; ++i){
		FILE* fileIn = fopen(argv[i], "r");
		if(fileIn == NULL){
			perror("Open file\n");
			return 1;
		}
		/*FILE* fileOut = fopen(argv[i], "w");
		if(fileOut == NULL){
			perror("Open file\n");
			return 1;
		}*/
		printf("%s:\n", argv[i]);
		CallGraph().create(Lexer().start(fileIn));
		fclose(fileIn);
		printf("\n");
		//fclose(fileOut);
	}
	return 0;
}
