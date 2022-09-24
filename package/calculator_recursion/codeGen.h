#ifndef __CODEGEN__
#define __CODEGEN__

#include "parser.h"

// Evaluate the syntax tree
extern int evaluateTree(BTNode *root);

// Print the syntax tree in prefix
extern void printPrefix(BTNode *root);
extern void PreCalTree(BTNode *root);
extern void printAssembly(BTNode *root);
extern void checkAssignIncdec(BTNode* root, int *check);
extern void checkID(BTNode* root, int *check);
extern int print(BTNode *root, int idx, int *mem);
#endif // __CODEGEN__
