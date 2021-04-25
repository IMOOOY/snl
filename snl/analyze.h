/****************************************************/
/* �ļ� analyze.h									*/
/* ���� TINY����������������ܺ���					*/
/* ���� �������ṹ: ԭ���ʵ��						*/
/****************************************************/
/* �ж������־,���ͷ�ļ��Ѿ������������� */
#ifndef _ANALYZE_H_
#define _ANALYZE_H_


void initialize(void);

TypeIR* TypeProcess(TreeNode* t, DecKind deckind);

TypeIR* nameType(TreeNode* t);

TypeIR* arrayType(TreeNode* t);

TypeIR* recordType(TreeNode* t);


void TypeDecPart(TreeNode* t);

void VarDecPart(TreeNode* t);

void varDecList(TreeNode* t);

void  procDecPart(TreeNode* t);

SymbTable* HeadProcess(TreeNode* t);

ParamTable* ParaDecList(TreeNode* t);

void Body(TreeNode* t);

void statement(TreeNode* t);

TypeIR* Expr(TreeNode* t, AccessKind* Ekind);

TypeIR* arrayVar(TreeNode* t);

TypeIR* recordVar(TreeNode* t);

void assignstatement(TreeNode* t);

void callstatement(TreeNode* t);

void ifstatment(TreeNode* t);

void whilestatement(TreeNode* t);

void readstatement(TreeNode* t);

void writestatement(TreeNode* t);

void returnstatement(TreeNode* t);

void analyze(TreeNode* t);

#endif

