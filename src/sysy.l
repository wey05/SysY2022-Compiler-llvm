%option noyywrap
%option yylineno
%option nounput
%option noinput

%{
#include <string>
#include <iostream>
#include <vector>
#include "Ast.h"
#include "sysy.tab.h" 
void yyerror(const char* s);
%}

/* 定义区 */
ws          [ \t\r\n]
digit       [0-9]
letter      [a-zA-Z_]
ident       {letter}({letter}|{digit})*

dec_const   [1-9]{digit}*|0
oct_const   0[0-7]+
hex_const   0[xX][0-9a-fA-F]+

float_const ({digit}+\.?{digit}*|\.{digit}+)([eE][+-]?{digit}+)?[fF]?|0[xX]([0-9a-fA-F]+\.?[0-9a-fA-F]*|\.[0-9a-fA-F]+)([pP][+-]?{digit}+)?[fF]?
/* 注释状态 */
%x COMMENT 






/* 规则区 */
%%
"//".*          { }
"/*"            { BEGIN(COMMENT); }
<COMMENT>"*/"   { BEGIN(INITIAL); }
<COMMENT>(.|\n) { }
{ws}+           { }


"int"           { return INT; }
"float"         { return FLOAT; }
"void"          { return VOID; }
"const"         { return CONST; }
"return"        { return RETURN; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"break"         { return BREAK; }
"continue"      { return CONTINUE; }


"("             { return LPAREN; }
")"             { return RPAREN; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
","             { return COMMA; }
";"             { return SEMICOLON; }
"="             { return ASSIGN; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MUL; }
"/"             { return DIV; }
"%"             { return MOD; }
"!"             { return NOT; }
"=="            { return EQ; }
"!="            { return NE; }
"<"             { return LT; }
">"             { return GT; }
"<="            { return LE; }
">="            { return GE; }
"&&"            { return AND; }
"||"            { return OR; }


{dec_const}     { yylval.intVal = std::stoi(yytext, nullptr, 10); return INT_CONST; }
{oct_const}     { yylval.intVal = std::stoi(yytext, nullptr, 8);  return INT_CONST; }
{hex_const}     { yylval.intVal = std::stoi(yytext, nullptr, 16); return INT_CONST; }

{float_const}   { 
    std::string s = yytext;
    if (s.back() == 'f' || s.back() == 'F') s.pop_back();
    yylval.floatVal = std::stof(s); 
    return FLOAT_CONST; 
}

{ident}         { yylval.strVal = new std::string(yytext); return IDENT; }

.               { std::cerr << "Error: Unknown character: " << yytext << " at line " << yylineno << std::endl; }

%%
