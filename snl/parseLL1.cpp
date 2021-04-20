/****************************************************/
/* �ļ� parseLL1.cpp								*/
/* ˵�� ��pascal���������﷨������ʵ��				*/
/* ���� �������ṹ��ԭ���ʵ��						*/
/* ˵�� ����LL1��������                             */
/****************************************************/


/***********  ���ļ���������ͷ�ļ�  ****************/

#include "globals.h"	/* ��ͷ�ļ�������ȫ��������� */

#include "util.h"		/* ��ͷ�ļ������˹��ܺ��� */

#include "string.h"

#include "scanner.h"	/* ��ͷ�ļ������˴ʷ�ɨ�������� */

#include "parseLL1.h"	/* ��ͷ�ļ��������﷨���������� */


/*��ǰ����*/
TokenType  currentToken;
/*��ǰ�����кţ����ڸ���������ʾ��Ϣ*/
extern int  lineno;
/*LL1������*/
int LL1Table[TABLESIZE][TABLESIZE];
/*��¼��ǰ�﷨���ڵ�*/
TreeNode* currentP = NULL;

/*Ϊ����������Ҫ����ʱ����*/
DecKind* temp = NULL;
/*���浱ǰָ�룬�Ա��޸ĺ󣬽���ָ�*/
TreeNode* saveP = NULL;

/*��¼���ʽ�У�δƥ�����������Ŀ*/
int  expflag = 0;

/*�жϼ򵥱��ʽ����������������ʽ�Ƿ��������ʶ*/
/*�����������ʽʱ��ȡ��ֵ���򵥱��ʽʱ��ȡ��ֵ*/
/*���ں���preocess84*/
int  getExpResult = TRUE;

/*����������������ʳ�ʼ��Ϊ�٣������������ʱ������
  �ı�Ϊ�棬�Ա��ں���process84�У����������ʽ����ʱ��
  ���﷨��ջ�е�����Ӧָ�룬�������±���ʽ�Ľṹ����
  �ڵ���*/
int  getExpResult2 = FALSE;


/*����ջ��ָ��*/
StackNode* StackTop;
/*ջ�ձ�־*/
int STACKEMPTY;

/*�﷨��ջ��ָ��*/
StackNodePA* StackTopPA;
/*ջ�ձ�־*/
int  paSTACKEMPTY;

/*������ջ��ջ��ָ��*/
StackNodeP* OpStackTop = NULL;
/*������ջ�ձ�־*/
int OpSTACKEMPTY = TRUE;

/*������ջ��ջ��ָ��*/
StackNodeP* NumStackTop = NULL;
/*������ջ�ձ�־*/
int NumSTACKEMPTY = TRUE;



/************<�﷨�������ܺ���> **************/

/********************************************************/
/* ������  CreatLL1Table								*/
/* ��  ��  ����LL1������								*/
/* ˵  ��  ��ʼ���飨���е�ÿһ�Ϊ0������LL1�ķ�   */
/*         �����鸳ֵ���������ú���ֵΪ0��		*/
/*         ��ʾ�޲���ʽ��ѡ��������Ϊѡ�еĲ���ʽ  		*/
/********************************************************/

