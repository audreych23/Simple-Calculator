#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"

static TokenSet getToken(void);
static TokenSet curToken = UNKNOWN; //wut --> lex.h (something like unidentified but not an error) mean something like hasn't been known yet
static char lexeme[MAXLEN];
// maxlen is 256 is it bad if it's more than 256?
/*
 Tokenizer “getToken()”: a function that extracts the next token from the input string;
 stores the token in “char lexeme[MAXLEN]”; identifies the token’s type

 typedef enum {UNKNOWN, END, INT, ID, ADDSUB, MULDIV, ASSIGN, LPAREN, RPAREN, ENDFILE} TokenSet
*/
TokenSet getToken(void)
{
    int i = 0;
    char c = '\0';
    // idk what is this for
    while ((c = fgetc(stdin)) == ' ' || c == '\t');
    // oh this one is to skip space or leading tab

    if (isdigit(c)) {
        lexeme[0] = c;
        c = fgetc(stdin);
        i = 1;
        // i think this is something like 12 and u parse 1 by 1, 1 and 2 
        while (isdigit(c) && i < MAXLEN) {
            lexeme[i] = c;
            ++i;
            c = fgetc(stdin);
        }
        //idk what unget is 
        // waht does it do when u unget ? make it into '\0'?
        // unget it because last one u get is not digit so u want to put it back into the str
        ungetc(c, stdin);
        // for end of string at max len?
        lexeme[i] = '\0';
        // if imt not wrong then it's like
        // 12
        // lexeme[1] == '1'
        // lexeme[2] == '2'
        // c = (e.g) ' + ' ( not a digit break loop) i = 3
        // do ungetc('+')
        // Q. why do u need to unget tho...?
        // returns charscter '+' btw  (google)
        // returns to what ???
        // lexeme[3] = '\0'
        return INT;
    } else if (c == '+' || c == '-') {
        // pls fix im crying
        // but what im confused about is, if 2++1 works, then how do u do the incdec thingy?
        // 2++1 is 2 + 0 + 1 apparently..?
        // oh it will become like 2+(+1);
        lexeme[0] = c;
        c = fgetc(stdin);
        if(lexeme[0] == '+'){
            if(c == '+'){
                // do the inc dec thing
                lexeme[1] = c;
                lexeme[2] = '\0';
                return INCDEC;
            } else {
                // +- 3 doesnt work but 1 +- 3 works
                // 1+-3 still works tho
                // oh else jut ungetc
                ungetc(c, stdin);
            }
        } else if(lexeme[0] == '-'){
            if(c == '-'){
                // do the dec thingy
                lexeme[1] = c;
                lexeme[2] = '\0';
                return INCDEC;
            } else {
                // else do unget c
                ungetc(c, stdin);
            }
        }
        lexeme[1] = '\0';
        return ADDSUB;
    } else if (c == '*' || c == '/') {
        lexeme[0] = c;
        lexeme[1] = '\0';
        return MULDIV;
    } else if (c == '\n') {
        lexeme[0] = '\0';
        return END;
    } else if (c == '=') {
        // why not lexeme[0] = '='
        // is this the same? xd
        strcpy(lexeme, "=");
        return ASSIGN;
    } else if (c == '(') {
        strcpy(lexeme, "(");
        return LPAREN;
    } else if (c == ')') {
        strcpy(lexeme, ")");
        return RPAREN;
    } else if (isalpha(c) || c == '_') {
        // edit this so it will accept more than 1 char
        lexeme[0] = c;
        c = fgetc(stdin);
        i = 1;
        while((isalpha(c) || isdigit(c) || c == '_') && i < MAXLEN){
            lexeme[i] = c;
            ++i;
            c = fgetc(stdin);
        }
        ungetc(c, stdin);
        lexeme[i] = '\0';
        return ID;
    } else if (c == EOF) {
        return ENDFILE;
    } else if (c == '^'){
        lexeme[0] = c;
        lexeme[1] = '\0';
        return XOR;
    } else if (c == '|'){
        lexeme[0] = c;
        lexeme[1] = '\0';
        return OR;
    } else if(c == '&'){
        lexeme[0] = c;
        lexeme[1] = '\0';
        return AND;
    } else if(c <= '%' && c >= '!' || c == '\'' || c == ',' || c == '.' || (c >= ':' && c <= '<') || c == '>' || c == '@' || c == '\?' || c == '{' || c == '}' || c == '~' || c == '`' || c == '[' || c == ']'){
        return UNDEFUKNOWN;
    } else {
        return UNKNOWN;
    }
}
// add more like '&' operator etcetc

void advance(void) {
    //curr token call the function to get the token
    curToken = getToken();
}

int match(TokenSet token) {
    if (curToken == UNKNOWN)
        advance();
    else if(curToken == UNDEFUKNOWN){
        printf("EXIT 1\n");
        //exit(0);
        error(SYNTAXERR);
    }
    return token == curToken;
}

char *getLexeme(void) {
    return lexeme;
}
