#ifndef __PARSER__
#define __PARSER__

#include "lex.h"
#define TBLSIZE 64

// Set PRINTERR to 1 to print error message while calling error()
// Make sure you set PRINTERR to 0 before you submit your code
#define PRINTERR 1

// Call this macro to print error message and exit the program
// This will also print where you called it in your program
#define error(errorNum) { \
    if (PRINTERR) \
        fprintf(stderr, "error() called at %s:%d: ", __FILE__, __LINE__); \
    err(errorNum); \
}

// Error types
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

//extern int passtablecnt(int);
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

/*extern BTNode *term(void);
extern BTNode *term_tail(BTNode *left);
extern BTNode *expr(void);
extern BTNode *expr_tail(BTNode *left);
*/
// Print error message and exit the program
extern void err(ErrorType errorNum);

#endif // __PARSER__
