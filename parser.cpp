//*****************************************************************************
//  Driver File
// Author: Moses Azeez
//*****************************************************************************
#include <iostream>
#include <map>
#include <string>
#include <cstdio>

#include "lexer.h"

using namespace std;

extern "C"
{
// Instantiate global variables
extern FILE *yyin;         // input stream
extern FILE *yyout;        // output stream
extern int   yyleng;       // length of current lexeme
extern char *yytext;       // text of current lexeme
extern int   yylex();      // the generated lexical analyzer
}

int token;   // keep each token read
map<string, double> memo;  // table to contain identifier values

// helper function to print n spaces as deep in the tree
string printSpace(int n)
{
  return string(n, ' ');
}

// #include "productions.h"

// helpers macros to determine first set of tokens
#define isFirstOfP  token == TOK_OPENBRACE
#define isFirstOfS  token == TOK_LET or token == TOK_READ or token == TOK_PRINT or token == TOK_IF or token == TOK_WHILE 
#define isFirstOfA  token == TOK_LET
#define isFirstOfE  token == TOK_NOT or token == TOK_MINUS or token == TOK_OPENPAREN or token == TOK_IDENTIFIER or token == TOK_FLOATLIT
#define isFirstOfB  isFirstOfE
#define isFirstOfR  isFirstOfE
#define isFirstOfT  isFirstOfE
#define isFirstOfF  isFirstOfE
#define isFirstOfU  token == TOK_OPENPAREN or token == TOK_IDENTIFIER or token == TOK_FLOATLIT
#define isFirstOfG  token == TOK_READ
#define isFirstOfO  token == TOK_PRINT
#define isFirstOfC  token == TOK_IF
#define isFirstOfW  token == TOK_WHILE

// if a conditional was found then should be it evaluated?
bool valid = true;

// grammatical productions
void P();
void S();
void A();
void G();
void O();
void C();
void W();

double E();
double B();
double R();
double U();
double T();
double F();

int main( int argc, char* argv[] )
{
  // Set the input and output streams
  yyin = stdin;
  yyout = stdout;
 
  try 
  {
    // Do the lexical parsing
    token = yylex();

    if (not (isFirstOfP))
      throw "Expected {";
    
    P();

    if (token != TOK_EOF )
      throw "Expected EOF";
  }
  catch (const char * exec)
  {
    cout << "error: " << exec << '\n';
  }

  // Tell the world about our success!!
  cout << '\n' << "=== GO BULLDOGS! Your parse was successful! ===" << "\n\n";

  for(const auto & it : memo)
    cout << "symbol = " << it.first << ", value = " << it.second << endl;

}

// P -> { {S} }
void P()
{
    static int countP = 0;
    int curP = countP++;

    cout << printSpace(curP) << "enter P " << curP << '\n';
    cout << "-->found " << yytext << '\n';  // '{'
    
    token = yylex();
    
    if (not (isFirstOfS)) // we expect to see a S
        throw "Expected let, read, print, if, while";
    
    S();  // process it

    while (isFirstOfS)  // there may be one or more S followed
        S();

 
    if (token != TOK_CLOSEBRACE)  // we expect to see a }
        throw "Expected }";
    
    cout << "-->found " << yytext << '\n';
    cout << printSpace(curP) << "exit P " << curP << '\n';


    token = yylex();  // process next token
}

// S -> A | G | O | C | W
void S()
{
    static int countS = 0;
    int curS = countS++;

    cout << printSpace(curS) << "enter S " << curS << '\n';
    
    if (isFirstOfA) 
        A();
    else if (isFirstOfG) 
        G();
    else if (isFirstOfO) 
        O();
    else if (isFirstOfC) 
        C();
    else if (isFirstOfW) 
        W();
    else 
        throw "Expected let, read, print, if, while";
    
    cout << printSpace(curS) << "exit S " << curS << '\n';
}