void CreatLL1Table()
{
	int i, j;

	/*��ʼ��LL1��Ԫ��*/
	for (i = 0; i < TABLESIZE; i++)
		for (j = 0; j < TABLESIZE; j++)
			LL1Table[i][j] = 0;

	LL1Table[Program][PROGRAM] = 1;

	LL1Table[ProgramHead][PROGRAM] = 2;

	LL1Table[ProgramName][ID] = 3;

	LL1Table[DeclarePart][TYPE] = 4;
	LL1Table[DeclarePart][VAR] = 4;
	LL1Table[DeclarePart][PROCEDURE] = 4;
	LL1Table[DeclarePart][BEGIN] = 4;

	LL1Table[TypeDec][VAR] = 5;
	LL1Table[TypeDec][PROCEDURE] = 5;
	LL1Table[TypeDec][BEGIN] = 5;

	LL1Table[TypeDec][TYPE] = 6;

	LL1Table[TypeDeclaration][TYPE] = 7;

	LL1Table[TypeDecList][ID] = 8;

	LL1Table[TypeDecMore][VAR] = 9;
	LL1Table[TypeDecMore][PROCEDURE] = 9;
	LL1Table[TypeDecMore][BEGIN] = 9;


	LL1Table[TypeDecMore][ID] = 10;

	LL1Table[TypeId][ID] = 11;

	LL1Table[TypeName][INTEGER] = 12;
	LL1Table[TypeName][CHAR] = 12;

	LL1Table[TypeName][ARRAY] = 13;
	LL1Table[TypeName][RECORD] = 13;

	LL1Table[TypeName][ID] = 14;

	LL1Table[BaseType][INTEGER] = 15;

	LL1Table[BaseType][CHAR] = 16;

	LL1Table[StructureType][ARRAY] = 17;

	LL1Table[StructureType][RECORD] = 18;

	LL1Table[ArrayType][ARRAY] = 19;

	LL1Table[Low][INTC] = 20;

	LL1Table[Top][INTC] = 21;

	LL1Table[RecType][RECORD] = 22;

	LL1Table[FieldDecList][INTEGER] = 23;
	LL1Table[FieldDecList][CHAR] = 23;

	LL1Table[FieldDecList][ARRAY] = 24;

	LL1Table[FieldDecMore][END] = 25;

	LL1Table[FieldDecMore][INTEGER] = 26;
	LL1Table[FieldDecMore][CHAR] = 26;
	LL1Table[FieldDecMore][ARRAY] = 26;

	LL1Table[IdList][ID] = 27;

	LL1Table[IdMore][SEMI] = 28;

	LL1Table[IdMore][COMMA] = 29;

	LL1Table[VarDec][PROCEDURE] = 30;
	LL1Table[VarDec][BEGIN] = 30;

	LL1Table[VarDec][VAR] = 31;

	LL1Table[VarDeclaration][VAR] = 32;

	LL1Table[VarDecList][INTEGER] = 33;
	LL1Table[VarDecList][CHAR] = 33;
	LL1Table[VarDecList][ARRAY] = 33;
	LL1Table[VarDecList][RECORD] = 33;
	LL1Table[VarDecList][ID] = 33;

	LL1Table[VarDecMore][PROCEDURE] = 34;
	LL1Table[VarDecMore][BEGIN] = 34;


	LL1Table[VarDecMore][INTEGER] = 35;
	LL1Table[VarDecMore][CHAR] = 35;
	LL1Table[VarDecMore][ARRAY] = 35;
	LL1Table[VarDecMore][RECORD] = 35;
	LL1Table[VarDecMore][ID] = 35;

	LL1Table[VarIdList][ID] = 36;

	LL1Table[VarIdMore][SEMI] = 37;

	LL1Table[VarIdMore][COMMA] = 38;

	LL1Table[ProcDec][BEGIN] = 39;

	LL1Table[ProcDec][PROCEDURE] = 40;

	LL1Table[ProcDeclaration][PROCEDURE] = 41;

	LL1Table[ProcDecMore][BEGIN] = 42;

	LL1Table[ProcDecMore][PROCEDURE] = 43;

	LL1Table[ProcName][ID] = 44;

	LL1Table[ParamList][RPAREN] = 45;

	LL1Table[ParamList][INTEGER] = 46;
	LL1Table[ParamList][CHAR] = 46;
	LL1Table[ParamList][ARRAY] = 46;
	LL1Table[ParamList][RECORD] = 46;
	LL1Table[ParamList][ID] = 46;
	LL1Table[ParamList][VAR] = 46;

	LL1Table[ParamDecList][INTEGER] = 47;
	LL1Table[ParamDecList][CHAR] = 47;
	LL1Table[ParamDecList][ARRAY] = 47;
	LL1Table[ParamDecList][RECORD] = 47;
	LL1Table[ParamDecList][ID] = 47;
	LL1Table[ParamDecList][VAR] = 47;

	LL1Table[ParamMore][RPAREN] = 48;

	LL1Table[ParamMore][SEMI] = 49;

	LL1Table[Param][INTEGER] = 50;
	LL1Table[Param][CHAR] = 50;
	LL1Table[Param][ARRAY] = 50;
	LL1Table[Param][RECORD] = 50;
	LL1Table[Param][ID] = 50;

	LL1Table[Param][VAR] = 51;

	LL1Table[FormList][ID] = 52;

	LL1Table[FidMore][SEMI] = 53;
	LL1Table[FidMore][RPAREN] = 53;

	LL1Table[FidMore][COMMA] = 54;

	LL1Table[ProcDecPart][TYPE] = 55;
	LL1Table[ProcDecPart][VAR] = 55;
	LL1Table[ProcDecPart][PROCEDURE] = 55;
	LL1Table[ProcDecPart][BEGIN] = 55;

	LL1Table[ProcBody][BEGIN] = 56;

	LL1Table[ProgramBody][BEGIN] = 57;

	LL1Table[StmList][ID] = 58;
	LL1Table[StmList][IF] = 58;
	LL1Table[StmList][WHILE] = 58;
	LL1Table[StmList][RETURN] = 58;
	LL1Table[StmList][READ] = 58;
	LL1Table[StmList][WRITE] = 58;

	LL1Table[StmMore][END] = 59;
	LL1Table[StmMore][ENDWH] = 59;
	LL1Table[StmMore][ELSE] = 59;
	LL1Table[StmMore][FI] = 59;

	LL1Table[StmMore][SEMI] = 60;

	LL1Table[Stm][IF] = 61;

	LL1Table[Stm][WHILE] = 62;

	LL1Table[Stm][READ] = 63;

	LL1Table[Stm][WRITE] = 64;

	LL1Table[Stm][RETURN] = 65;

	LL1Table[Stm][ID] = 66;

	LL1Table[AssCall][ASSIGN] = 67;
	LL1Table[AssCall][LMIDPAREN] = 67;
	LL1Table[AssCall][DOT] = 67;


	LL1Table[AssCall][LPAREN] = 68;

	LL1Table[AssignmentRest][ASSIGN] = 69;
	LL1Table[AssignmentRest][LMIDPAREN] = 69;
	LL1Table[AssignmentRest][DOT] = 69;

	LL1Table[ConditionalStm][IF] = 70;


	LL1Table[LoopStm][WHILE] = 71;

	LL1Table[InputStm][READ] = 72;

	LL1Table[InVar][ID] = 73;

	LL1Table[OutputStm][WRITE] = 74;

	LL1Table[ReturnStm][RETURN] = 75;

	LL1Table[CallStmRest][LPAREN] = 76;

	LL1Table[ActParamList][RPAREN] = 77;

	LL1Table[ActParamList][ID] = 78;
	LL1Table[ActParamList][INTC] = 78;
	LL1Table[ActParamList][LPAREN] = 78;

	LL1Table[ActParamMore][RPAREN] = 79;

	LL1Table[ActParamMore][COMMA] = 80;

	LL1Table[RelExp][LPAREN] = 81;
	LL1Table[RelExp][INTC] = 81;
	LL1Table[RelExp][ID] = 81;

	LL1Table[OtherRelE][LT] = 82;
	LL1Table[OtherRelE][EQ] = 82;

	LL1Table[Exp][LPAREN] = 83;
	LL1Table[Exp][INTC] = 83;
	LL1Table[Exp][ID] = 83;

	LL1Table[OtherTerm][LT] = 84;
	LL1Table[OtherTerm][EQ] = 84;
	LL1Table[OtherTerm][THEN] = 84;
	LL1Table[OtherTerm][DO] = 84;
	LL1Table[OtherTerm][RPAREN] = 84;
	LL1Table[OtherTerm][END] = 84;
	LL1Table[OtherTerm][SEMI] = 84;
	LL1Table[OtherTerm][COMMA] = 84;
	LL1Table[OtherTerm][ENDWH] = 84;
	LL1Table[OtherTerm][ELSE] = 84;
	LL1Table[OtherTerm][FI] = 84;
	LL1Table[OtherTerm][RMIDPAREN] = 84;


	LL1Table[OtherTerm][PLUS] = 85;
	LL1Table[OtherTerm][MINUS] = 85;

	LL1Table[Term][LPAREN] = 86;
	LL1Table[Term][INTC] = 86;
	LL1Table[Term][ID] = 86;

	LL1Table[OtherFactor][PLUS] = 87;
	LL1Table[OtherFactor][MINUS] = 87;
	LL1Table[OtherFactor][LT] = 87;
	LL1Table[OtherFactor][EQ] = 87;
	LL1Table[OtherFactor][THEN] = 87;
	LL1Table[OtherFactor][ELSE] = 87;
	LL1Table[OtherFactor][FI] = 87;
	LL1Table[OtherFactor][DO] = 87;
	LL1Table[OtherFactor][ENDWH] = 87;
	LL1Table[OtherFactor][RPAREN] = 87;
	LL1Table[OtherFactor][END] = 87;
	LL1Table[OtherFactor][SEMI] = 87;
	LL1Table[OtherFactor][COMMA] = 87;
	LL1Table[OtherFactor][RMIDPAREN] = 87;

	LL1Table[OtherFactor][TIMES] = 88;
	LL1Table[OtherFactor][OVER] = 88;

	LL1Table[Factor][LPAREN] = 89;

	LL1Table[Factor][INTC] = 90;

	LL1Table[Factor][ID] = 91;

	LL1Table[Variable][ID] = 92;

	LL1Table[VariMore][ASSIGN] = 93;
	LL1Table[VariMore][TIMES] = 93;
	LL1Table[VariMore][OVER] = 93;
	LL1Table[VariMore][PLUS] = 93;
	LL1Table[VariMore][MINUS] = 93;
	LL1Table[VariMore][LT] = 93;
	LL1Table[VariMore][EQ] = 93;
	LL1Table[VariMore][THEN] = 93;
	LL1Table[VariMore][ELSE] = 93;
	LL1Table[VariMore][FI] = 93;
	LL1Table[VariMore][DO] = 93;
	LL1Table[VariMore][ENDWH] = 93;
	LL1Table[VariMore][RPAREN] = 93;
	LL1Table[VariMore][END] = 93;
	LL1Table[VariMore][SEMI] = 93;
	LL1Table[VariMore][COMMA] = 93;
	LL1Table[VariMore][RMIDPAREN] = 93;

	LL1Table[VariMore][LMIDPAREN] = 94;

	LL1Table[VariMore][DOT] = 95;

	LL1Table[FieldVar][ID] = 96;

	LL1Table[FieldVarMore][ASSIGN] = 97;
	LL1Table[FieldVarMore][TIMES] = 97;
	LL1Table[FieldVarMore][OVER] = 97;
	LL1Table[FieldVarMore][PLUS] = 97;
	LL1Table[FieldVarMore][MINUS] = 97;
	LL1Table[FieldVarMore][LT] = 97;
	LL1Table[FieldVarMore][EQ] = 97;
	LL1Table[FieldVarMore][THEN] = 97;
	LL1Table[FieldVarMore][ELSE] = 97;
	LL1Table[FieldVarMore][FI] = 97;
	LL1Table[FieldVarMore][DO] = 97;
	LL1Table[FieldVarMore][ENDWH] = 97;
	LL1Table[FieldVarMore][RPAREN] = 97;
	LL1Table[FieldVarMore][END] = 97;
	LL1Table[FieldVarMore][SEMI] = 97;
	LL1Table[FieldVarMore][COMMA] = 97;

	LL1Table[FieldVarMore][LMIDPAREN] = 98;

	LL1Table[CmpOp][LT] = 99;

	LL1Table[CmpOp][EQ] = 100;

	LL1Table[AddOp][PLUS] = 101;

	LL1Table[AddOp][MINUS] = 102;

	LL1Table[MultOp][TIMES] = 103;

	LL1Table[MultOp][OVER] = 104;

}

