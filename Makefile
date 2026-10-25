
CC = gcc
FLEX = flex
BISON = bison

CFLAGS = -Wall -g -Isrc

TARGET = conditional_compiler

all: $(TARGET)

$(TARGET): parser.tab.c lex.yy.c src/main.c src/ast.c src/semantic.c
	$(CC) $(CFLAGS) -o $(TARGET) src/main.c src/ast.c src/semantic.c parser.tab.c lex.yy.c -lfl

parser.tab.c parser.tab.h: src/parser.y
	$(BISON) -d -o parser.tab.c src/parser.y

lex.yy.c: src/lexer.l parser.tab.h
	$(FLEX) -o lex.yy.c src/lexer.l

clean:
	rm -f $(TARGET) lex.yy.c parser.tab.c parser.tab.h

.PHONY: all clean