// A -> let ID := E;
void A()
{
    static int countA = 0;
    int curA = countA++;

    cout << printSpace(curA) << "enter A " << curA << '\n';
    cout << "-->found " << yytext << '\n';  // let

    token = yylex();

    if (token != TOK_IDENTIFIER)  // we expect to see an identifier
        throw "Expected an identifier";

    cout << "-->found ID: " << yytext << '\n'; 
    
    string id = yytext; // keep the identifier

    token = yylex();

    if (token != TOK_ASSIGN)  // we expect to see a :=
        throw "Expected :=";
    
    cout << "-->found " << yytext << '\n';

    token = yylex();
   
    if (not (isFirstOfE)) // we expect to see a E
        throw "Expected not, -, (, ID, FLOATLIT";

    double value = E();

    if (valid)  // if this A is in an invalid scope, then ignore it
        memo[id] = value;

    if (token != TOK_SEMICOLON) // and at the end ;
        throw "Expected ;";
    
    cout << "-->found " << yytext << '\n';
    cout << printSpace(curA) << "exit A " << curA << '\n';

    token = yylex();  // process next token
}

// E -> B {( and | or ) B }
double E()
{
    static int countE = 0;
    int curE = countE++;

    cout << printSpace(curE) << "enter E " << curE << '\n';

    if (not (isFirstOfB)) // we expect to see a B
        throw "not, -, (, ID, FLOATLIT";

    double value1 = B();  

    while (token == TOK_AND or token == TOK_OR) { // there may be 1 or more {( and | or ) B }
        cout << "-->found " << yytext << '\n';
        int prev = token; // keep the current token
        
        token = yylex();

        if (not (isFirstOfB)) // we expect to see a B
            throw "not, -, (, ID, FLOATLIT ";

        double value2 = B();

        // process the logical operation
        if (token == TOK_AND) 
            value1 = (double) (value1 and value2);
        else
            value1 = (double) (value1 or value2);
    }

    cout << printSpace(curE) << "exit E " << curE << '\n';

   return value1;
}

// B -> R [( < | > | == ) R ]
double B()
{
    static int countB = 0;
    int curB = countB++;

    cout << printSpace(curB) << "enter B " << curB << '\n';

    if (not (isFirstOfR)) // we expect to see a R
         throw "not, -, (, ID, FLOATLIT";

    double value1 = R();
    
    while (token == TOK_LESSTHAN or token == TOK_GREATERTHAN or token == TOK_EQUALTO) { // in case there is a [( < | > | == ) R ] followed
        cout << "-->found " << yytext << '\n';
        int prev = token; // keep the current token
        
        token = yylex();

        if (not (isFirstOfR)) // we expect to see a R
            throw "not, -, (, ID, FLOATLIT ";

        double value2 = R();
        
        // process the logical operation
        if (prev == TOK_LESSTHAN)
            value1 = (double) (value1 < value2);
        else if (prev == TOK_GREATERTHAN)
            value1 = (double) (value1 > value2);
        else
            value1 = (double) (value1 == value2);
        
    }

    cout << printSpace(curB) << "exit B " << curB << '\n';

    return value1;
}

// R -> T {( + | - ) T }
double R()
{
    static int countR = 0;
    int curR = countR++;

    cout << printSpace(curR) << "enter R " << curR << '\n';

    if (not (isFirstOfT)) // we expect to see a T
        throw "not, -, (, ID, FLOATLIT ";

    double value1 = T();

    while (token == TOK_PLUS or token == TOK_MINUS) { // there may be 1 or more {( + | - ) T }
        cout << "-->found " << yytext << '\n';
        int prev = token; // keep the current token
  
        token = yylex();

        if (not (isFirstOfT)) // we expect to see a T
            throw "not, -, (, ID, FLOATLIT ";
        
        double value2 = T();

        // process aritmetic operation
        if (prev == TOK_PLUS)
            value1 += value2;
        else
            value1 -= value2;
    }

    cout << printSpace(curR) << "exit R " << curR << '\n';
  
    return value1;
}

