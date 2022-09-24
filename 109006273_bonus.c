// merge
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAXLEN 256

#define TBLSIZE 64

// Set PRINTERR to 1 to print error message while calling error()
// Make sure you set PRINTERR to 0 before you submit your code
#define PRINTERR 0

// Call this macro to print error message and exit the program
// This will also print where you called it in your program
#define error(errorNum) { \
    if (PRINTERR) \
        fprintf(stderr, "error() called at %s:%d: ", __FILE__, __LINE__); \
    err(errorNum); \
}

// lex.h
// Error types
typedef enum {
    UNKNOWN, END, ENDFILE, UNDEFUKNOWN,
    INT, ID,
    INCDEC,
    ADDSUB, MULDIV,
    AND, OR, XOR,
    ASSIGN, 
    LPAREN, RPAREN
} TokenSet;

// parser.c
typedef enum {
    UNDEFINED, MISPAREN, NOTNUMID, NOTFOUND, RUNOUT, NOTLVAL, DIVZERO, SYNTAXERR
} ErrorType;

// Structure of the symbol table
typedef struct {
    int mem;
    int val;
    char name[MAXLEN];
} Symbol;

// Structure of a tree node
typedef struct _Node {
    TokenSet data;
    int val;
    char lexeme[MAXLEN];
    struct _Node *left; 
    struct _Node *right;
} BTNode;


// functions of parser.h
// The symbol table
extern Symbol table[TBLSIZE];
// array table which has a symbol datatype
// e.g
// no. val name
// 0    0   x
// 1    0   y
// 2    0   z

// Initialize the symbol table with builtin variables
extern void initTable(void);

// Get the value of a variable
extern int getval(char *str);

// Set the value of a variable
extern int setval(char *str, int val);

extern int getmem(char *str);

extern int setmem(char *str, int mem);

// Make a new node according to token type and lexeme
extern BTNode *makeNode(TokenSet tok, const char *lexe);

// Free the syntax tree
extern void freeTree(BTNode *root);

extern void statement(void);
extern BTNode *assign_expr(void); 
extern BTNode *or_expr(void);
extern BTNode *or_expr_tail(BTNode* left);
extern BTNode *xor_expr(void);
extern BTNode *xor_expr_tail(BTNode* left);
extern BTNode *and_expr(void);
extern BTNode *and_expr_tail(BTNode* left);
extern BTNode *addsub_expr(void);
extern BTNode *addsub_expr_tail(BTNode* left);
extern BTNode *muldiv_expr(void);
extern BTNode *muldiv_expr_tail(BTNode* left);
extern BTNode *unary_expr(void);
extern BTNode *factor(void);

// Print error message and exit the program
extern void err(ErrorType errorNum);

// Token types

// function of lex.h 
// Test if a token matches the current token 
extern int match(TokenSet token);

// Get the next token
extern void advance(void);

// Get the lexeme of the current token
extern char *getLexeme(void);

// function of CodeGen.h
// Evaluate the syntax tree
extern int evaluateTree(BTNode *root);

// Print the syntax tree in prefix
extern void printPrefix(BTNode *root);

// Calculate the leaves of the tree for optimization
extern void PreCalTree(BTNode* root);

// check for Assign and incdec sign if there is than update the check
extern void checkAssignIncdec(BTNode* root, int *check);
// Check the ID for handling divisible by zero
extern void checkID(BTNode* root, int *check);

// Print the assembly code
extern int print(BTNode *root, int idx, int *mem);

