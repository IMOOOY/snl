/****************************************************/
/* 文件 symbTable.h									*/
/* 说明 TINY编译器的符号表界面	            		*/
/* 主题 编译器结构:原理和实例						*/
/****************************************************/

/* 判断联入标志,如该头文件已经联入则不再联入 */

#ifndef _SYMBTABLE_H_
#define _SYMBTABLE_H_

SymbTable* NewTable(void);

void CreatTable(void);

void DestroyTable(void);

int Enter(char* id, AttributeIR* attribP, SymbTable** entry);

int FindEntry(char* id, SymbTable** entry);

AttributeIR FindAttr(SymbTable* entry);

int Compat(TypeIR* tp1, TypeIR* tp2);

TypeIR* NewTy(TypeKind kind);

fieldChain* NewBody(void);

ParamTable* NewParam(void);

void ErrorPrompt(int line, char* name, char* message);

void printTab(int tabnum);


bool  FindField(char* Id, fieldChain* head, fieldChain** Entry);

void   PrintFieldChain(fieldChain* currentP);

void  PrintOneLayer(int level);

void   PrintSymbTable();


#endif

