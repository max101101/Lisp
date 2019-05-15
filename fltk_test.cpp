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

Fl_Input * input1;
Fl_Input * input2;
Fl_Input * input3;
Fl_Input * input4;
Fl_Input * input5;
Fl_Input * input6;

int main(int argc, char **argv) {
  Fl_Window *window = new Fl_Window(1024,768);
  window->label("Lisp Antiplagiarism");
  Fl_Text_Buffer *buf = new Fl_Text_Buffer(0,0);
  Fl_Text_Display *disp = new Fl_Text_Display(400, 0, 624, 768);
  disp->buffer(buf);
	/*
  char buffer[10240];
  int fd = open("out.txt", O_RDONLY, 0777);
  int i = read(fd, buffer, 10000);
  buffer[i] = 0;
  buf->text(buffer);
*/
  Fl_Button *button = new Fl_Button(150, 550, 100, 100, "Start");
  button->type(FL_NORMAL_BUTTON);
  Fl_Check_Button *button2 = new Fl_Check_Button(0, 230, 30, 30, "Show program");
  button->type(FL_NORMAL_BUTTON);
  input1 = new Fl_Input(0, 0, 200, 40, "Database name");
  input1->value("database.dblite");
  input2 = new Fl_Input(0, 40, 200, 40, "Log file name");
  input2->value("logfile.txt");
  input3 = new Fl_Input(0, 100, 200, 40, "Function threshold");
  input3->value("0.5");
  input4 = new Fl_Input(0, 140, 200, 40, "Program threshold");
  input4->value("0.5");
  input5 = new Fl_Input(0, 260, 200, 40, "Program name");
  input5->value("prog.lsp");
  input1->align(FL_ALIGN_RIGHT);
  input2->align(FL_ALIGN_RIGHT);
  input3->align(FL_ALIGN_RIGHT);
  input4->align(FL_ALIGN_RIGHT);
  input5->align(FL_ALIGN_RIGHT);
  Fl_Check_Button *button3 = new Fl_Check_Button(0, 350, 30, 30, "Insert programs");
  Fl_Check_Button *button4 = new Fl_Check_Button(0, 380, 30, 30, "Compare programs");
  input6 = new Fl_Input(0, 410, 200, 40, "Program location");
  input6->value("Path/");
  input6->align(FL_ALIGN_RIGHT);
  window->end();
  window->show(argc, argv);
  return Fl::run();
}
