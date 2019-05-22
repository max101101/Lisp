#include <cstdio>
#include <lisp_lexer.hpp>
#include <lexem.hpp>
#include <call_graph.hpp>
#include <sqlite3.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <Fl/Fl_Input.H>
#include <Fl/Fl_Text_Display.H>
#include <Fl/Fl_Text_Buffer.H>
#include <stdlib.h>
#include <fcntl.h>

struct Gui{
	Fl_Window* window;
	Fl_Text_Buffer* buf;
	Fl_Text_Display* disp;
	Fl_Button* button_start;
	Fl_Check_Button* button_prog;
	Fl_Check_Button* button_compare;
	Fl_Check_Button* button_insert;
	Fl_Input* input_db;
	Fl_Input* input_log;
	Fl_Input* input_thf;
	Fl_Input* input_thp;
	Fl_Input* input_name;
	Fl_Input* input_path;
}gui;

const char* SQLCREATE = "CREATE TABLE IF NOT EXISTS progs (path text,prog text, PRIMARY KEY (path));";
const char* SQLINSERT = "INSERT INTO progs VALUES (?, ?);";
const char* SQLSELECT = "SELECT * FROM progs";
const char* SQLSELECTPROG = "SELECT prog FROM progs WHERE path = ?";

double FUNC_THRESHOLD = 0;
double PROG_THRESHOLD = 0;
FILE* LOG = NULL;

struct cb_data{
	char* name;
	char* text;
	int size;
	Function* cg;
};

static int callback(void *data, int argc, char **argv, char **col_name){
	struct cb_data* cb_data = static_cast<struct cb_data*>(data);
	if(cb_data->size == 0){
		return 0;
	}
	CallGraph cg;
	cg.create(Lexer().start(argv[1]));
	vector<Result> res = cg.Compare(cb_data->cg, cb_data->size, FUNC_THRESHOLD);
	if((double)res.size()/cb_data->size >= PROG_THRESHOLD){
		char buf[1024];
		for(int i = 0; i < 1024; i++){
			buf[i] = 0;
		}
		sprintf(buf, "%s alike %s\r\n", cb_data->name, argv[0]);
		if(LOG){
			fputs(buf, LOG);
		}
		gui.buf->append(buf);
		for(int i = 0; i < res.size(); i++){
			for(int j = 0; j < 1024; j++){
				buf[j] = 0;
			}
			sprintf(buf, "%s and %s got %lf\r\n", res[i].src_name, res[i].dst_name, res[i].prob);
			if(LOG){
				fputs(buf, LOG);
			}
			gui.buf->append(buf);
		}
		if(LOG){
			fputs("\r\n\r\n", LOG);
		}
		gui.buf->append("\r\n\r\n");
	}
	return 0;
}

int check_file_path(char* file_path)
{
	// .txt .lsp .ss .LSP .lisp
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
	if(strcmp(&file_path[len-5],".lisp") == 0){
		return 0;
	}
	return 1;
}