/********************************************************************/
/* ������ gettoken					     							*/
/* ��  �� ��Token������ȡ��һ��Token	                            */
/* ˵  �� ���ļ��д��Token����������ȡһ�����ʣ���Ϊ��ǰ����.      */
/********************************************************************/
int fpnum = 0;
void gettoken(TokenType* p)
{
	FILE* fp2;
	/*��ֻ����ʽ���ļ�*/
	fp2 = fopen("Tokenlist", "rb");
	if (fp == NULL)
	{
		printf("cannot create file Tokenlist!\n");
		exit(0);
	}
	fseek(fp2, fpnum * sizeof(TokenType), 0);
	fread(p, sizeof(TokenType), 1, fp2);
	fpnum++;
	fclose(fp2);
}


/********************************************************************/
/* ������ syntaxError												*/
/* ��  �� �﷨��������											*/
/* ˵  �� ����������messageָ���Ĵ�����Ϣ��ʽ��д���б��ļ�listing	*/
/*		  ���ô���׷�ٱ�־ErrorΪTRUE								*/
/********************************************************************/
static void syntaxError(char* message)

{
	fprintf(listing, "\n>>> ");

	/* �������к�lineno���﷨������Ϣmessage��ʽ��д���ļ�listing */
	fprintf(listing, "Syntax error at line %d: %s", lineno, message);

	/* ���ô���׷�ٱ�־ErrorΪTRUE,��ֹ�����һ������ */
	Error = TRUE;
}

void process1()
{
	Push(2, DOT);
	Push(1, ProgramBody);
	Push(1, DeclarePart);
	Push(1, ProgramHead);

}