// T -> F {( * | / ) F }
double T()
{
    static int countT = 0;
    int curT = countT++;

    cout << printSpace(curT) << "enter T " << curT << '\n';

    if (not (isFirstOfF)) // we expect to see a F
        throw "not, -, (, ID, FLOATLIT ";

    double value1 = F();

    while (token == TOK_MULTIPLY or token == TOK_DIVIDE) {  // there may be 1 or more {( * | / ) F }
        cout << "-->found " << yytext << '\n';
        int prev = token; // keep the current token
        
        token = yylex();

        if (not (isFirstOfF)) // we expect to see a F
            throw "not, -, (, ID, FLOATLIT";
        
        double value2 = F();

        // process aritmetic operation
        if (prev == TOK_MULTIPLY)
            value1 *= value2;
        else
            value1 /= value2;
        
    }

    cout << printSpace(curT) << "exit T " <<  curT << '\n';

    return value1;
}

// F -> [ not | - ] U
double F()
{
    static int countF = 0;
    int curF = countF++;

    cout << printSpace(curF) << "enter F " << curF << '\n';

    bool sign = false;  // is signed U?
    
    if (token == TOK_NOT or token == TOK_MINUS) { // if yes then
        sign = true;

        cout << "-->found " << yytext << '\n';
        token = yylex();
    }
    
    if (not (isFirstOfU)) // we expect to see a U
        throw "Expected token (, ID, FLOATLIT";

    double value = U();
    
    // if is signed then multiply it by -1
    if (sign and value)
        value *= -1;

    cout << printSpace(curF) << "exit F " <<  curF << '\n';

    return value;
}

// U -> ID | FLOATLIT | (E)
double U()
{
    static int countU = 0;
    int curU = countU++;

    cout << printSpace(curU) << "enter U " << curU << '\n';

    double value = 0;

    if (token == TOK_IDENTIFIER) {
        if (memo.find(yytext) == memo.end() and valid) // if the identifier is not found and is in a valid scope then
            throw "Uninitialized identifier used in expression";
        
        cout << "-->found ID: " << yytext << '\n';
        value = memo[yytext]; // insert the identifier and update it's value
    }
    
    if (token == TOK_FLOATLIT) {
        cout << "-->found FLOATLIT: " << yytext << '\n';
        value = stod(string(yytext), nullptr);
    }
    
    if (token == TOK_OPENPAREN) {
        cout << "-->found " << yytext << '\n';
        
        token = yylex();

        if (not (isFirstOfE)) // we expect to see a (
            throw "Expected not, -, (, ID, FLOATLIT";
        
        value = E();

        if (token != TOK_CLOSEPAREN)  // and at the end )
            throw "Expected )";
        
        cout << "-->found " << yytext << '\n';

        
    }     
    
    cout << printSpace(curU) << "exit U " <<  curU << '\n';
    
    token = yylex();  // process next token

    return value;
}

// read [ STRINGLIT ] ID;
void G()
{
    static int countG = 0;
    int curG = countG++;

    cout << printSpace(curG) << "enter G " << curG << '\n';
    
    cout << "-->found " << yytext << '\n';  // read

    token = yylex();
   
    if (token == TOK_STRINGLIT or token == TOK_IDENTIFIER) {  // there may be a string literal
        if (token == TOK_STRINGLIT) { // if yes then
            cout << "-->found string: " << yytext << '\n';
            token = yylex();
            if (token == TOK_IDENTIFIER) {  // we expect to see an identifier  
                cout << "-->found ID: " << yytext << '\n';
                memo[yytext] = 0.0; // insert it and update it's value
            }
            else
                throw "Expected identifier";
        }
        else  // there must be an identifier
            memo[yytext] = 0.0; // insert it and update it's value
    }
    else 
        throw "Expected string literal or identifier";
    
    token = yylex();

    if (token != TOK_SEMICOLON) // we expect to see a ;
        throw "Expected semicolon";
    
    cout << "-->found " << yytext << '\n';
    cout << printSpace(curG) << "exit G " <<  curG << '\n';

    token = yylex();  // process next token
}

