#include <cstdio>
#include <lisp_lexer.hpp>
#include <lexem.hpp>
#include <call_graph.hpp>
#include <sqlite3.h>
#include <string.h>
#include <dirent.h>

const char* DATABASE = "database.dblite";
const char* SQLCREATE = "CREATE TABLE IF NOT EXISTS progs (path text,prog text);";
const char* SQLINSERT = "INSERT INTO progs VALUES (?, ?);";
const char* SQLSELECT = "SELECT * FROM progs";

static int callback(void *data, int argc, char **argv, char **col_name){
	printf("%s = %s\n", col_name[0], argv[0]);
	CallGraph().create(Lexer().start(argv[1]));
	printf("\n");
	return 0;
}

int check_file_path(char* file_path)
{
	// .txt .lsp .ss .LSP
	int len = strlen(file_path);
	if(len < 4){
		return 1;
	}
	if(strcmp(&file_path[len-4],".txt") == 0){
		return 0;
	}
	if(strcmp(&file_path[len-4],".lsp") == 0){
		return 0;
	}
	if(strcmp(&file_path[len-3],".ss") == 0){
		return 0;
	}
	if(strcmp(&file_path[len-4],".LSP") == 0){
		return 0;
	}
	return 1;
}

void file_to_db(char* file_path, sqlite3* db)
{
	//File Read
	if(check_file_path(file_path) != 0){
		return;
	}
	FILE* f = fopen(file_path, "r");
	if(f == NULL){
		return;
	}
	// Determine file size
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	char* data = new char[size];
	rewind(f);
	fread(data, sizeof(char), size, f);
	//DB work
	sqlite3_stmt *stmt;
	const char *test;
	// Insert data item into myTable
	int rc = sqlite3_prepare(db, SQLINSERT, strlen(SQLINSERT), &stmt, &test);
	if(rc == SQLITE_OK){
		// bind the value
		sqlite3_bind_text(stmt, 1, file_path, strlen(file_path), 0);
		sqlite3_bind_text(stmt, 2, data, strlen(data), 0);
		// commit
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
	}
	delete[] data;
	fclose(f);
}

int dir_to_db(char* name_dir, const char* path, sqlite3* db)
{
	//current dir path
	int len = strlen(path);
	len += strlen(name_dir);
	len += 2;
	char* dir_path = new char [len];
	sprintf(dir_path, "%s%s/", path, name_dir);
	DIR* dir = opendir(dir_path);
	if(dir == NULL){
		return 1;
	}
	struct dirent* name;
	//read files in dir
	while((name = readdir(dir)) != NULL){
		if(strcmp(name->d_name, ".") == 0 || strcmp(name->d_name, "..") == 0){
			continue;
		}
		//check if not dir
		if(dir_to_db(name->d_name, dir_path, db) == 1){
			int len = strlen(dir_path);
			len += strlen(name->d_name);
			len += 1;
			char* file_path = new char [len];
			sprintf(file_path, "%s%s", dir_path, name->d_name);
			file_to_db(file_path, db);
			delete [] file_path;
		}
	}
	delete [] dir_path;
	closedir(dir);
	return 0;
}

int main(int argc, char** argv)
{
	if(argc < 2){
		printf("Error: wrong params\nprog type [dir]\n");
		return 1;
	}
	//prepare DB
	sqlite3 *db = 0;
	char *err = 0;
	if(sqlite3_open(DATABASE, &db)){
		fprintf(stdout, "DB open Error: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return 1;
	}
	//prepare table
	if(sqlite3_exec(db, SQLCREATE, 0, 0, &err)){
		fprintf(stdout, "DB Sql Error: %s\n", err);
		sqlite3_free(err);
		sqlite3_close(db);
		return 1;
	}
	if(strcmp(argv[1],"insert") == 0){
		//prepare dir_path
		if(argc != 3){
			printf("Dir missing\n");
		}
		int len = strlen(argv[2]);
		if(argv[2][len-1] == '/'){
			argv[2][len-1] = 0;
		}
		if(dir_to_db(argv[2], "", db)){
			file_to_db(argv[2], db);
		}
	}	
	if(strcmp(argv[1],"select") == 0){
		if(sqlite3_exec(db, SQLSELECT, callback, 0, &err)){
			fprintf(stdout, "DB Sql Error: %s\n", err);
			sqlite3_free(err);
			sqlite3_close(db);
			return 1;
		}
	}
	sqlite3_close(db);
	return 0;
}