/********************************************************************/
/* ������ process2() 				     							*/
/* ��  �� �������ͷ�������ɳ���ͷ�ڵ�Phead.                        */
/* ˵  �� ����ʽΪ��PROGRAM  ProgramName							*/
/********************************************************************/
void process2()
{
	Push(1, ProgramName);
	Push(2, PROGRAM);

	TreeNode** t = PopPA();
	currentP = newPheadNode();
	(*t) = currentP;
	/*����ͷ�ڵ�û���ֵܽڵ㣬������������ø��ڵ��child[1]ָ��*/

}

void process3()
{
	Push(2, ID);

	strcpy(currentP->name[0], currentToken.Sem);
	currentP->idnum++;
}

void process4()
{
	Push(1, ProcDec);
	Push(1, VarDec);
	Push(1, TypeDec);
}
void process5()
{
}
void process6()
{
	Push(1, TypeDeclaration);

}

void process7()
{
	Push(1, TypeDecList);
	Push(2, TYPE);

	TreeNode** t = PopPA();
	currentP = newTypeNode();  /*����Type��Ϊ��־�Ľ�㣬�����ӽ�㶼��
								 ��������*/
	(*t) = currentP;    /*ͷ�����ֵܽ��ָ��ָ��˽��*/
	PushPA(&((*currentP).sibling));  /* ѹ��ָ����������ڵ��ָ��*/
	PushPA(&((*currentP).child[0])); /*ѹ��ָ���һ�����������ڵ��ָ��*/
}

void process8()
{
	Push(1, TypeDecMore);
	Push(2, SEMI);
	Push(1, TypeName);
	Push(2, EQ);
	Push(1, TypeId);

	TreeNode** t = PopPA();
	currentP = newDecNode(); /*����һ����ʾ���������Ľ�㣬
							   �����κ���Ϣ*/

	(*t) = currentP; /*���ǵ�һ��������Type���͵��ӽ��ָ��ǰ��㣬
					   ��������һ�������������ֵܽ��*/

	PushPA(&((*currentP).sibling));
}

void process9()
{
	PopPA();
}

void process10()
{
	Push(1, TypeDecList);

}

void process11()
{
	Push(2, ID);

	strcpy((*currentP).name[0], currentToken.Sem);
	currentP->idnum++;
}

void process12()
{
	Push(1, BaseType);

	/*������������͵����⣬���ﲻ��ֱ����currentP->kind.dec=IntegerK;
	  ��Ӧ����ô��������Ӧ��������*/
	temp = &(currentP->kind.dec);

}
void process13()
{
	Push(1, StructureType);
}

void process14()
{
	Push(2, ID);

	/*���������Ͳ���Ϊ���ͱ�ʶ��*/
	(*currentP).kind.dec = IdK;
	strcpy(currentP->attr.type_name, currentToken.Sem);
}

void process15()
{
	Push(2, INTEGER);

	/*���������Ͳ���Ϊ��������*/
	(*temp) = IntegerK;
}

void process16()
{
	Push(2, CHAR);

	/*���������Ͳ���Ϊ�ӷ�����*/
	(*temp) = CharK;
}
void process17()
{
	Push(1, ArrayType);
}
void process18()
{
	Push(1, RecType);
}
void process19()
{
	Push(1, BaseType);
	Push(2, OF);
	Push(2, RMIDPAREN);
	Push(1, Top);
	Push(2, UNDERANGE);
	Push(1, Low);
	Push(2, LMIDPAREN);
	Push(2, ARRAY);

	/*����������Ϊ��������*/
	(*currentP).kind.dec = ArrayK;
	temp = &(currentP->attr.ArrayAttr.childtype);

}
void process20()
{
	Push(2, INTC);

	/*�洢������½�*/
	(*currentP).attr.ArrayAttr.low = atoi(currentToken.Sem);
}
void process21()
{
	Push(2, INTC);

	/*�洢������Ͻ�*/
	(*currentP).attr.ArrayAttr.up = atoi(currentToken.Sem);
}
void process22()
{
	Push(2, END);
	Push(1, FieldDecList);
	Push(2, RECORD);

	/*���������Ͳ���Ϊ��¼����*/
	(*currentP).kind.dec = RecordK;

	saveP = currentP; /*ѹ�뵱ǰ�ڵ㣬��Ϊ�˴������ص���ǰ�ڵ㣬
					  ��Ҫ��Ϊ�˱����������ֱ�ʶ�������ں��棬����*/
	PushPA(&((*currentP).child[0]));
}
void process23()
{
	Push(1, FieldDecMore);
	Push(2, SEMI);
	Push(1, IdList);
	Push(1, BaseType);

	TreeNode** t = PopPA();
	currentP = newDecNode(); /*���ɼ�¼���͵���һ���򣬲����κ���Ϣ*/
	temp = (&(currentP->kind.dec));
	(*t) = currentP; /*���ǵ�һ��������record���͵��ӽ��ָ��ǰ��㣬
						��������һ����¼���������ֵܽ��*/
	PushPA(&((*currentP).sibling));

}

void process24()
{
	Push(1, FieldDecMore);
	Push(2, SEMI);
	Push(1, IdList);
	Push(1, ArrayType);

	TreeNode** t = PopPA();
	currentP = newDecNode(); /*���ɼ�¼���͵���һ���򣬲����κ���Ϣ*/
	(*t) = currentP; /*���ǵ�һ��������record���͵��ӽ��ָ��ǰ��㣬
					   ��������һ����¼���������ֵܽ��*/
	PushPA(&((*currentP).sibling));

}

void process25()
{
	/*����û�м�¼���͵���һ�����ˣ��ָ���ǰ��¼���ͽڵ��ָ��*/
	PopPA();
	currentP = saveP;
}

void process26()
{
	Push(1, FieldDecList);

}
void process27()
{
	Push(1, IdMore);
	Push(2, ID);

	/*��¼һ�����и���������������Ϣ*/
	strcpy(currentP->name[currentP->idnum], currentToken.Sem);
	currentP->idnum++;
}
void process28()
{
}