// print [ STRINGLIT ] [ ID ];
void O()
{
    static int countO = 0;
    int curO = countO++;

    cout << printSpace(curO) << "enter O " << curO << '\n';

    cout << "-->found " << yytext << '\n';  // print

    token = yylex();

    if (token == TOK_STRINGLIT or token == TOK_IDENTIFIER or token == TOK_SEMICOLON) {  //  // there may be a string literal or an identifier
        if (token == TOK_STRINGLIT) { // if is a string literal then
            cout << "-->found  string: " << yytext << '\n';
            token = yylex();
            if (token == TOK_IDENTIFIER) {  // we expect to see an identifier
                cout << "-->found ID: " << yytext << '\n';
                token = yylex();
                if (token == TOK_SEMICOLON) // we expect to see a ;
                    cout << "-->found " << yytext << '\n';
                else
                    throw "Expected semicolon";
            }
            else if (token == TOK_SEMICOLON)  //  must be a ; and if not
                cout << "-->found " << yytext << '\n';
            else
                throw "Expected identifier or semicolon";
        }
        else if (token == TOK_IDENTIFIER) { // if is an identifier then
            cout << "-->found ID: " << yytext << '\n';
            token = yylex();
            if (token == TOK_SEMICOLON) // we expect to see a ;
                cout << "-->found " << yytext << '\n';
            else
                throw "Expected semicolon";
        }
        else
            cout << "-->found " << yytext << '\n';
    }
    else 
        throw "Expected string literal, identifier or semicolon";

    cout << printSpace(curO) << "exit O " <<  curO << '\n';

    token = yylex(); // process next token
}

// if (E) P [ else P ]
void C()
{
    static int countC = 0;
    int curC = countC++;

    cout << printSpace(curC) << "enter C " << curC << '\n';
    cout << "-->found " << yytext << '\n';  // if

    token = yylex();

    if (token != TOK_OPENPAREN) // we expect to see (
        throw "Expected (";
    
    cout << "-->found " << yytext << '\n';

    token = yylex();

    if (not (isFirstOfE)) // we expect to see a E
        throw "Expected not, -, (, ID, FLOATLIT";
    
    double value = E();

    if (token != TOK_CLOSEPAREN)  // we expect to see )
        throw "Expected )";

    cout << "-->found " << yytext << '\n';
    
    token = yylex();

    if (not (isFirstOfP)) // body of the statement
        throw "Expected {";
    
    if (not value)  // if statement returns false, then is an invalid scope
        valid = false;
    
    P();
    
    if (token == TOK_ELSE) {  // there may be an else
        if (not valid)  // if statement returns false, then is a valid scope
          valid = true; 
        
        cout << "-->found " << yytext << '\n';
        
        token = yylex();
        
        if (not (isFirstOfP))
          throw "Expected {";
        
        P();
    }

    valid = true; // reset valid to true

    cout << printSpace(curC) << "exit C " <<  curC << '\n';
}

// while (E) P
void W()
{
    static int countW = 0;
    int curW = countW++;

    cout << printSpace(curW) << "enter W " << curW << '\n';
    cout << "-->found " << yytext << '\n';  // while

    token = yylex();

    if (token != TOK_OPENPAREN) // we expect to see (
        throw "Expected (";
    
    cout << "-->found " << yytext << '\n';
    token = yylex();

    if (not (isFirstOfE)) // we expect to see a E
        throw "Expected not, -, (, ID, FLOATLIT";
    
    double value = E();

    if (token != TOK_CLOSEPAREN)  // and at the end )
        throw "Expected )";
    
    cout << "-->found " << yytext << '\n';

    token = yylex();

    if (not value)  // if the statment returns false, then the next P is invalid
        valid = false; 
    
    if (not (isFirstOfP)) // we expect to see a P
        throw "Expected {";

    P();
    
    valid = true; // reset the valid to true
    
    cout << printSpace(curW) << "exit W " <<  curW << '\n';
}