 Conditional Statement Compiler

A Compiler Design mini-project that implements a **compiler front-end** for conditional statements using **Flex (Lex), YACC/Bison, C, and Flask**. The compiler recognizes `if`, `if-else`, and nested conditional statements, validates their syntax, generates an Abstract Syntax Tree (AST), performs semantic analysis, and displays the results through a web-based interface.

---

## Project Overview

This project demonstrates the front-end phases of a compiler by processing conditional statements from source code and producing structured analysis results. It includes lexical analysis, syntax analysis, AST generation, semantic analysis, and a Flask-based GUI for interactive testing.

---

## Objectives

- Recognize keywords, identifiers, numbers, operators, and symbols.
- Validate the syntax of `if`, `if-else`, and nested conditional statements.
- Generate an Abstract Syntax Tree (AST).
- Perform semantic analysis with a symbol table.
- Detect lexical, syntax, and semantic errors.
- Provide a user-friendly web interface for testing source code.

---

## Technologies Used

- C Programming
- Flex (Lex)
- YACC/Bison
- Python (Flask)
- HTML
- CSS
- JavaScript
- Git & GitHub

---

## Project Features

- Lexical analysis using Flex.
- Syntax analysis using YACC/Bison.
- Support for `if`, `if-else`, and nested `if` statements.
- AST generation and tree display.
- Semantic analysis with symbol table generation.
- Detection of lexical, syntax, and semantic errors.
- Flask-based web interface for compilation and output display.

---

## Compiler Architecture

```text
Source Code
     │
     ▼
Lexical Analyzer (Flex)
     │
     ▼
Token Generation
     │
     ▼
Syntax Analyzer (YACC/Bison)
     │
     ▼
Abstract Syntax Tree (AST)
     │
     ▼
Semantic Analysis
     │
     ▼
Compiler Output
     │
     ▼
Flask GUI
```

---

## Project Structure

```text
conditional-compiler/
├── src/
│   ├── lexer.l
│   ├── parser.y
│   ├── main.c
│   ├── ast.c
│   ├── ast.h
│   ├── semantic.c
│   └── semantic.h
├── templates/
│   └── index.html
├── static/
│   ├── style.css
│   └── script.js
├── tests/
├── docs/
│   ├── grammar.md
│   ├── Project_Report.pdf
│   └── Project_Presentation.pptx
├── screenshots/
├── app.py
├── Makefile
└── README.md
```

---

## Installation

### Prerequisites

Install the required packages on Ubuntu/Linux:

```bash
sudo apt update
sudo apt install gcc flex bison make python3 python3-pip python3-venv
```

### Clone the Repository

```bash
git clone https://github.com/ambad-iii/conditional-statement-compiler.git
cd conditional-compiler
```

### Build the Compiler

```bash
make
```

This generates:

- `parser.tab.c`
- `parser.tab.h`
- `lex.yy.c`
- `conditional_compiler`

### Run the Flask Application

Install Flask:

```bash
python3 -m pip install flask
```

Run the application:

```bash
python3 app.py
```

Open:

```text
http://127.0.0.1:9000
```

---

## Lexical Analysis

The lexical analyzer (`lexer.l`) recognizes:

### Keywords

- `if`
- `else`

### Identifiers

- `x`
- `count`
- `_value`

### Numbers

- `10`
- `25.5`

### Operators

- `+`
- `-`
- `*`
- `/`
- `>`
- `<`
- `>=`
- `<=`
- `==`
- `!=`
- `&&`
- `||`
- `!`

### Symbols

- `(`
- `)`
- `{`
- `}`
- `;`

Invalid characters produce lexical error messages with line numbers.

---

## Grammar Support

The parser validates:

- `if` statements
- `if-else` statements
- Nested conditional statements
- Assignment statements
- Statement blocks
- Arithmetic expressions
- Relational expressions
- Logical expressions

---

## Abstract Syntax Tree (AST)

The compiler generates an AST representing the structure of the program.

Example:

Input:

```c
x = 10;

if (x > 5) {
    y = 20;
}
```

AST:

```text
PROGRAM
├── ASSIGN: x
│   └── NUMBER: 10
└── IF
    ├── CONDITION
    │   └── BINARY_OP: >
    │       ├── IDENTIFIER: x
    │       └── NUMBER: 5
    └── THEN
        └── BLOCK
            └── ASSIGN: y
                └── NUMBER: 20
```

---

## Semantic Analysis

The semantic analyzer performs:

- Symbol table generation
- Variable validation
- Boolean condition checking
- Expression type checking
- Assignment compatibility checking
- Division-by-zero detection

Example Symbol Table:

```text
Symbol table:

x      number
y      number
z      number
flag   bool
```

---

## Example Test Cases

### Valid Test Cases

#### Test 1

```c
x = 10;

if (x > 5) {
    y = 20;
}
```

#### Test 2

```c
x = 10;

if (x > 5) {
    y = 20;
} else {
    y = 1;
}
```

#### Test 3

```c
x = 10;

if (x > 5) {
    if (x < 20) {
        z = 30;
    }
}
```

#### Test 4

```c
x = 5;

if (x == 5) {
    y = 1;
}
```

#### Test 5

```c
x = 8;

if (x >= 5 && x <= 10) {
    y = 100;
}
```

---

### Invalid Test Cases

#### Test 1

```c
if x > 5 {
    y = 20;
}
```

#### Test 2

```c
if (x > 5 {
    y = 20;
}
```

#### Test 3

```c
if (x > 5) {
    y = 20;
```

#### Test 4

```c
x = ;
```

#### Test 5

```c
x = 10 @ 5;
```

---

## Error Handling

The compiler detects:

### Lexical Errors

Example:

```text
Invalid character '@'
```

### Syntax Errors

Example:

```text
Syntax Error at line 3
```

### Semantic Errors

Examples include:

- undeclared variables
- invalid Boolean conditions
- incompatible assignments
- division by zero

---

## GUI Features

The Flask-based interface provides:

- Source code editor
- Compile button
- Clear button
- Syntax Analysis panel
- AST panel
- Semantic Analysis panel
- Complete Compiler Output panel
- Error status messages

---

## Build Commands

Build the compiler:

```bash
make
```

Clean generated files:

```bash
make clean
```

Rebuild:

```bash
make clean
make
```

Run Flask:

```bash
python3 app.py
```

---

## Team Contributions

| Member | Responsibility |
|--------|---------------|
| **Ambadi B (44)** | Lexical Analyzer and Flask GUI |
| **Angelina Anna Antony (52)** | CFG and Grammar |
| **Ashmy Philo Joji (67)** | Syntax Analyzer |
| **Abhinav A (7)** | AST and Semantic Analysis |
| **Akshay Wilson (26)** | Integration and Testing |
| **Abiya Susan Cherian (14)** | Report and PPT Preparation |

---

## Future Enhancements

- Variable declaration support.
- Additional data types.
- More control structures.
- Better error recovery.
- Enhanced GUI with token visualization.

---

## Department

**Department of Computer Science and Engineering**

**Amal Jyothi College of Engineering**

Compiler Design Mini Project (Group 3)
