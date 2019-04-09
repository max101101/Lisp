CPPMODULESNAMES = lexem.cpp lisp_lexer.cpp call_graph.cpp
CMODULESNAMES = sqlite3.c
INCLUDEPATH = include
MODULESPATH = src
OBJPATH = obj
MAINMODULE = main.cpp
PROGNAME = prog
CPPSRCMODULES = $(CPPMODULESNAMES:%=$(MODULESPATH)/%)
CSRCMODULES = $(CMODULESNAMES:%=$(MODULESPATH)/%)
CPPOBJMODULES = $(CPPMODULESNAMES:%.cpp=$(OBJPATH)/%.o)
COBJMODULES = $(CMODULESNAMES:%.c=$(OBJPATH)/%.o)
CXXFLAGS = -g -Wall
CFLAGS = -g -Wall

.PHONY: all clean

all: $(OBJPATH) $(PROGNAME)

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

deps.mk: $(CPPSRCMODULES)
	$(CXX) -I$(INCLUDEPATH) -MM $^ > $@

$(OBJPATH):
	mkdir -p $@

$(OBJPATH)/%.o: $(MODULESPATH)/%.cpp $(INCLUDEPATH)/%.hpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDEPATH) -c $< -o $@

$(OBJPATH)/%.o: $(MODULESPATH)/%.c $(INCLUDEPATH)/%.h
	gcc $(CFLAGS) -I$(INCLUDEPATH) -c $< -o $@

$(PROGNAME): $(MAINMODULE) $(CPPOBJMODULES) $(COBJMODULES)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEPATH) $^ -o $@

clean:
	rm -rf $(OBJPATH) $(PROGNAME) deps.mk $(PROGNAME).dSYM
