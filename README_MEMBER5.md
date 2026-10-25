# Member 5 — Integration and Testing

## Files in this package

- `main.c` — connects the parser, AST and semantic-analysis modules.
- `Makefile` — builds the Lex/Yacc compiler.
- `tests/` — sample valid and invalid input programs.

## Important integration agreement

The provided `main.c` expects the parser to expose these symbols:

```c
extern int yyparse(void);
extern FILE *yyin;
extern ASTNode *ast_root;
```

In `parser.y`, define the root variable:

```c
#include "ast.h"
ASTNode *ast_root = NULL;
```

At the end of your start/program grammar rule, assign the generated AST to `ast_root`. The exact action depends on your grammar's semantic-value names. For example, if the start rule's value is `$1`, the action may be:

```yacc
program:
    statement_list { ast_root = $1; }
;
```

Do not paste this example blindly if your grammar uses different nonterminals or semantic types. Make sure `%union`, `%type`, and token definitions match the AST actions in your parser.

## Build

Put `main.c`, `ast.c`, `ast.h`, `semantic.c`, `semantic.h`, `lexer.l`, and `parser.y` in the same directory. Then run:

```bash
make
```

Run one input file:

```bash
./conditional_compiler tests/valid_if.src
```

Run the starter tests:

```bash
make test
```

Clean generated files:

```bash
make clean
```

## Notes about semantic declarations

The current semantic module has no declaration grammar. To let the starter tests use `x`, `y`, `z`, and `flag`, `main.c` registers those names before analysis. This is a temporary testing arrangement, not a substitute for proper declarations. If your grammar adds variable declarations, remove those hard-coded calls and populate the symbol table from parsed declarations instead.

The Makefile uses `-lfl`, which is typical for Flex on Linux. If your system does not have that library or uses a different Lex implementation, the link command may need adjustment.

## Test coverage checklist

- Simple `if`
- `if-else`
- Nested `if`
- Boolean condition
- Invalid syntax (manual negative test)
- AST printed after successful parsing
- Semantic analysis and symbol table output

The exact source syntax must match the productions implemented in your group's `parser.y`.
