/****************************************************/
/* �ļ� symbTable.h									*/
/* ˵�� TINY�������ķ��ű����	            		*/
/* ���� �������ṹ:ԭ���ʵ��						*/
/****************************************************/

/* �ж������־,���ͷ�ļ��Ѿ������������� */

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

//void printTable(void);

//void printTy(TypeIR * ty);

//void printVar(SymbTable * entry);

//void printProc(SymbTable * entry);


bool  FindField(char* Id, fieldChain* head, fieldChain** Entry);


/*��ӵ�*/
void   PrintFieldChain(fieldChain* currentP);

void  PrintOneLayer(int level);

void   PrintSymbTable();


#endif