void file_to_db(char* file_path, sqlite3* db)
{
	//File Read
	if(check_file_path(file_path) != 0){
		char buf[1024];
		for(int i = 0; i < 1024; i++){
			buf[i] = 0;
		}
		sprintf(buf, "Skip insert:%s\r\n", file_path);
		gui.buf->append(buf);
		return;
	}
	FILE* f = fopen(file_path, "r");
	if(f == NULL){
		return;
	}
	// Determine file size
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	char* data = new char[size+1];
	rewind(f);
	fread(data, sizeof(char), size, f);
	data[size] = 0;
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
		delete [] dir_path;
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

void compare_file(char* file_path, sqlite3* db, char is_insert)
{
	//File Read
	if(check_file_path(file_path) != 0){
		//printf("Skip compare:%s\n", file_path);
		return;
	}
	FILE* f = fopen(file_path, "r");
	if(f == NULL){
		return;
	}
	// Determine file size
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	char* data = new char[size+1];
	rewind(f);
	fread(data, sizeof(char), size, f);
	data[size] = 0;
	CallGraph cg;
	cg.create(Lexer().start(data));
	struct cb_data* cb_data = new struct cb_data;
	cb_data->name = file_path;
	cb_data->text = data;
	cb_data->size = cg.GetSize();
	cb_data->cg = cg.GetGraph();
	char *err = 0;
	if(sqlite3_exec(db, SQLSELECT, callback, cb_data, &err)){
		gui.buf->append("DB Sql Error\r\n");
		sqlite3_free(err);
		sqlite3_close(db);
		return;
	}
	//DB work
	if(is_insert){
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
	}
	delete cb_data;
	delete[] data;
	fclose(f);
}

int compare_dir(char* name_dir, const char* path, sqlite3* db, char is_insert)
{
	//current dir path
	int len = strlen(path);
	len += strlen(name_dir);
	len += 2;
	char* dir_path = new char [len];
	sprintf(dir_path, "%s%s/", path, name_dir);
	DIR* dir = opendir(dir_path);
	if(dir == NULL){
		delete [] dir_path;
		return 1;
	}
	struct dirent* name;
	//read files in dir
	while((name = readdir(dir)) != NULL){
		if(strcmp(name->d_name, ".") == 0 || strcmp(name->d_name, "..") == 0){
			continue;
		}
		//check if not dir
		if(compare_dir(name->d_name, dir_path, db, is_insert) == 1){
			int len = strlen(dir_path);
			len += strlen(name->d_name);
			len += 1;
			char* file_path = new char [len];
			sprintf(file_path, "%s%s", dir_path, name->d_name);
			compare_file(file_path, db, is_insert);
			delete [] file_path;
		}
	}
	delete [] dir_path;
	closedir(dir);
	return 0;
}

void select_file(sqlite3* db, const char* name)
{
	//DB work
	sqlite3_stmt *stmt;
	const char *test;
	// Insert data item into myTable
	if(LOG){
		fputs(name, LOG);
		fputs(":\r\n", LOG);
	}
	gui.buf->append(name);
	gui.buf->append(":\r\n");
	int e_flag = 0;
	int rc = sqlite3_prepare(db, SQLSELECTPROG, strlen(SQLSELECTPROG), &stmt, &test);
	if(rc == SQLITE_OK){
		// bind the value
		sqlite3_bind_text(stmt, 1, name, strlen(name), 0);
		// commit
		rc = sqlite3_step(stmt);
		int ncols = sqlite3_column_count(stmt);
		while(rc == SQLITE_ROW){
			e_flag = 1;
			for(int i = 0; i < ncols; i++){
				const unsigned char* text = sqlite3_column_text(stmt,i);
				if(LOG){
					fputs((const char*)text, LOG);
				}
				gui.buf->append((const char*)text);
				gui.buf->append(" ");
			}
			if(LOG){
				fputs("\r\n\r\n", LOG);
			}
			gui.buf->append("\r\n\r\n");
			rc = sqlite3_step(stmt);
		}
		if(e_flag == 0){
			if(LOG){
				fputs("No such file\r\n\r\n", LOG);
			}
			gui.buf->append("No such file\r\n\r\n");
		}
		sqlite3_finalize(stmt);
	}
}

void start(Fl_Widget *w, void* data)
{
	//prepare DB
	sqlite3 *db = 0;
	char *err = 0;
	if(sqlite3_open(gui.input_db->value(), &db)){
		gui.buf->text("DB open Error\r\n");
		sqlite3_close(db);
		return;
	}
	//prepare table
	if(sqlite3_exec(db, SQLCREATE, 0, 0, &err)){
		gui.buf->text("DB Sql Error\r\n");
		sqlite3_free(err);
		sqlite3_close(db);
		return;
	}
	FUNC_THRESHOLD = atof(gui.input_thf->value());
	PROG_THRESHOLD = atof(gui.input_thp->value());
	LOG = fopen(gui.input_log->value(), "w");
	gui.buf->text("");
	//output program
	if(gui.button_prog->value()){
		select_file(db, gui.input_name->value());
	}
	//compare with db
	if(gui.button_compare->value()){
		char is_insert = gui.button_insert->value();
		const char* path_c = gui.input_path->value();
		int len = strlen(path_c);
		if(len < 1){
			sqlite3_close(db);
			return;
		}
		char* path = new char[len+1];
		for(int i = 0; i < len+1; i++){
			path[i] = path_c[i];
		}
		if(path[len-1] == '/' || path[len-1] == '\\'){
			path[len-1] = 0;
		}
		if(compare_dir(path, "", db, is_insert)){
			compare_file(path, db, is_insert);
		}
		delete [] path;
	}
	//inserting in db
	else if(gui.button_insert->value()){
		//prepare dir_path
		const char* path_c = gui.input_path->value();
		int len = strlen(path_c);
		if(len < 1){
			sqlite3_close(db);
			return;
		}
		char* path = new char[len+1];
		for(int i = 0; i < len+1; i++){
			path[i] = path_c[i];
		}
		if(path[len-1] == '/' || path[len-1] == '\\'){
			path[len-1] = 0;
		}
		if(dir_to_db(path, "", db)){
			file_to_db(path, db);
		}
		delete [] path;
	}
	sqlite3_close(db);
	if(LOG){
		fclose(LOG);
	}
}

int main(int argc, char **argv) {
  gui.window = new Fl_Window(1024,768);
  gui.window->label("Lisp Antiplagiarism");
  gui.buf = new Fl_Text_Buffer(0,0);
  gui.disp = new Fl_Text_Display(400, 0, 624, 768);
  gui.disp->buffer(gui.buf);
  gui.button_start = new Fl_Button(150, 550, 100, 100, "Start");
  gui.button_start->type(FL_NORMAL_BUTTON);
  gui.button_start->callback(start, NULL);
  gui.button_prog = new Fl_Check_Button(0, 230, 30, 30, "Show program");
  gui.button_insert = new Fl_Check_Button(0, 350, 30, 30, "Insert programs");
  gui.button_compare = new Fl_Check_Button(0, 380, 30, 30, "Compare programs");
  gui.input_db = new Fl_Input(0, 0, 200, 40, "Database name");
  gui.input_db->value("database.dblite");
  gui.input_log = new Fl_Input(0, 40, 200, 40, "Log file name");
  gui.input_log->value("logfile.txt");
  gui.input_thf = new Fl_Input(0, 100, 200, 40, "Function threshold");
  gui.input_thf->value("0.8");
  gui.input_thp = new Fl_Input(0, 140, 200, 40, "Program threshold");
  gui.input_thp->value("0.2");
  gui.input_name = new Fl_Input(0, 260, 200, 40, "Program name");
  gui.input_name->value("prog.lsp");
  gui.input_path = new Fl_Input(0, 410, 200, 40, "Program location");
  gui.input_path->value("Path/");
  gui.input_db->align(FL_ALIGN_RIGHT);
  gui.input_log->align(FL_ALIGN_RIGHT);
  gui.input_thf->align(FL_ALIGN_RIGHT);
  gui.input_thp->align(FL_ALIGN_RIGHT);
  gui.input_name->align(FL_ALIGN_RIGHT);
  gui.input_path->align(FL_ALIGN_RIGHT);
  gui.window->end();
  gui.window->show(argc, argv);
  return Fl::run();
}
