%{
  #include "transform.h"
  #include <stdio.h>
%}

%expect 8

%union {
  struct ast_node *ast;
  int d;
  char *s;
}

%token <d> NUMBER
%token SCRANGE COLLECT MAP VAL IF ELSE
%token <s> IDENTIFIER
%token STRING CHAR ARROW EQ NE GE LE

%type <ast> Program MapOps UDF Expression SimpleExpr TupleExpr ComplexExpr AssignExprs AssignExpr PureExpr CompExpr Op Comp

%start Program

%%

Program:
  SCRANGE '(' NUMBER ',' NUMBER ')' MapOps COLLECT {
    ast_node *root = program($3, $5, $7);
    // walk(root, 0);
    trans(root);
    printf("\n");
    free_ast_tree(root);
  }
;
MapOps:
  /* empty */ { $$ = NULL; }
| MapOps MAP '(' UDF ')' { $$ = ast('M', $1, $4); }
;
UDF:
  IDENTIFIER ARROW Expression { $$ = udf($1, $3); }
;
Expression:
 '{' ComplexExpr '}' { $$ = $2; }
| SimpleExpr { $$ = $1; }
;
SimpleExpr:
  PureExpr { $$ = $1; }
| '(' TupleExpr ')' { $$ = $2; }
;
TupleExpr:
  PureExpr ',' PureExpr { $$ = ast('T', $1, $3); }
| TupleExpr ',' PureExpr { $$ = ast('T', $1, $3); }
;
ComplexExpr:
  SimpleExpr { $$ = $1; }
| AssignExprs ';' SimpleExpr { $$ = ast('S', $1, $3); }
;
AssignExprs:
  AssignExpr { $$ = $1; }
| AssignExprs ';' AssignExpr { $$ = ast('S', $1, $3); }
;
AssignExpr:
  VAL IDENTIFIER '=' PureExpr { $$ = assignment($2, $4); }
;
PureExpr:
  NUMBER { $$ = num($1); }
| IDENTIFIER { $$ = id($1); }
| IDENTIFIER '.' IDENTIFIER { $$ = dotID($1, $3); }
| '(' PureExpr ')' { $$ = $2; }
| PureExpr Op PureExpr { $$ = operation($1, $2, $3); }
| IF '(' CompExpr ')' PureExpr ELSE PureExpr { $$ = ifstmt($3, $5, $7); }
;
CompExpr:
  PureExpr Comp PureExpr { $$ = comparison($1, $2, $3); }
;
Op:
  '+' { $$ = op('+'); }
| '-' { $$ = op('-'); }
| '*' { $$ = op('*'); }
| '%' { $$ = op('%'); }
;
Comp:
  EQ  { $$ = comparator(eq); }
| '<' { $$ = comparator(lt); }
| '>' { $$ = comparator(gt); }
| NE  { $$ = comparator(ne); }
| GE  { $$ = comparator(ge); }
| LE  { $$ = comparator(le); }
;

%%
