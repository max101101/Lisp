MODULES_NAMES = lexem.cpp lisp_lexer.cpp call_graph.cpp
INCLUDE_PATH = include
MODULES_PATH = src
OBJ_PATH = obj
SRCMODULES = $(MODULES_NAMES:%=$(MODULES_PATH)/%)
OBJMODULES = $(MODULES_NAMES:%.cpp=$(OBJ_PATH)/%.o)
CXXFLAGS = -g -Wall

ifneq (clean, $(MAKECMDGOALS))
-include deps.mk
endif

deps.mk: $(SRCMODULES)
	$(CXX) -I$(INCLUDE_PATH) -MM $^ > $@

clean:
	rm -rf obj prog deps.mk prog.dSYM

$(OBJ_PATH)/%.o: $(MODULES_PATH)/%.cpp $(INCLUDE_PATH)/%.hpp
	mkdir -p $(OBJ_PATH)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_PATH) -c $< -o $@

prog: main.cpp $(OBJMODULES)
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_PATH) $^ -o $@
