MODULESNAMES = lexem.cpp lisp_lexer.cpp call_graph.cpp sqlite.cpp
INCLUDEPATH = include
MODULESPATH = src
OBJPATH = obj
MAINMODULE = main.cpp
PROGNAME = prog
SRCMODULES = $(MODULESNAMES:%=$(MODULESPATH)/%)
OBJMODULES = $(MODULESNAMES:%.cpp=$(OBJPATH)/%.o)
CXXFLAGS = -g -Wall

.PHONY: all clean

all: $(OBJPATH) $(PROGNAME)

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

deps.mk: $(SRCMODULES)
	$(CXX) -I$(INCLUDEPATH) -MM $^ > $@

$(OBJPATH):
	mkdir -p $@

$(OBJPATH)/%.o: $(MODULESPATH)/%.cpp $(INCLUDEPATH)/%.hpp
	$(CXX) $(CXXFLAGS) -I$(INCLUDEPATH) -c $< -o $@

$(PROGNAME): $(MAINMODULE) $(OBJMODULES)
	$(CXX) $(CXXFLAGS) -I$(INCLUDEPATH) $^ -o $@

clean:
	rm -rf $(OBJPATH) $(PROGNAME) deps.mk $(PROGNAME).dSYM
