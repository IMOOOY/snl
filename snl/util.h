
/****************************************************/
/* �ļ� util.h										*/
/* ���� TINY�������Ĺ��ܺ���						*/
/* ���� �������ṹ: ԭ���ʵ��						*/
/****************************************************/

/* ���Ѿ������ͷ�ļ�util.h�������� */s
#ifndef _UTIL_H_
#define _UTIL_H_


void printTokenlist();

void ChainToFile(ChainNodeType* p);

void ReadNextToken(TokenType* p);

char* copyString(char* s);



/***********************************/
void Push(int i, int j);

void  Pop();

/*��ջ����־�������ռ������Ƿ��ռ���*/
int readStackflag();

/*���ռ���ʱ*/
NontmlType readstackN();

/*�ռ���*/
TmlType readstackT();

/*ѹջ��ʵ�ֹ��̣�ѹ�����ָ�����ڵ��ָ��ĵ�ַ*/
void  PushPA(TreeNode** t);

/*��ջ��ʵ�ֹ���*/
TreeNode** PopPA();

/*������ջ��ѹջʵ�ֹ��̣�ѹ�����ָ�����ڵ��ָ��*/
void  PushOp(TreeNode* t);

/*������ջ�ĵ�ջʵ�ֹ���*/
TreeNode* PopOp();

LexType ReadOpStack();

/*������ջ��ѹջʵ�ֹ��̣�ѹ�����ָ�����ڵ��ָ��*/
void  PushNum(TreeNode* t);

/*������ջ�ĵ�ջʵ�ֹ���*/
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
ArgRecord* NewTemp(AccessKind  access);

int  NewLabel();

ArgRecord* ARGAddr(char* id, int level, int off, AccessKind access);

ArgRecord* ARGLabel(int  label);

ArgRecord* ARGValue(int value);

CodeFile* GenCode(CodeKind codekind, ArgRecord* Arg1, ArgRecord* Arg2, ArgRecord* Arg3);

void  PrintCodeName(CodeKind kind);

void PrintContent(ArgRecord* arg);

void PrintOneCode(CodeFile* code);

void  PrintMidCode(CodeFile* firstCode);

int  DivBaseBlock();

void PrintBaseBlock(int  blocknum);

void   printValuNum();

void   printUsbleExpr();

void   printTempEqua();

void  PushLoop(LoopInfo* t);

LoopInfo* PopLoop();












void FindAdd(TreeNode* t);

void findSp(int varlevel);

void freeTree(TreeNode* t);

void freeDec(TreeNode* p);

void freeStm(TreeNode* p);

void freeExp(TreeNode* t);

void freeTable(void);

void freeMidCode(void);

#endif                                 