// CodeGen.c
int evaluateTree(BTNode *root) {
    int retval = 0, lv = 0, rv = 0;

    if (root != NULL) {
        switch (root->data) {
            case ID:
				// only get the value when you assign
                retval = getval(root->lexeme);
                break;
            case INT:                
                retval = atoi(root->lexeme);
                break;
            case ASSIGN:   
                rv = evaluateTree(root->right);
                retval = setval(root->left->lexeme, rv);
                break;
            case ADDSUB:                
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                if (strcmp(root->lexeme, "+") == 0) {
                    retval = lv + rv;
                } else if (strcmp(root->lexeme, "-") == 0) {
                    retval = lv - rv;
                }
                break;
            case MULDIV:                
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                
                if (strcmp(root->lexeme, "*") == 0) {
                    retval = lv * rv;
                } else if (strcmp(root->lexeme, "/") == 0) {
                    if (rv == 0)
                        return 0;
                    
                    retval = lv / rv;
                }
                break;
            case INCDEC:         
                rv = evaluateTree(root->right);
				// root->lexeme  ++ or --
                if(strcmp(root->lexeme, "--") == 0){
                    retval = setval(root->right->lexeme, rv - 1);                   
                } else if (strcmp(root->lexeme, "++") == 0){
                    retval = setval(root->right->lexeme, rv + 1);
                }
                break;
            case AND:
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);

                if(strcmp(root->lexeme, "&") == 0){
                    retval = lv & rv;
                }
                break;
            case OR:
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);

                if(strcmp(root->lexeme, "|") == 0){
                    retval = lv | rv;
                }
                break;
            case XOR:
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);

                if(strcmp(root->lexeme, "^") == 0){
                    retval = lv ^ rv;
                }
                break;
            default:
                retval = 0;
        }
    }
    return retval;
}


void printPrefix(BTNode *root) {
    if (root != NULL) {
        printf("%s ", root->lexeme);
        printPrefix(root->left);
        printPrefix(root->right);
    }
}


void PreCalTree(BTNode* root){
    int retval = 0;
    char buf[12];
    
    if(root != NULL){
        PreCalTree(root->left);
        PreCalTree(root->right);
        if(root->left != NULL && root->right != NULL){
            if(root->left->data == INT && root->right->data == INT){
				// pre calculate it
                if(strcmp(root->lexeme, "/") == 0 && atoi(root->right->lexeme) == 0){
                    printf("EXIT 1\n");
                    error(DIVZERO);
                }
                retval = evaluateTree(root);
                //printf("%d\n", retval);
                root->data = INT;
                sprintf(buf, "%d", retval);
                strcpy(root->lexeme, buf);
            }
        }

    }
    return;

}
void checkAssignIncdec(BTNode* root, int *check){
    if(root != NULL){
        if(root->data == ASSIGN || root->data == INCDEC){
            *check = 1;
        }
        checkAssignIncdec(root->left, check);
        checkAssignIncdec(root->right, check);
    }
}
void checkID(BTNode* root, int *check){
    if(root == NULL) return;
    if(root->data == ID) *check = 1;
    checkID(root->left, check);
    checkID(root->right, check);
}