void process29()
{
	Push(1, IdList);
	Push(2, COMMA);

}
void process30()
{
}

void process31()
{
	Push(1, VarDeclaration);

}
void process32()
{
	Push(1, VarDecList);
	Push(2, VAR);

	currentP = newVarNode();   /*����һ����־���������Ľڵ�*/
	TreeNode** t = PopPA();
	(*t) = currentP;
	PushPA(&((*currentP).sibling));  /*ѹ��ָ����������ָ��*/
	PushPA(&((*currentP).child[0])); /*ѹ��ָ���һ�����������ڵ��ָ��*/
}
void process33()
{
	Push(1, VarDecMore);
	Push(2, SEMI);
	Push(1, VarIdList);
	Push(1, TypeName);

	TreeNode** t = PopPA();
	currentP = newDecNode();/*����һ���µ������ڵ㣬�����ʾ��������*/
	(*t) = currentP;       /*���ǵ�һ���ڵ㣬�����������ͷָ��ָ������
							 ��������ǰһ�����������ĺ��*/
	PushPA(&((*currentP).sibling));

}
void process34()
{
	PopPA();
}

void process35()
{
	Push(1, VarDecList);

}
void process36()
{
	Push(1, VarIdMore);
	Push(2, ID);

	strcpy(currentP->name[currentP->idnum], currentToken.Sem);
	currentP->idnum++;

}
void process37()
{
}

void process38()
{
	Push(1, VarIdList);
	Push(2, COMMA);

}
void process39()
{
}

void process40()
{
	Push(1, ProcDeclaration);

}
void process41()
{
	Push(1, ProcDecMore);
	Push(1, ProcBody);
	Push(1, ProcDecPart);
	Push(2, SEMI);
	Push(2, RPAREN);
	Push(1, ParamList);
	Push(2, LPAREN);
	Push(1, ProcName);
	Push(2, PROCEDURE);


	currentP = newProcNode();
	TreeNode** t = PopPA();
	(*t) = currentP;

	PushPA(&(currentP->sibling));

	PushPA(&(currentP->child[2])); /*ָ���������*/

	PushPA(&(currentP->child[1]));  /*ָ��������������*/

	PushPA(&(currentP->child[0]));  /*ָ�������������*/
}

void process42()
{   /*�������̽ڵ���ֵܽڵ�ָ��*/
	//PopPA( );  /*Ϊ��ͳһ�����������ڵ���*/
}

void process43()
{
	Push(1, ProcDeclaration);
}

void process44()
{
	Push(2, ID);

	strcpy(currentP->name[0], currentToken.Sem);
	currentP->idnum++;
}

void process45()
{
	/*�ββ���Ϊ�գ�����ָ���βε�ָ��*/
	PopPA();
}

void process46()
{
	Push(1, ParamDecList);
}
void process47()
{
	Push(1, ParamMore);
	Push(1, Param);
}
void process48()
{
	PopPA();
}

void process49()
{
	Push(1, ParamDecList);
	Push(2, SEMI);

}
void process50()
{
	Push(1, FormList);
	Push(1, TypeName);

	TreeNode** t = PopPA();
	currentP = newDecNode();
	/*�����Ĳ���������ֵ����*/
	currentP->attr.ProcAttr.paramt = valparamType;
	(*t) = currentP;
	PushPA(&(currentP->sibling));


}
void process51()
{
	Push(1, FormList);
	Push(1, TypeName);
	Push(2, VAR);

	TreeNode** t = PopPA();
	currentP = newDecNode();
	/*�����Ĳ��������Ǳ�������*/
	currentP->attr.ProcAttr.paramt = varparamType;
	(*t) = currentP;
	PushPA(&(currentP->sibling));

}
void process52()
{
	Push(1, FidMore);
	Push(2, ID);

	strcpy(currentP->name[currentP->idnum], currentToken.Sem);
	currentP->idnum++;

}
void process53()
{
}

void process54()
{
	Push(1, FormList);
	Push(2, COMMA);
}

void process55()
{
	Push(1, DeclarePart);
}
void process56()
{
	Push(1, ProgramBody);
}

void process57()
{
	Push(2, END);
	Push(1, StmList);
	Push(2, BEGIN);

	/*ע�⣬��û���������֣��򵯳����ǳ������̸��ڵ���ָ��
	  �������ֵ�ָ��child[1];�����������֣��򵯳������������ǰ
	  �����һ��������ʶ�ڵ���ֵ�ָ�룻���������������������
	  ��Ҫ�����﷨��ջ�е�һ��ָ��*/
	PopPA();

	/*����������б�ʶ�ڵ�*/
	TreeNode** t = PopPA();
	currentP = newStmlNode();
	(*t) = currentP;
	PushPA(&(currentP->child[0]));

}
void process58()
{
	Push(1, StmMore);
	Push(1, Stm);
}
void process59()
{
	PopPA();
}

void process60()
{
	Push(1, StmList);
	Push(2, SEMI);

}
void process61()
{
	Push(1, ConditionalStm);

	currentP = newStmtNode(IfK);
	//currentP->kind.stmt=;

	TreeNode** t = PopPA();
	(*t) = currentP;
	PushPA(&currentP->sibling);


}
void process62()
{
	Push(1, LoopStm);

	currentP = newStmtNode(WhileK);
	//currentP->kind.stmt=;

	TreeNode** t = PopPA();
	(*t) = currentP;
	PushPA(&currentP->sibling);

}

void process63()
{
	Push(1, InputStm);

	TreeNode** t = PopPA();
	currentP = newStmtNode(ReadK);
	//currentP->kind.stmt=;
	(*t) = currentP;
	PushPA(&currentP->sibling);

}
void process64()
{
	Push(1, OutputStm);

	TreeNode** t = PopPA();
	currentP = newStmtNode(WriteK);
	//currentP->kind.stmt=;
	(*t) = currentP;
	PushPA(&currentP->sibling);

}
void process65()
{
	Push(1, ReturnStm);

	TreeNode** t = PopPA();
	currentP = newStmtNode(ReturnK);
	//currentP->kind.stmt=;
	(*t) = currentP;
	PushPA(&currentP->sibling);

}

