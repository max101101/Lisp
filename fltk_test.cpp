#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Check_Button.H>
#include <Fl/Fl_Multiline_Output.H>
#include <Fl/Fl_Input.H>
#include <Fl/Fl_Text_Display.H>
#include <Fl/Fl_Text_Buffer.H>
#include <unistd.h>
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

int main(int argc, char **argv) {
  gui.window = new Fl_Window(1024,768);
  gui.window->label("Lisp Antiplagiarism");
  gui.buf = new Fl_Text_Buffer(0,0);
  gui.disp = new Fl_Text_Display(400, 0, 624, 768);
  gui.disp->buffer(gui.buf);
  gui.button_start = new Fl_Button(150, 550, 100, 100, "Start");
  gui.button_start->type(FL_NORMAL_BUTTON);
  gui.button_prog = new Fl_Check_Button(0, 230, 30, 30, "Show program");
  gui.button_insert = new Fl_Check_Button(0, 350, 30, 30, "Insert programs");
  gui.button_compare = new Fl_Check_Button(0, 380, 30, 30, "Compare programs");
  gui.input_db = new Fl_Input(0, 0, 200, 40, "Database name");
  gui.input_db->value("database.dblite");
  gui.input_log = new Fl_Input(0, 40, 200, 40, "Log file name");
  gui.input_log->value("logfile.txt");
  gui.input_thf = new Fl_Input(0, 100, 200, 40, "Function threshold");
  gui.input_thf->value("0.5");
  gui.input_thp = new Fl_Input(0, 140, 200, 40, "Program threshold");
  gui.input_thp->value("0.5");
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