int print(BTNode* root, int idx, int* mem){
	// *mem to update the memory by 4
	// case for register more than 7 is idx > 7 (push) and idx > 6 (pop)
    int check = 0;
    int lv = 0, rv = 0;
    if(root!= NULL){
        switch(root->data){
            case ID:
                
                if(idx > 7){
                    printf("MOV [%d] r%d\n", *mem,  idx % 8);
                    *mem += 4;
                }
                printf("MOV r%d [%d]\n", idx % 8, getmem(root->lexeme));
                
                break;
            case INT:
                if(idx > 7){
                    printf("MOV [%d] r%d\n", *mem,  idx % 8);
                    *mem += 4;
                }
                printf("MOV r%d %d\n", idx % 8 , atoi(root->lexeme));
                break;
            case ADDSUB:
                
                lv = print(root->left, idx, mem);
                rv = print(root->right, idx + 1, mem);
                
                if (strcmp(root->lexeme, "+") == 0) {
                    printf("ADD r%d r%d\n", lv, rv);
                    
                } else if (strcmp(root->lexeme, "-") == 0) {
                    printf("SUB r%d r%d\n", lv, rv);
                }
                
                // backtrack when register overloaded

                if(idx > 6){ // because when u backtrack it's - 1
                    *mem -=4;
                    printf("MOV r%d [%d]\n", (idx+1)%8,  *mem);

                }
                break;
            case MULDIV:
                check = evaluateTree(root->right);
                if(check == 0){
                    checkID(root->right, &check);
                }
                
                lv = print(root->left, idx, mem);
                rv = print(root->right, idx + 1, mem);
                if (strcmp(root->lexeme, "*") == 0) {
                    printf("MUL r%d r%d\n", lv, rv);
                } else if (strcmp(root->lexeme, "/") == 0) {
                    if (check == 0){
                        printf("EXIT 1\n");
                        error(DIVZERO);
                    }
                    printf("DIV r%d r%d\n", lv, rv);
                }
				// pop
                if(idx > 6){ 
                    *mem -=4;
                    printf("MOV r%d [%d]\n", (idx+1)%8,  *mem);

                }
                break;
            case ASSIGN:                
                rv = print(root->right, idx, mem);
                
                printf("MOV [%d] r%d\n", setmem(root->left->lexeme, 0), rv);
                break;
            case INCDEC:                
                rv = print(root->right, idx, mem);                
                if((idx + 1) > 7){
                    printf("MOV [%d] r%d\n", *mem,  (idx+1) % 8);
                    *mem += 4;
                }
                printf("MOV r%d 1\n", (idx + 1) % 8);
               
                if(strcmp(root->lexeme, "--") == 0){
                    printf("SUB r%d r%d\n", rv, (idx + 1)%8);
                    // because incdec is always + 1 (this is the register for the const 1
                    if((idx + 1) > 7){
                        *mem -=4;
                        printf("MOV r%d [%d]\n", (idx+1)%8,  *mem);
                    }
                    printf("MOV [%d] r%d\n", setmem(root->right->lexeme, 0), idx % 8);
                    
                } else if (strcmp(root->lexeme, "++") == 0){
                    printf("ADD r%d r%d\n", rv, (idx + 1) % 8);
                    if((idx + 1) > 7){                       
                        *mem -=4;
                        printf("MOV r%d [%d]\n", (idx+1)%8,  *mem);
                    }
                    printf("MOV [%d] r%d\n", setmem(root->right->lexeme, 0), idx % 8);
                }                
                break;
            case AND:
                lv = print(root->left, idx, mem);
                rv = print(root->right, idx+1, mem);
                if(strcmp(root->lexeme, "&") == 0){
                    printf("AND r%d r%d\n", lv, rv);
                }
				
                if(idx > 6){ 
                    *mem -=4;
                    printf("MOV r%d [%d]\n", (idx+1)%8,  *mem);
                }
                break;
            case OR:
                lv = print(root->left, idx, mem);
                rv = print(root->right, idx+1, mem);
                if(strcmp(root->lexeme, "|") == 0){
                    printf("OR r%d r%d\n", lv, rv);
                }
                // shady stuff going on here
                if(idx > 6){ 
                    *mem -=4;
                    printf("MOV r%d [%d]\n", (idx+1)%8,  *mem);

                }
                break;
            case XOR:
                lv = print(root->left, idx, mem);
                rv = print(root->right, idx+1, mem);
                if(strcmp(root->lexeme, "^") == 0){
                    printf("XOR r%d r%d\n", lv, rv);
                }
                if(idx > 6){ 
                    *mem -=4;
                    printf("MOV r%d [%d]\n", (idx+1)%8,  *mem);

                }
                break;
        }
    }
    return (idx %8);
}

// lex.c
/*
 Tokenizer “getToken()”: a function that extracts the next token from the input string;
 stores the token in “char lexeme[MAXLEN]”; identifies the token’s type

 typedef enum {UNKNOWN, END, INT, ID, ADDSUB, MULDIV, ASSIGN, LPAREN, RPAREN, ENDFILE} TokenSet
*/
static TokenSet getToken(void);
static TokenSet curToken = UNKNOWN; 
static char lexeme[MAXLEN];

