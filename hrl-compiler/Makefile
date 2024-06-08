CC = gcc
FLEX = flex
BISON = bison

LEX_FILE = hrl.l
YACC_FILE = hrl.y
LEX_OUTPUT = lex.yy.c
YACC_OUTPUT = hrl.tab.c
EXECUTABLE = hrl_compiler

FLEX_CMD = $(FLEX) $(LEX_FILE)
BISON_CMD = $(BISON) -d $(YACC_FILE)

COMPILE_CMD = $(CC) -o $(EXECUTABLE) $(YACC_OUTPUT) $(LEX_OUTPUT)

all: $(EXECUTABLE)

$(EXECUTABLE): $(LEX_OUTPUT) $(YACC_OUTPUT)
	$(COMPILE_CMD)

$(LEX_OUTPUT): $(LEX_FILE)
	$(FLEX_CMD)

$(YACC_OUTPUT): $(YACC_FILE)
	$(BISON_CMD)

clean:
	rm -f $(LEX_OUTPUT) $(YACC_OUTPUT) $(EXECUTABLE) hrl.tab.h

.PHONY: all clean
