# Context-Free Grammar for Conditional Statement Compiler

## 1. Introduction

A Context-Free Grammar (CFG) defines the syntax rules of a programming language. In this project, the CFG is used to recognize `if`, `if-else`, and nested conditional statements and validate conditions, expressions, and statement blocks.

The grammar is designed for a simplified C-like language.

## 2. Tokens

The grammar uses the following tokens supplied by the lexical analyzer:

| Token | Description |
|---|---|
| `IF` | if keyword |
| `ELSE` | else keyword |
| `ID` | Identifier or variable name |
| `NUM` | Numeric constant |
| `RELOP` | Relational operators: `<`, `>`, `<=`, `>=`, `==`, `!=` |
| `ASSIGN` | Assignment operator `=` |
| `AND` | Logical AND `&&` |
| `OR` | Logical OR `||` |
| `NOT` | Logical NOT `!` |
| `LPAREN`, `RPAREN` | Parentheses `(` and `)` |
| `LBRACE`, `RBRACE` | Braces `{` and `}` |
| `SEMICOLON` | Semicolon `;` |
| `PLUS`, `MINUS`, `MUL`, `DIV` | Arithmetic operators `+`, `-`, `*`, `/` |

## 3. Context-Free Grammar (CFG)

The following grammar defines the syntax of conditional statements.

### 3.1 Program and Statements

```text
program       -> statement_list

statement_list -> statement_list statement
               | statement

statement     -> matched_statement
               | unmatched_statement

matched_statement -> simple_statement
                   | block
                   | IF LPAREN condition RPAREN
                     matched_statement ELSE matched_statement

unmatched_statement -> IF LPAREN condition RPAREN statement
                     | IF LPAREN condition RPAREN
                       matched_statement ELSE unmatched_statement

block         -> LBRACE statement_list RBRACE
               | LBRACE RBRACE

simple_statement -> ID ASSIGN expression SEMICOLON
                  | expression SEMICOLON
```

### 3.2 Conditions

```text
condition -> expression RELOP expression
           | expression
           | NOT condition
           | LPAREN condition RPAREN
           | condition AND condition
           | condition OR condition
```

### 3.3 Arithmetic Expressions

```text
expression -> expression PLUS term
            | expression MINUS term
            | term

term       -> term MUL factor
            | term DIV factor
            | factor

factor     -> ID
            | NUM
            | LPAREN expression RPAREN
```

**Note:** Operator precedence and associativity must be specified in the YACC/Bison implementation so that expressions such as `a + b * c` are interpreted correctly.

## 4. Grammar Transformations

### Dangling-Else Problem

The dangling-else problem occurs when an `if` statement is nested inside another `if` statement without braces. It may be unclear which `if` the `else` belongs to.

Example:

```c
if (a > 0)
    if (b > 0)
        x = 1;
    else
        x = 2;
```

In this project, `else` is associated with the nearest unmatched `if`.

The grammar separates statements into `matched_statement` and `unmatched_statement` categories to remove this ambiguity.

- A matched statement contains a complete `if-else` pair or another complete statement.
- An unmatched statement contains an `if` without a corresponding `else`.

This grammar structure helps the parser handle nested conditional statements without the usual dangling-else ambiguity.

## 5. Valid Test Examples

### Example 1: Simple if

```c
if (a > 10)
    x = 5;
```

### Example 2: If-else

```c
if (a > b) {
    x = 10;
} else {
    x = 20;
}
```

### Example 3: Nested if

```c
if (a > 0) {
    if (b > 0) {
        x = 1;
    }
}
```

### Example 4: Nested if-else

```c
if (a > b) {
    x = 1;
} else {
    if (a < b) {
        x = 2;
    } else {
        x = 3;
    }
}
```

### Example 5: Logical conditions

```c
if (a > 0 && b < 10) {
    x = a + b;
}
```

## 6. Invalid Test Examples

### Example 1: Missing condition

```c
if ()
    x = 5;
```

### Example 2: Missing closing parenthesis

```c
if (a > 10 {
    x = 5;
}
```

### Example 3: Missing statement terminator

```c
if (a > 10) {
    x = 5
}
```

### Example 4: Missing condition operand

```c
if (a > ) {
    x = 5;
}
```

### Example 5: Missing opening brace

```c
if (a > 10) {
    x = 5;
else {
    x = 10;
}
```

## 7. Conclusion

The CFG defines the syntax of conditional statements, arithmetic expressions, logical conditions, and statement blocks. The matched/unmatched grammar structure addresses the dangling-else problem. These grammar rules will be implemented by the syntax analyzer using YACC/Bison to identify valid statements and report syntax errors.