void process66()
{
	Push(1, AssCall);
	Push(2, ID);

	currentP = newStmtNode(AssignK);

	/*��ֵ����󲿱����ڵ�*/
	TreeNode* t = newExpNode(VariK);
	strcpy(t->name[0], currentToken.Sem);
	t->idnum++;

	/*��ֵ����child[0]ָ���󲿵ı����ڵ�*/
	currentP->child[0] = t;

	TreeNode** t1 = PopPA();
	(*t1) = currentP;
	PushPA(&currentP->sibling);

}
void process67()
{
	Push(1, AssignmentRest);

	currentP->kind.stmt = AssignK;
}
void process68()
{
	Push(1, CallStmRest);
	/*���̵���������Ǳ�ʶ����������ʾ������*/
	currentP->child[0]->attr.ExpAttr.varkind = IdV;

	currentP->kind.stmt = CallK;
}
void process69()
{
	Push(1, Exp);
	Push(2, ASSIGN);
	Push(1, VariMore);
	/*ѹ��ָ��ֵ�Ҳ���ָ��*/
	PushPA(&(currentP->child[1]));
	/*��ǰָ��ָ��ֵ��*/
	currentP = currentP->child[0];

	TreeNode* t = newExpNode(OpK);
	t->attr.ExpAttr.op = END;
	PushOp(t);  //������ջ��ջ�״���һ������Ĳ�������Ϊ��־
}
void process70()
{
	Push(2, FI);
	Push(1, StmList);
	Push(2, ELSE);
	Push(1, StmList);
	Push(2, THEN);
	Push(1, RelExp);
	Push(2, IF);

	PushPA(&(currentP->child[2]));
	PushPA(&(currentP->child[1]));
	PushPA(&(currentP->child[0]));

}

void process71()
{
	Push(2, ENDWH);
	Push(1, StmList);
	Push(2, DO);
	Push(1, RelExp);
	Push(2, WHILE);

	PushPA(&(currentP->child[1]));
	PushPA(&(currentP->child[0]));

}

void process72()
{
	Push(2, RPAREN);
	Push(1, InVar);
	Push(2, LPAREN);
	Push(2, READ);
}
void process73()
{
	Push(2, ID);

	strcpy(currentP->name[0], currentToken.Sem);
	currentP->idnum++;
}
void process74()
{
	Push(2, RPAREN);
	Push(1, Exp);
	Push(2, LPAREN);
	Push(2, WRITE);

	PushPA(&(currentP->child[0]));

	TreeNode* t = newExpNode(OpK);
	t->attr.ExpAttr.op = END;
	PushOp(t);  //������ջ��ջ�״���һ������Ĳ�������Ϊ��־
}
void process75()
{
	Push(2, RETURN);
}

void process76()
{
	Push(2, RPAREN);
	Push(1, ActParamList);
	Push(2, LPAREN);

	PushPA(&(currentP->child[1]));

}
void process77()
{
	PopPA();
}
void process78()
{
	Push(1, ActParamMore);
	Push(1, Exp);

	TreeNode* t = newExpNode(OpK);
	t->attr.ExpAttr.op = END;
	PushOp(t);  //������ջ��ջ�״���һ������Ĳ�������Ϊ��־
}

void process79()
{
}
void process80()
{
	Push(1, ActParamList);
	Push(2, COMMA);

	PushPA(&(currentP->sibling));
}


/*�趨�����������ȼ���ֵԽ�����ȼ�Խ��*/
int  Priosity(LexType  op)
{
	int  pri = 0;
	switch (op)
	{
	case END:
		pri = -1; break;//ջ�ױ�ʶ�����ȼ����
	case LPAREN:
		pri = 0;
	case LT:
	case EQ:
		pri = 1; break;
	case PLUS:
	case MINUS:
		pri = 2; break;
	case TIMES:
	case OVER:
		pri = 3; break;
	default: {fprintf(listing, "no such  operator !");
		fprintf(listing, "%d\n", op);
		pri = -1;
	}
	}
	return  pri;
}

/*********************���ʽ����************************/

void process81()
{
	Push(1, OtherRelE);
	Push(1, Exp);

	TreeNode* t = newExpNode(OpK);
	t->attr.ExpAttr.op = END;
	PushOp(t);  //������ջ��ջ�״���һ������Ĳ�������Ϊ��־	

	getExpResult = FALSE;
}

void process82()
{
	Push(1, Exp);
	Push(1, CmpOp);

	TreeNode* currentP = newExpNode(OpK);
	currentP->attr.ExpAttr.op = currentToken.Lex;

	LexType  sTop = ReadOpStack();
	while (Priosity(sTop) >= Priosity(currentToken.Lex))
		/*���������ջ������������ȼ����ڻ���ڵ�ǰ�����Ĳ�����*/
	{
		TreeNode* t = PopOp();
		TreeNode* Rnum = PopNum();
		TreeNode* Lnum = PopNum();
		t->child[1] = Rnum;
		t->child[0] = Lnum;
		PushNum(t);

		sTop = ReadOpStack();
	}

	PushOp(currentP);
	/*�������ϵ�������Ҳ��ı��ʽʱ��Ҫ���﷨��ջ����
	  ����getExpResultΪ��*/
	getExpResult = TRUE;
}

void process83()
{
	Push(1, OtherTerm);
	Push(1, Term);
}

