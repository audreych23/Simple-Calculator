#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "codeGen.h"

/*
The parsing process: 
group tokens into statements based on a set of rules, collectively called a grammar.
*/
int sbcount = 0;
Symbol table[TBLSIZE];

// initialize table just with x y z 
// keep in mind that it will be an error if we do an x = aa +2 
// and aa is not initialized yet
/*int passtablecnt(int sbcount){
    return sbcount;  
}*/
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
        /*printf("%s\n", str);
        printf("table name : %s, i : %d\n", table[i].name, i);

        printf("cmp : %d", strcmp(str, table[i].name));*/
        if(strcmp(str, table[i].name) == 0){
            return table[i].mem;
        }
    }

    if(sbcount >= TBLSIZE){
        printf("EXIT 1\n");
        //exit(0);
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
            // this is for setting the old variable,, u cant just return mem
            // 
            //printf("i : %d, mem : %d", i, mem);
            table[i].mem = mem;
            // so you want to change the mem into the table mem and print this
            // remember mem will start from this again later every time 
            // or nevermind that means if it exists the it goes here
            // i dont think i even need table[i].mem but we'll see
            // because we can just iterate from mem = 0;
            // wait nvm we need to
            // but i dont think we need the mem = table[i].mem
            //mem = table[i].mem;
            return mem;
        }
        mem = mem + 4;
    }
    if (sbcount >= TBLSIZE){
        printf("EXIT 1\n");
        //exit(0);
        error(RUNOUT);
    }
    
    // else u make a table[sbcount].name and quickly set the value 
    // aa = 5; (e.g)
    // the problem is how u se the mem like this
    strcpy(table[sbcount].name, str);
    table[sbcount].mem = mem;
    sbcount++;
    return mem;
}
// variable name more than tblsize which is 65355? wait no i got scammed it's 64 iirc exit the program? 
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
    // else you want this to be an error because u didnt find the 'thing'
    // destination , source
    //strcpy(table[sbcount].name, str); 
    // copy str(?) // Q. what is str here (passed from what function)
    // copy str to the table
    // i assume it's for variable name but idk 
    //table[sbcount].val = 0; 
    // val of table always set it to 0
    // sbcount is the amount of element in table rn
    //sbcount++; 
    printf("EXIT 1\n");
    //exit(0);
    error(NOTFOUND);
    return 0;
}

int setval(char *str, int val) {
    int i = 0;
    // Q. what is val and str here? (trace it from other function)
    // oh this time u set the val
    // when u initialize all val is zero
    // I THINK THIS IS TO INITIALIZE THE X, Y, Z
    // x = 2
    // compare if x == table[i].name (x)
    // table[i].val = the value 
    /* use like a loop to check if the variable in the rhs exist by iterating thru the table. if it doesnt then exit 1*/
    for (i = 0; i < sbcount; i++) {
        if (strcmp(str, table[i].name) == 0) {
            table[i].val = val;
            return val;
        }
    }
    // can't we put this before the loop? nvm this is like an else statment, if the variable isn't found then you set a new one
    // Q. wait then what's the use of the function on top ( don't forget to trace)
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
        //retp = assign_expr();
        retp = assign_expr();
        if(match(END)){
            // get memory now to initiate stack 
            // variable to check to optimize if there's equal sign
            int ch = 0;
            int memnow = sbcount*4;
            //printf("%d\n", evaluateTree(retp));
            printf("Prefix traversal: ");
            printPrefix(retp);
            puts("");
            checkAssignIncdec(retp, &ch);
            PreCalTree(retp);
            //int check = 1;
            // make a function to look for equal sign or incdec sign because this is the only 
            // if u find any of those try printing which might be faster (slightly) why ? have to make those funct
            // because 
            // 1 + ++x (valid and updates x) or 1 + 3 + (a = 4) (valid and updates 4 )
            // so maybe we need to think of this too
            if(ch == 1){
                print(retp, 0, &memnow);
            }
            
            freeTree(retp);
            //printf(">> ");
            // this advance for'\n' to parse after N token
            advance();
        } else {
            printf("EXIT 1\n");
            //exit(0);
            error(SYNTAXERR);
        }
    }
}

BTNode *assign_expr(void){
    // can we call a function to advance in the parameter 
    // so the idea is it can backtrack ???
    // is there other way we can, not advance ?? 
    // what if i write xx xy xz 
    BTNode* retp = NULL;
    BTNode* left = NULL;
    left = or_expr();
    if(match(ASSIGN)){
        
        // the bracket error
        if(left->data != ID){
            printf("EXIT 1\n");
            error(NOTLVAL);
        }
        // what about (x) = 3 ( ithink invalid)
        // for case like x + 5 ? 
        // call or_expr?
        // no what im supposed to do is to make an ungetc (de advance function)
        // left = or_expr();
        // retp = left; 
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
        // how do u know it's gonna be a space next ??
        retp = makeNode(ADDSUB, getLexeme());
        advance();
        left = makeNode(INT, "0");
        retp->left = left;
        retp->right = unary_expr();
        // how do i return before calling the advance?? 
        // and backtrack lol
    } else {
        // if it's incdec it will also go here, but how??
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
            //exit(0);
            error(SYNTAXERR);
        }
    } else if(match(LPAREN)){
        advance();
        retp = assign_expr();
        if(match(RPAREN)){
            
            advance();
        } else {
            printf("EXIT 1\n");
            //exit(0);
            error(MISPAREN);
        }
    } else {
        printf("EXIT 1\n");
        //exit(0);
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
