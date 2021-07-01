###############################################################################
# CSE 4713 / 6713 Assignment 2 example flex file
###############################################################################
LEX		=	flex
.PRECIOUS 	=	parser.cpp rules.l lexer.h	

lex.exe: lex.yy.o parser.o
	g++ -std=c++11 -o $@ $^

lex.yy.o: lex.yy.c lexer.h
	cc  -o $@ -c lex.yy.c

parser.o: parser.cpp lexer.h
	g++ -std=c++11 -o $@ -c parser.cpp

lex.yy.c: rules.l
	$(LEX) $^

clean: 
	$(RM) *.o lex.yy.c lex.exe

