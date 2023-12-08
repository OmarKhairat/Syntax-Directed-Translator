%{
#include <stdio.h>
%}

letter [a-zA-Z]
digit [0-9]
digits {digit}+
relop (==|!=|>|>=|<|<=)
assign =

%%

boolean    { printf("BOOLEAN\n"); }
int        { printf("INT\n"); }
float      { printf("FLOAT\n"); }

id         { printf("ID: %s\n", yytext); }

num        { printf("NUM: %s\n", yytext); }

{relop}    { printf("RELOP: %s\n", yytext); }
{assign}   { printf("ASSIGN\n"); }

if         { printf("IF\n"); }
else       { printf("ELSE\n"); }
while      { printf("WHILE\n"); }

[;,\(\){}] { printf("PUNCTUATION: %s\n", yytext); }

"+"        { printf("ADDOP: %s\n", yytext); }
"-"        { printf("ADDOP: %s\n", yytext); }
"*"        { printf("MULOP: %s\n", yytext); }
"/"        { printf("MULOP: %s\n", yytext); }

{letter}({letter}|{digit})* { printf("ID: %s\n", yytext); }
{digits}   { printf("NUM: %s\n", yytext); }

.          { /* Ignore any other characters */ }

%%

int main() {
    yylex();
    return 0;
}

