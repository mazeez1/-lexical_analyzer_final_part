/*****************************************************************************
// CSE 4713 / 6713 Assignment 2 example flex file
*****************************************************************************/
%option noyywrap

%{
#include "lexer.h"
%}

/* matches any letter */
alpha 		[[:alpha:]]

/* matches any with space, tab */							
blank 		[[:blank:]]

/* matches any digit */							
digit 		[[:digit:]]

/* matches a letter followed by one-or-more letters-or-digits-or-underscore */							
id 			({alpha})({alpha}|{digit}|[_])*		

/* matches one "" and all the characters inside except \n, until find other "" */
string  	\"[^\n]+\"							

/* matches one-or-more digits */
int 		{digit}+

/* matches one-or-more digits followed by a '.' followed by one-or-more digits */							
float 		{digit}+"."{digit}*|"."{digit}+	

/* any character that don't match */	
unknown 	.

%%

	/* if is a blank ignore it */
{blank}|\n 	
	/* else process lexeme */
if 			return TOK_IF; 
else 		return TOK_ELSE; 
for 		return TOK_FOR; 
while 		return TOK_WHILE;
print 		return TOK_PRINT;
return 		return TOK_RETURN;
continue 	return TOK_CONTINUE;
break 		return TOK_BREAK;
debug 		return TOK_DEBUG;
read 		return TOK_READ;
let 		return TOK_LET;

int 		return TOK_INT;
float 		return TOK_FLOAT;
string 		return TOK_STRING;

";"			return TOK_SEMICOLON;
"("			return TOK_OPENPAREN;
")" 		return TOK_CLOSEPAREN;
"["			return TOK_OPENBRACKET;
"]"			return TOK_CLOSEBRACKET;
"{"			return TOK_OPENBRACE;
"}"			return TOK_CLOSEBRACE;
","			return TOK_COMMA;

"+"			return TOK_PLUS;
"-"			return TOK_MINUS;
"*"			return TOK_MULTIPLY;
"/"			return TOK_DIVIDE;
":="		return TOK_ASSIGN;
"=="		return TOK_EQUALTO;
"<"			return TOK_LESSTHAN;
">"			return TOK_GREATERTHAN;
"<>"		return TOK_NOTEQUALTO;
and			return TOK_AND;
or			return TOK_OR;
not 		return TOK_NOT;
len 		return TOK_LENGTH;

{int}		return TOK_FLOATLIT;
{float}		return TOK_FLOATLIT;
{string}	return TOK_STRINGLIT;
{id}		return TOK_IDENTIFIER;
{unknown}	return TOK_UNKNOWN;

<<EOF>> 	return TOK_EOF;