void process84()
{
	if ((currentToken.Lex == RPAREN) && (expflag != 0))
		//˵����ǰ�������Ǳ��ʽ�е�һ����
	{
		while (ReadOpStack() != LPAREN)
		{
			TreeNode* t = PopOp();
			TreeNode* Rnum = PopNum();
			TreeNode* Lnum = PopNum();

			t->child[1] = Rnum;
			t->child[0] = Lnum;
			PushNum(t);
		}
		PopOp(); //����������
		expflag--;
	}
	else
	{
		if ((getExpResult) || (getExpResult2))
		{
			while (ReadOpStack() != END)
			{
				TreeNode* t = PopOp();
				TreeNode* Rnum = PopNum();
				TreeNode* Lnum = PopNum();

				t->child[1] = Rnum;
				t->child[0] = Lnum;
				PushNum(t);
			}
			PopOp();//����ջ�ױ�־
			currentP = PopNum();

			TreeNode** t = PopPA();
			(*t) = currentP;

			/*�����������������־�ָ���ʼֵ�٣�
			  ������һ�������±���ʽʱ���ٽ�������Ϊ��ֵ*/
			if (getExpResult2 == TRUE)
				getExpResult2 = FALSE;
		}
	}
}

void process85()
{
	Push(1, Exp);
	Push(1, AddOp);

	TreeNode* currentP = newExpNode(OpK);
	currentP->attr.ExpAttr.op = currentToken.Lex;
	LexType  sTop = ReadOpStack();
	while (Priosity(sTop) >= Priosity(currentToken.Lex))
		/*���������ջ������������ȼ����ڻ���ڵ�ǰ�����Ĳ�����*/
	{
		TreeNode* t = PopOp();
		TreeNode* Rnum = PopNum();
		TreeNode* Lnum = PopNum();
		t->child[1] = Rnum;
		t->child[0] = Lnum;
		PushNum(t);
		sTop = ReadOpStack();
	}
	PushOp(currentP);
}

void process86()
{
	Push(1, OtherFactor);
	Push(1, Factor);
}
void process87()
{
}
void process88()
{
	Push(1, Term);
	Push(1, MultOp);

	TreeNode* currentP = newExpNode(OpK);
	currentP->attr.ExpAttr.op = currentToken.Lex;

	LexType  sTop = ReadOpStack();
	while (Priosity(sTop) >= Priosity(currentToken.Lex))
		/*���������ջ������������ȼ����ڻ���ڵ�ǰ�����Ĳ�����*/
	{
		TreeNode* t = PopOp();
		TreeNode* Rnum = PopNum();
		TreeNode* Lnum = PopNum();
		t->child[1] = Rnum;
		t->child[0] = Lnum;
		PushNum(t);

		sTop = ReadOpStack();
	}
	PushOp(currentP);
}

void process89()
{
	Push(2, RPAREN);
	Push(1, Exp);
	Push(2, LPAREN);

	TreeNode* t = newExpNode(OpK);
	t->attr.ExpAttr.op = currentToken.Lex; /*��������Ҳѹ��ջ��*/
	PushOp(t);
	expflag++;
}

void process90()
{
	Push(2, INTC);

	TreeNode* t = newExpNode(ConstK);
	t->attr.ExpAttr.val = atoi(currentToken.Sem);
	/*�����ڵ��������ջ*/
	PushNum(t);

}

void process91()
{
	Push(1, Variable);
}

void process92()
{
	Push(1, VariMore);
	Push(2, ID);

	currentP = newExpNode(VariK);
	strcpy(currentP->name[0], currentToken.Sem);
	currentP->idnum++;
	/*�����ڵ��������ջ*/
	PushNum(currentP);

}

void process93()
{
	/*��ʶ������*/
	currentP->attr.ExpAttr.varkind = IdV;
}

void process94()
{
	Push(2, RMIDPAREN);
	Push(1, Exp);
	Push(2, LMIDPAREN);
	/*�����Ա����*/
	currentP->attr.ExpAttr.varkind = ArrayMembV;
	PushPA(&currentP->child[0]);

	/*Ҫ������ʽ������ʼ��������ջ*/
	//������ջ��ջ�״���һ������Ĳ�������Ϊ��־
	TreeNode* t = newExpNode(OpK);
	t->attr.ExpAttr.op = END;
	PushOp(t);

	/*Ҫ���������±���ʽ�����ں���process84�����У�Ҫ
	  �����﷨��ջ���ʽ���־getExpResult2����Ϊ��ֵ*/
	getExpResult2 = TRUE;


}

void process95()
{
	Push(1, FieldVar);
	Push(2, DOT);
	/*���Ա����*/
	currentP->attr.ExpAttr.varkind = FieldMembV;
	PushPA(&currentP->child[0]);
}

void process96()
{
	Push(1, FieldVarMore);
	Push(2, ID);

	/*��¼��ĳ�Ա*/
	currentP = newExpNode(VariK);
	strcpy(currentP->name[0], currentToken.Sem);
	currentP->idnum++;

	TreeNode** t = PopPA();
	(*t) = currentP;


}

void process97()
{
	/*���Ա�Ǳ�ʶ������*/
	currentP->attr.ExpAttr.varkind = IdV;
}

void process98()
{
	Push(2, RMIDPAREN);
	Push(1, Exp);
	Push(2, LMIDPAREN);
	/*���Ա���������*/
	currentP->attr.ExpAttr.varkind = ArrayMembV;
	/*ָ�������Ա���ʽ*/
	PushPA(&currentP->child[0]);

	//������ջ��ջ�״���һ������Ĳ�������Ϊ��־
	TreeNode* t = newExpNode(OpK);
	t->attr.ExpAttr.op = END;
	PushOp(t);

	/*Ҫ���������±���ʽ�����ں���process84�����У�Ҫ
	  �����﷨��ջ���ʽ���־getExpResult2����Ϊ��ֵ*/
	getExpResult2 = TRUE;
}
void process99()
{
	Push(2, LT);
}

void process100()
{
	Push(2, EQ);
}

void process101()
{
	Push(2, PLUS);
}

void process102()
{
	Push(2, MINUS);
}

void process103()
{
	Push(2, TIMES);
}

void process104()
{
	Push(2, OVER);
}


