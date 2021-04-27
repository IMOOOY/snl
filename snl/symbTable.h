#ifndef _SYMBTABLE_H_
#define _SYMBTABLE_H_

SymbTable* NewTable(void);
void  PrintOneLayer(int level);
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

