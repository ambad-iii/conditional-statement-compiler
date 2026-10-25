# Member 5 build file
# Requires: gcc, flex, bison (or yacc-compatible tools)

CC      = gcc
FLEX    = flex
BISON   = bison
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -g
TARGET  = conditional_compiler

# Change these filenames if your group named them differently.
LEX_FILE    = lexer.l
PARSER_FILE = parser.y

.PHONY: all clean test

all: $(TARGET)

parser.tab.c parser.tab.h: $(PARSER_FILE)
	$(BISON) -d -Wall -o parser.tab.c $(PARSER_FILE)

lex.yy.c: $(LEX_FILE) parser.tab.h
	$(FLEX) -o lex.yy.c $(LEX_FILE)

$(TARGET): main.c ast.c semantic.c parser.tab.c lex.yy.c
	$(CC) $(CFLAGS) -o $(TARGET) main.c ast.c semantic.c parser.tab.c lex.yy.c -lfl

test: $(TARGET)
	@echo "Running compiler tests..."
	@set -e; \
	for f in tests/valid_*.src; do \
	  echo ""; echo "---- $$f (expected to parse) ----"; \
	  ./$(TARGET) "$$f" || exit 1; \
	done
	@echo ""; echo "Valid-input tests completed."
	@echo "Invalid syntax tests are listed in tests/invalid_syntax.src."
	@echo "Run manually: ./$(TARGET) tests/invalid_syntax.src"
	@echo "A non-zero exit status is expected for invalid syntax."

clean:
	rm -f $(TARGET) lex.yy.c parser.tab.c parser.tab.h
