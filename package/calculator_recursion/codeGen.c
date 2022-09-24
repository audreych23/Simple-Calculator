#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codeGen.h"
/*
The code generation process: 
constructing the machine-language instructions to implement the statements recognized 
by the parser and represented as syntax trees
In codeGen.h/codeGen.c, we provide a function: evaluateTree(BTNode *root) 
that calculates the answer by pre-order traversal of the syntax tree
*/
int evaluateTree(BTNode *root) {
    int retval = 0, lv = 0, rv = 0;

    if (root != NULL) {
        switch (root->data) {
            case ID:
                // e.g a 
                // u only get the value of a u dont assing it againn
                retval = getval(root->lexeme);
                break;
            case INT:
                // forgot on what atoi does again
                retval = atoi(root->lexeme);
                break;
            case ASSIGN:
                // assign here is = sign
                rv = evaluateTree(root->right);
                // call the function again
                // maybe it's something like a + (b = 5) // and b hasn't existed yet
                // when there is an assign statement u want to set the value
                // this is why there is no case a = a + 5
                retval = setval(root->left->lexeme, rv);
                break;
            case ADDSUB:
                // wait why is this empty
                lv = evaluateTree(root->left);
                rv = evaluateTree(root->right);
                if (strcmp(root->lexeme, "+") == 0) {
                    retval = lv + rv;
                } else if (strcmp(root->lexeme, "-") == 0) {
                    retval = lv - rv;
                }
                break;
            case MULDIV:
                // why put + and - here? sdsf
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
                // should i put lv as 0 always?? wheneer it meets an incdec?
                /* pls fix im crying */
                rv = evaluateTree(root->right);
                // i have to make an assign
                // root->lexeme will always be ++ or --
                if(strcmp(root->lexeme, "--") == 0){
                    retval = setval(root->right->lexeme, rv - 1);
                    // don't forget to somehow set it first to ID like setval
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

// should I make a postfix
void printPrefix(BTNode *root) {
    if (root != NULL) {
        printf("%s ", root->lexeme);
        printPrefix(root->left);
        printPrefix(root->right);
    }
}

/*void printAssembly(BTNode *root){
    if(root != NULL){
        printAssembly(root->left);
        printAssembly(root->right);
        print(root);
    }
}*/
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
void checkID(BTNode* root, int *check){
    if(root == NULL) return;
    if(root->data == ID) *check = 1;
    checkID(root->left, check);
    checkID(root->right, check);
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
int print(BTNode* root, int idx, int* mem){
    //printf("%d", idx);
    /* havent fixed what happen when the code has an assign statement in the middle */
    // do the register thing
    // what happen if more than 7
    // maybe idx as parameter?
    int check = 0;
    int lv = 0, rv = 0;
    // not finished yett ;CC
    if(root!= NULL){
        switch(root->data){
            case ID:
                // problem is what happen if it's a new varaibel on the left side
                // do I just put the val on the table for cheat way ? can I do that 
                
                //printf("%slol\n", root->lexeme);
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
                    // why put + and - here? sdsf
                    // valid if it
                check = evaluateTree(root->right);
                if(check == 0){
                    checkID(root->right, &check);
                }
                // pass the check in the function or global array??or like before ths function is called or i can make a new function to pass the check around
                lv = print(root->left, idx, mem);
                rv = print(root->right, idx + 1, mem);
                if (strcmp(root->lexeme, "*") == 0) {
                    printf("MUL r%d r%d\n", lv, rv);
                } else if (strcmp(root->lexeme, "/") == 0) {
                    if (check == 0){
                        printf("EXIT 1\n");
                        //exit(0);
                        error(DIVZERO);
                    }
                    printf("DIV r%d r%d\n", lv, rv);
                }
                if(idx > 6){ // because when u backtrack it's - 1
                    *mem -=4;
                    printf("MOV r%d [%d]\n", (idx+1)%8,  *mem);

                }
                break;
            case ASSIGN:
                // assign here is = sign
                rv = print(root->right, idx, mem);
                // call the function again
                // maybe it's something like a + (b = 5) // and b hasn't existed yet
                // when there is an assign statement u want to set the value
                // this is why there is no case a = a + 5
                // check left
                /*if(*check == 2){
                    *check = 1;
                    break;
                }*/
                printf("MOV [%d] r%d\n", setmem(root->left->lexeme, 0), rv);
                break;
            case INCDEC:
                // not yet
                // not sure i did a mod 8 but im not sure
                rv = print(root->right, idx, mem);
                // should i put lv as 0 always?? wheneer it meets an incdec?
                if((idx + 1) > 7){
                    printf("MOV [%d] r%d\n", *mem,  (idx+1) % 8);
                    *mem += 4;
                }
                printf("MOV r%d 1\n", (idx + 1) % 8);
                /* pls fix im crying */
                // i have to make an assign
                // root->lexeme will always be ++ or --
                if(strcmp(root->lexeme, "--") == 0){
                    printf("SUB r%d r%d\n", rv, (idx + 1)%8);
                    //BRUH I HAVENT THOUGHT ABOUT THE CASE IF AN ASSIGN IS IN THE MIDDLE OF A STATEMENT AND MORE THAN USED REGISTER
                    // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
                    if((idx + 1) > 7){
                        // not - 1 cuz no backtrack?
                        *mem -=4;
                        printf("MOV r%d [%d]\n", (idx+1)%8,  *mem);
                    }
                    printf("MOV [%d] r%d\n", setmem(root->right->lexeme, 0), idx % 8);
                    // don't forget to somehow set it first to ID like setval
                } else if (strcmp(root->lexeme, "++") == 0){
                    printf("ADD r%d r%d\n", rv, (idx + 1) % 8);
                    if((idx + 1) > 7){
                        // not - 1 cuz no backtrack?
                        *mem -=4;
                        printf("MOV r%d [%d]\n", (idx+1)%8,  *mem);
                    }
                    printf("MOV [%d] r%d\n", setmem(root->right->lexeme, 0), idx % 8);
                }
                //not sure about this one

                break;
            case AND:
                lv = print(root->left, idx, mem);
                rv = print(root->right, idx+1, mem);
                if(strcmp(root->lexeme, "&") == 0){
                    printf("AND r%d r%d\n", lv, rv);
                }
                if(idx > 6){ // because when u backtrack it's - 1
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
                if(idx > 6){ // because when u backtrack it's - 1
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
                if(idx > 6){ // because when u backtrack it's - 1
                    *mem -=4;
                    printf("MOV r%d [%d]\n", (idx+1)%8,  *mem);

                }
                break;
            default:
            // IDONT THINK U NEED THIS BUT OK
                idx = 0;
        }
    }
    return (idx %8);
}