TokenSet getToken(void)
{
    int i = 0;
    char c = '\0';
    
    while ((c = fgetc(stdin)) == ' ' || c == '\t');
    // this one is to skip space or leading tab

    if (isdigit(c)) {
        lexeme[0] = c;
        c = fgetc(stdin);
        i = 1;
        
        while (isdigit(c) && i < MAXLEN) {
            lexeme[i] = c;
            ++i;
            c = fgetc(stdin);
        }
        
        ungetc(c, stdin);
        
        lexeme[i] = '\0';
        return INT;
    } else if (c == '+' || c == '-') {
        lexeme[0] = c;
        c = fgetc(stdin);
        if(lexeme[0] == '+'){
            if(c == '+'){
                // do the inc dec thing
                lexeme[1] = c;
                lexeme[2] = '\0';
                return INCDEC;
            } else {
                
                ungetc(c, stdin);
            }
        } else if(lexeme[0] == '-'){
            if(c == '-'){
                // do the dec thingy
                lexeme[1] = c;
                lexeme[2] = '\0';
                return INCDEC;
            } else {
                
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


void advance(void) {
    //curr token call the function to get the token
    curToken = getToken();
}

int match(TokenSet token) {
    if (curToken == UNKNOWN)
        advance();
    else if(curToken == UNDEFUKNOWN){
        printf("EXIT 1\n");
        exit(0);
        error(SYNTAXERR);
    }
    return token == curToken;
}

char *getLexeme(void) {
    return lexeme;
}


// parse.c
/*
The parsing process: 
group tokens into statements based on a set of rules, collectively called a grammar.
*/
int sbcount = 0;
Symbol table[TBLSIZE];

void initTable(void) {
    strcpy(table[0].name, "x");
    table[0].val = 0;
    table[0].mem = 0;
    strcpy(table[1].name, "y");
    table[1].val = 0;
    table[1].mem = 4;
    strcpy(table[2].name, "z");
    table[2].val = 0;
    table[2].mem = 8;
    sbcount = 3;
}

int getmem(char *str){
    int i = 0;

    for(i = 0; i < sbcount; i++){
        if(strcmp(str, table[i].name) == 0){
            return table[i].mem;
        }
    }

    if(sbcount >= TBLSIZE){
        printf("EXIT 1\n");
        exit(0);
        error(RUNOUT);
    }

    printf("EXIT 1\n");
    //exit(0);
    error(NOTFOUND);
    return 0;
}

int setmem(char *str, int mem){
    int i = 0;
    for(i = 0; i < sbcount; i++){
        if(strcmp(str, table[i].name) == 0){
            
			table[i].mem = mem;
            return mem;
        }
        mem = mem + 4;
    }
    if (sbcount >= TBLSIZE){
        printf("EXIT 1\n");        
        error(RUNOUT);
    }
    
    // else u make a table[sbcount].name and quickly set the mem
    strcpy(table[sbcount].name, str);
    table[sbcount].mem = mem;
    sbcount++;
    return mem;
}

int getval(char *str) {
    int i = 0;

    for (i = 0; i < sbcount; i++)
        if (strcmp(str, table[i].name) == 0)
            return table[i].val;

    if (sbcount >= TBLSIZE){
        printf("EXIT 1\n");
        //exit(0);
        error(RUNOUT);
    }
    // this ^ 
    // else you want this to be an error because u didnt find the 'variable (u haven't set it yet)'
    
    printf("EXIT 1\n");
    error(NOTFOUND);
    return 0;
}

int setval(char *str, int val) {
    int i = 0;
    
    /* use like a loop to check if the variable in the rhs exist by iterating thru the table. if it doesnt then exit 1*/
    for (i = 0; i < sbcount; i++) {
        if (strcmp(str, table[i].name) == 0) {
            table[i].val = val;
            return val;
        }
    }
    // this is like an else statment, if the variable isn't found then you set a new one
    if (sbcount >= TBLSIZE)
        error(RUNOUT);
    
    // else u make a table[sbcount].name and quickly set the value 
    // aa = 5; (e.g)
    strcpy(table[sbcount].name, str);
    table[sbcount].val = val;
    sbcount++;
    return val;
}

BTNode *makeNode(TokenSet tok, const char *lexe) {
    BTNode *node = (BTNode*)malloc(sizeof(BTNode));
    strcpy(node->lexeme, lexe);
    node->data = tok;
    node->val = 0;
    node->left = NULL;
    node->right = NULL;
    return node;
}

void freeTree(BTNode *root) {
    if (root != NULL) {
        freeTree(root->left);
        freeTree(root->right);
        free(root);
    }
}

void statement(void){
    BTNode* retp = NULL;
    if(match(END)){
        //printf(">> ");
        advance();

    } else if(match(ENDFILE)){
        printf("MOV r0 [0]\n");
        printf("MOV r1 [4]\n");
        printf("MOV r2 [8]\n");
        printf("EXIT 0\n");
        exit(0);
    } else {
        retp = assign_expr();
        if(match(END)){
            int memnow = sbcount*4;
            //printf("%d\n", evaluateTree(retp));
            /*printf("Prefix traversal: ");
            printPrefix(retp);
            puts("");*/
			int ch = 0;
            // make a function to look for equal sign or incdec sign for optimization
			// e.g ++x , 1 + 2 + (a = 4)
			checkAssignIncdec(retp, &ch);
			PreCalTree(retp);
            print(retp, 0, &memnow);
            freeTree(retp);
            // printf(">> ");
            // this advance for'\n' to parse after N token
            advance();
        } else {
            printf("EXIT 1\n");
            error(SYNTAXERR);
        }
    }
}


BTNode *assign_expr(void){
    BTNode* retp = NULL;
    BTNode* left = NULL;
    left = or_expr();
	
    if(match(ASSIGN)){
		
        // the bracket error
        if(left->data != ID){
            printf("EXIT 1\n");
            error(NOTLVAL);
        }
        
        retp = makeNode(ASSIGN, getLexeme());
        advance();
        retp->left= left;
        retp->right = assign_expr();
        return retp;
    } else {
        return left;
    }
}


BTNode *or_expr(void){
    BTNode* node = xor_expr();
    return or_expr_tail(node);
}

// or_expr_tail     := OR xor_expr or_expr_tail | NiL 
BTNode *or_expr_tail(BTNode* left){
    BTNode* node = NULL;
    if(match(OR)){
        node = makeNode(OR, getLexeme());
        advance();
        node->left = left;
        node->right = xor_expr();
        return or_expr_tail(node);
    } else {
        return left;
    }
}

BTNode *xor_expr(void){
    BTNode* node = and_expr();
    return xor_expr_tail(node);    
}

BTNode *xor_expr_tail(BTNode* left){
    BTNode* node = NULL;
    if(match(XOR)){
        node = makeNode(XOR, getLexeme());
        advance();
        node->left = left;
        node->right = and_expr();
        return xor_expr_tail(node);
    } else {
        return left;
    }
}

BTNode *and_expr(void){
    BTNode* node = addsub_expr();
    return and_expr_tail(node);
}

BTNode *and_expr_tail(BTNode* left){
    BTNode* node = NULL;
    if(match(AND)){
        node = makeNode(AND, getLexeme());
        advance();
        node->left = left;
        node->right = addsub_expr();
        return and_expr_tail(node);
    } else {
        return left;
    }
}

BTNode *addsub_expr(void){
    BTNode* node = muldiv_expr();
    return addsub_expr_tail(node);
}

BTNode *addsub_expr_tail(BTNode* left){
    BTNode* node = NULL;
    if(match(ADDSUB)){
        node = makeNode(ADDSUB, getLexeme());
        advance();
        node->left = left;
        node->right = muldiv_expr();
        return addsub_expr_tail(node);
    } else {
        return left;
    }
}

BTNode *muldiv_expr(void){
    BTNode* node = unary_expr();
    return muldiv_expr_tail(node);
}

BTNode *muldiv_expr_tail(BTNode* left){
    BTNode* node = NULL;
    if(match(MULDIV)){
        node = makeNode(MULDIV, getLexeme());
        advance();
        node->left = left;
        node->right = unary_expr();
        return muldiv_expr_tail(node);
    } else {
        return left;
    }
}

BTNode *unary_expr(){
    BTNode* left = NULL;
    BTNode* retp = NULL;
    if(match(ADDSUB)){      
        retp = makeNode(ADDSUB, getLexeme());
        advance();
        left = makeNode(INT, "0");
        retp->left = left;
        retp->right = unary_expr();
        
    } else {
        
        retp = factor();
    }
    return retp;
}

BTNode *factor(){
    BTNode* retp = NULL;
    BTNode* left = NULL;
    if(match(INT)){
        retp = makeNode(INT, getLexeme());
        advance();
    } else if(match(ID)){
        retp = makeNode(ID, getLexeme());
        advance();
    } else if(match(INCDEC)){
        retp = makeNode(INCDEC, getLexeme());
        advance();
        if(match(ID)){
            retp->right = makeNode(ID, getLexeme());
            advance();
        } else {
            printf("EXIT 1\n");
            error(SYNTAXERR);
        }
    } else if(match(LPAREN)){
		
        advance();
        retp = assign_expr();
        if(match(RPAREN)){
			
            advance();
        } else {
            printf("EXIT 1\n");
            error(MISPAREN);
        }
    } else {
        printf("EXIT 1\n");
        error(NOTNUMID);
    }
    return retp;
}

void err(ErrorType errorNum) {
    if (PRINTERR) {
        fprintf(stderr, "error: ");
        switch (errorNum) {
            case MISPAREN:
                fprintf(stderr, "mismatched parenthesis\n");
                break;
            case NOTNUMID:
                fprintf(stderr, "number or identifier expected\n");
                break;
            case NOTFOUND:
                fprintf(stderr, "variable not defined\n");
                break;
            case RUNOUT:
                fprintf(stderr, "out of memory\n");
                break;
            case NOTLVAL:
                fprintf(stderr, "lvalue required as an operand\n");
                break;
            case DIVZERO:
                fprintf(stderr, "divide by constant zero\n");
                break;
            case SYNTAXERR:
                fprintf(stderr, "syntax error\n");
                break;
            default:
                fprintf(stderr, "undefined error\n");
                break;
        }
    }
    exit(0);
}
// main.c
int main() {
    initTable();
    //printf(">> ");
    while (1) {
        
        statement();
        // puts("meow");
        // go to parser.c
    }
    return 0;
}


// This package is a calculator
// It works like a Python interpretor
// Example:
// >> y = 2
// >> z = 2
// >> x = 3 * y + 4 / (2 * z)
// It will print the answer of every line
// You should turn it into an expression compiler
// And print the assembly code according to the input

// This is the grammar used in this package
// You can modify it according to the spec and the slide
// statement  :=  ENDFILE | END | expr END
// expr    	  :=  term expr_tail
// expr_tail  :=  ADDSUB term expr_tail | NiL
// term 	  :=  factor term_tail
// term_tail  :=  MULDIV factor term_tail| NiL
// factor	  :=  INT | ADDSUB INT |
//		   	      ID  | ADDSUB ID  | 
//		   	      ID ASSIGN expr |
//		   	      LPAREN expr RPAREN |
//		   	      ADDSUB LPAREN expr RPAREN
