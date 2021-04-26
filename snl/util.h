
#ifndef _UTIL_H_
#define _UTIL_H_


void printTokenlist();

void ChainToFile(ChainNodeType* p);

void ReadNextToken(TokenType* p);

char* copyString(char* s);



/***********************************/
void Push(int i, int j);

void  Pop();

int readStackflag();

NontmlType readstackN();

/*终极符*/
TmlType readstackT();

/*压栈的实现过程，压入的是指向树节点的指针的地址*/
void  PushPA(TreeNode** t);

/*弹栈的实现过程*/
TreeNode** PopPA();

/*操作符栈的压栈实现过程，压入的是指向树节点的指针*/
void  PushOp(TreeNode* t);

/*操作符栈的弹栈实现过程*/
TreeNode* PopOp();

LexType ReadOpStack();

/*操作数栈的压栈实现过程，压入的是指向树节点的指针*/
void  PushNum(TreeNode* t);

/*操作数栈的弹栈实现过程*/
TreeNode* PopNum();

/*******************************/

TreeNode* newTypeNode();

TreeNode* newVarNode();



TreeNode* newRootNode(void);

TreeNode* newPheadNode(void);

TreeNode* newDecANode(NodeKind kind);

TreeNode* newDecNode(void);

TreeNode* newProcNode(void);

TreeNode* newStmlNode(void);

TreeNode* newStmtNode(StmtKind kind);


TreeNode* newExpNode(ExpKind kind);

void  printTree(TreeNode* tree);

static void printSpaces(void);


/*********************************/
//ArgRecord* NewTemp(AccessKind  access);

int  NewLabel();

//ArgRecord* ARGAddr(char* id, int level, int off, AccessKind access);

//ArgRecord* ARGLabel(int  label);

//ArgRecord* ARGValue(int value);

//CodeFile* GenCode(CodeKind codekind, ArgRecord* Arg1, ArgRecord* Arg2, ArgRecord* Arg3);

//void  PrintCodeName(CodeKind kind);

//void PrintContent(ArgRecord* arg);

//void PrintOneCode(CodeFile* code);

//void  PrintMidCode(CodeFile* firstCode);

int  DivBaseBlock();

void PrintBaseBlock(int  blocknum);

void   printValuNum();

void   printUsbleExpr();

void   printTempEqua();

//void  PushLoop(LoopInfo* t);

//LoopInfo* PopLoop();












void FindAdd(TreeNode* t);

void findSp(int varlevel);

void freeTree(TreeNode* t);

void freeDec(TreeNode* p);

void freeStm(TreeNode* p);

void freeExp(TreeNode* t);

void freeTable(void);

void freeMidCode(void);

#endif                                 
