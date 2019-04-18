%{
#include "transform.tab.h"
%}
%option noyywrap

LETTER      [a-zA-Z_]
DIGIT       [0-9]
NUMBER      [0-9]+
IDENTIFIER  {LETTER}({LETTER}|{DIGIT})*
STRING      \"(.)*\"
CHAR        \'(.)\'


%%
{NUMBER}       { yylval.d = atof(yytext); return NUMBER; }

"sc.range"     { return SCRANGE; }
".collect()"   { return COLLECT; }
".map"         { return MAP; }
"val"          { return VAL; }
"if"           { return IF; }
"else"         { return ELSE; }

{IDENTIFIER}   { yylval.s = strdup(yytext); return IDENTIFIER; }
{STRING}       { return STRING; }
{CHAR}         { return CHAR; }

"=>"           { return ARROW; }
"=="           { return EQ; }
"!="           { return NE; }
">="           { return GE; }
"<="           { return LE; }
"<" |
">" |
"(" |
")" |
"{" |
"}" |
"." |
"," |
";" |
"+" |
"-" |
"*" |
"%" |
"="            { return yytext[0];}
[ \t\n]+       { /* ignore */}
.              { /* ignore */}
%%