/****************************************************/
/* ������  predict									*/
/* ��  ��  ѡ�����ʽ����							*/
/* ˵  ��  										    */
/****************************************************/
void predict(int num)
{
	switch (num)
	{
	case 1:     process1();	break;
	case 2:     process2();	break;
	case 3:     process3();	break;
	case 4:     process4();   break;
	case 5:	  process5();   break;
	case 6:	  process6();	break;
	case 7:	  process7();	break;
	case 8:	  process8();	break;
	case 9:	  process9();   break;
	case 10:	  process10();	break;
	case 11:	  process11();	break;
	case 12:	  process12();	break;
	case 13:	  process13();	break;
	case 14:	  process14();	break;
	case 15:	  process15();	break;
	case 16:	  process16();	break;
	case 17:	  process17();	break;
	case 18:	  process18();	break;
	case 19:	  process19();	break;
	case 20:	  process20();	break;
	case 21:	  process21();	break;
	case 22:	  process22();  break;
	case 23:	  process23();  break;
	case 24:	  process24();	break;
	case 25:	  process25();  break;
	case 26:	  process26();  break;
	case 27:	  process27();  break;
	case 28:	  process28();  break;
	case 29:	  process29();	break;
	case 30:	  process30();	break;
	case 31:	  process31();  break;
	case 32:	  process32();  break;
	case 33:	  process33();	break;
	case 34:	  process34();  break;
	case 35:	  process35();  break;

	case 36:	  process36();  break;
	case 37:	  process37();  break;
	case 38:	  process38();	break;
	case 39:	  process39();	break;
	case 40:	  process40();  break;

	case 41:	  process41();  break;
	case 42:	  process42();	break;
	case 43:	  process43();  break;
	case 44:	  process44();  break;
	case 45:	  process45();	break;

	case 46:    process46();  break;
	case 47:	  process47();	break;
	case 48:	  process48();  break;
	case 49:	  process49();  break;
	case 50:	  process50();	break;

	case 51:	  process51();  break;
	case 52:	  process52();	break;
	case 53:	  process53();	break;
	case 54:	  process54();  break;
	case 55:	  process55();  break;
	case 56:	  process56();	break;

	case 57:	  process57();  break;
	case 58:	  process58();	break;
	case 59:	  process59();	break;
	case 60:	  process60();	break;
	case 61:	  process61();	break;
	case 62:	  process62();	break;
	case 63:	  process63();	break;
	case 64:	  process64();	break;
	case 65:	  process65();	break;
	case 66:	  process66();	break;
	case 67:	  process67();	break;
	case 68:	  process68();	break;
	case 69:    process69();  break;
	case 70:    process70();  break;

	case 71:	  process71();	break;
	case 72:	  process72();	break;
	case 73:	  process73();  break;
	case 74:	  process74();  break;
	case 75:	  process75();  break;

	case 76:	  process76();  break;
	case 77:	  process77();	break;
	case 78:    process78();  break;
	case 79:    process79();  break;
	case 80:	  process80();  break;
	case 81:	  process81();  break;
	case 82:	  process82();	break;
	case 83:	  process83();  break;
	case 84:	  process84();  break;
	case 85:	  process85();	break;
	case 86:	  process86();  break;
	case 87:	  process87();  break;
	case 88:	  process88();	break;
	case 89:	  process89();  break;
	case 90:	  process90();	break;
	case 91:	  process91();	break;
	case 92:	  process92();	break;
	case 93:	  process93();	break;
	case 94:	  process94();	break;
	case 95:	  process95();	break;
	case 96:	  process96();	break;
	case 97:    process97();  break;
	case 98:    process98();  break;
	case 99:    process99();  break;
	case 100:   process100(); break;
	case 101:   process101(); break;
	case 102:   process102(); break;
	case 103:   process103(); break;
	case 104:   process104(); break;
	case 0:
	default: {
		syntaxError("unexpected token ->");
		fprintf(listing, "  %s", currentToken.Sem);
	}
	}
}

/****************************************************/
/* ������  parseLL1									*/
/* ��  ��  LL1�﷨����������						*/
/* ˵  ��  										    */
/****************************************************/

TreeNode* parseLL1()
{

	NontmlType  stacktopN;

	TmlType     stacktopT;

	/*�﷨���ĸ��ڵ�*/
	TreeNode* rootPointer = NULL;

	int pnum = 0; //��¼ѡ�еĲ���ʽ���

	CreatLL1Table();

	STACKEMPTY = TRUE;

	/*ָ�������﷨�����ڵ��ָ�룬�����õ��﷨��*/
	rootPointer = newRootNode();

	/*�����￪ʼ�����﷨�������﷨��������*/
	PushPA(&(rootPointer->child[2]));
	PushPA(&(rootPointer->child[1]));
	PushPA(&(rootPointer->child[0]));

	Push(1, Program);

	/*ȡһ��token*/
	gettoken(&currentToken);
	lineno = currentToken.lineshow;


	while (!(STACKEMPTY))
	{
		if (readStackflag() == 2) /*����ռ����Ƿ�ƥ��*/
		{
			stacktopT = readstackT();

			if (stacktopT == currentToken.Lex)
			{
				Pop();
				gettoken(&currentToken);
				lineno = currentToken.lineshow;

			}
			else
			{
				syntaxError("unexpected  token ->  ");
				fprintf(listing, "  %s", currentToken.Sem);
				fprintf(listing, "		");
				//printf("terminal not match!\n");
				//printf("%d\n",stacktopT);
				exit(0);
			}
		}
		else {  /*���ݷ��ռ�����ջ�з��Ž���Ԥ��*/
			stacktopN = readstackN();

			pnum = LL1Table[stacktopN][currentToken.Lex];
			Pop();
			//	if (0==pnum)
			//	{	printf("no predict!\n");
			//        printf("%d\n",stacktopN);
			//	}
			predict(pnum);

		}
	}
	if (currentToken.Lex != ENDFILE)
		syntaxError("Code  ends  before  file \n");

	return  rootPointer;

}




