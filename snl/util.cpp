/****************************************************/
/* �ļ�	util.cpp									*/
/* ˵�� ��PASCAL���Ա��������ܺ�����ʵ��			*/
/* ���� �������ṹ:ԭ���ʵ��						*/
/****************************************************/

/* ͷ�ļ�globals.h������ȫ����������� */
#include "globals.h"

#include "stdio.h"

#include "string.h"

#include "symbTable.h"

int  fp_num = 0;    /*���ʱ��¼token�����ı���*/



/*****************************************************************/
/* ������ printTokenlist     					        		 */
/* ��  �� ���ļ�tokenlist�е���Ϣ��Ϊ����ֵ                      */
/*        һ�㣬listingָ���׼�����                            */
/* ˵  �� ������ʾ�ʷ��������                                   */
/*****************************************************************/
void printTokenlist()
{
	TokenType  token;
	int m;

	fp = fopen("Tokenlist", "rb");  /*���ļ�*/

	if (fp == NULL)
	{
		printf("can not open the file:Tokenlist!\n");
		Error = TRUE;
		//exit(0);
	}

	for (m = 1; m <= Tokennum; m++)
	{
		fread(&token, TOKENLEN, 1, fp);

		fprintf(listing, "\t%d: ", token.lineshow);/*��ӡ�к�*/

		/* �Ժ�������Lex�������ʽ��з��ദ�� */
		switch (token.Lex)
		{
			/* ����tokenΪ������,�������ִ�Ԫ��ָ����ʽд���б��ļ�listing */
		case PROGRAM:
		case PROCEDURE:
		case TYPE:
		case VAR:
		case IF:
		case THEN:
		case ELSE:
		case FI:
		case INTEGER:
		case CHAR:
		case WHILE:
		case DO:
		case ENDWH:
		case BEGIN:
		case END:
		case READ:
		case WRITE:
		case ARRAY:
		case OF:
		case RECORD:
		case RETURN:

			fprintf(listing,     /*��ӡ������*/
				"reserved word: %s\n", token.Sem);
			break;

			/* ����tokenΪ������ţ�ASSIGN (��ֵ)����":="д���ļ�listing */
		case ASSIGN: fprintf(listing, ":=\n"); break;

			/* ����tokenΪ������ţ�LT (С��)����"<"д���ļ�listing */
		case LT: fprintf(listing, "<\n"); break;

			/* ����tokenΪ������ţ�EQ (����)����"="д���ļ�listing */
		case EQ: fprintf(listing, "=\n"); break;

			/* ����tokenΪ������ţ�LPAREN (������)����"("д���ļ�listing */
		case LPAREN: fprintf(listing, "(\n"); break;

			/* ����tokenΪ������ţ�RPAREN (������)����")"д���ļ�listing */
		case RPAREN: fprintf(listing, ")\n"); break;

			/* ����tokenΪ������ţ�SEMI (�ֺ�)����";"д���ļ�listing */
		case SEMI: fprintf(listing, ";\n"); break;

			/* ����tokenΪ������ţ�PLUS (�Ӻ�)����"+"д���ļ�listing */
		case PLUS: fprintf(listing, "+\n"); break;

			/* ����tokenΪ������ţ�MINUS (����)����"-"д���ļ�listing */
		case MINUS: fprintf(listing, "-\n"); break;

			/* ����tokenΪ������ţ�TIMES (�˺�)����"*"д���ļ�listing */
		case TIMES: fprintf(listing, "*\n"); break;

			/* ����tokenΪ������ţ�OVER (����)����"/"д���ļ�listing */
		case OVER: fprintf(listing, "/\n");  break;

		case DOT:  fprintf(listing, ".\n"); break;

		case COMMA: fprintf(listing, ",\n"); break;

		case LMIDPAREN: fprintf(listing, "[\n"); break;

		case RMIDPAREN: fprintf(listing, "]\n"); break;

		case UNDERANGE: fprintf(listing, "..\n"); break;

			/* ����tokenΪ���ǵ��ʷ��ţ�ENDFILE (�ļ���β)����EOFд���ļ�listing */
		case ENDFILE: fprintf(listing, "EOF\n"); break;

			/* ����tokenΪ���ַ����ʷ��ţ�NUM (����)������ֵд���ļ�listing */
		case INTC:
			fprintf(listing,
				"NUM, val= %s\n", token.Sem);
			break;

		case CHARC:
			fprintf(listing,
				"INCHAR, char=%s\n", token.Sem);
			break;

			/* ����tokenΪ���ַ����ʷ��ţ�ID (��ʶ��)������ʶ����д���ļ�listing */
		case ID:
			fprintf(listing,
				"ID, name= %s\n", token.Sem);
			break;

			/* ����tokenΪ���ǵ��ʷ��ţ�ERROR (����)����������Ϣд���ļ�listing */
		case ERROR:
			fprintf(listing,
				"ERROR: %s\n", token.Sem);
			break;

			/* ����tokenΪ����δ֪���ʣ�δ֪��Ϣд���ļ�listing,���������Ӧ���� */
		default:
			fprintf(listing, "Unknown token: %d\n", token.Lex);
			break;

		}
		fseek(fp, m * TOKENLEN, 0);
	}
	fprintf(listing, "\n");
	fclose(fp);
}

/*****************************************************************/
/* ������ ReadNextToken							        		 */
/* ��  �� ���ļ�tokenlist�е���Ϣ��Ϊ����ֵ                      */
/*        һ�㣬listingָ���׼�����                            */
/* ˵  �� ����ֵΪTokenType���ͣ������﷨������                  */
/*****************************************************************/
void ReadNextToken(TokenType* p)
{
	FILE* fp2;
	/*��ֻ����ʽ���ļ�*/
	fp2 = fopen("Tokenlist", "rb");
	if (fp == NULL)
	{
		printf("cannot create file Tokenlist!\n");
		Error = TRUE;
		//exit(0);
	}
	fseek(fp2, fp_num * sizeof(TokenType), 0);
	fread(p, sizeof(TokenType), 1, fp2);
	fp_num++;
	fclose(fp2);
}

/********************************************************/
/* ������ copyString									*/
/* ��  �� �ַ������ƺ���								*/
/* ˵  �� �ú���Ϊ�Ѵ��ڵ��ִ������ڴ浥Ԫ,�����临��	*/
/********************************************************/
char* copyString(char* s)

{
	int n;
	char* t;

	/* ��������s�����ִ�ΪNULL(��), ��������NULL */
	if (s == NULL) return NULL;

	/* ��������s�����ִ��ǿգ������ִ�s����+1������ʱ����n */
	n = strlen(s) + 1;

	/* ��̬�����ڴ浥Ԫ,ָ����Ԫ����Ϊn,tΪָ��õ�Ԫ��ָ�� */
	t = (char*)malloc(n);

	/* ��Ԫָ��tΪNULL(��),δ�ܳɹ�����			*
	 * ��������Ϣ���к�linenoд���б��ļ�listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* ���ÿ⺯��string.h,���Ƹ����ִ�s�����ִ���Ԫt */
	else strcpy(t, s);

	/* �������ظ��Ƶõ������ִ�ָ��t */
	return t;
}


/*****************************************************************/
/* ������ ChainToFile   										 */
/* ��  �� �������е�Token������δ����ļ���                      */
/* ˵  ��  ����p��ָ�������ָ��Token����ı�ͷ                  */
/*****************************************************************/
void ChainToFile(ChainNodeType* Chainhead)
{
	int num = 1;
	ChainNodeType* currentP = Chainhead;
	/*����һ���µ��ļ�"Tokenlist",�Դ洢Token����*/
	fp = fopen("Tokenlist", "wb+");
	if (fp == NULL)
	{
		printf("cannot create file Tokenlist!\n");
		Error = TRUE;
		//exit(0);
	}

	fp = fopen("Tokenlist", "ab");   /*��׷�ӷ�ʽ���ļ�*/
	if (fp == NULL)
	{
		printf("cannot open file Tokenlist!\n");
		Error = TRUE;
		//exit(0);
	}
	/*�ӱ�ͷ����β�����ν����е�Tokenд���ļ�*/
	do
	{
		fwrite(currentP, TOKENLEN, 1, fp);
		currentP = currentP->nextToken;
		num++;
	} while (currentP != NULL);

	fclose(fp);  /*�ر��ļ�*/
}

/********************************************************
 *********������LL1�﷨�����������õ���ջ����************
 ********************************************************/
 /*������ʵ��ջ������ջ�д�ŵ����ռ����ͷ��ռ��������������ö��
   ����*/
void Push(int i, int j)
{
	StackNode* p;
	p = (StackNode*)malloc(sizeof(StackNode));

	if (i == 1)
		(*p).var.Ntmlvar = (NontmlType)j;
	else if (i == 2)
		(*p).var.tmlvar = (TmlType)j;

	p->flag = i;
	p->underNode = StackTop;
	StackTop = p;
	STACKEMPTY = false;
}

void  Pop()
{
	StackNode* p;
	p = StackTop;
	StackTop = (*StackTop).underNode;
	free(p);
	if (StackTop == NULL)
		STACKEMPTY = true;
}

/*��ջ����־�������ռ������Ƿ��ռ���*/
int readStackflag()
{
	int j;
	j = StackTop->flag;
	return(j);
}

/*���ռ���ʱ*/
NontmlType readstackN()
{
	return ((*StackTop).var.Ntmlvar);
}

/*�ռ���*/
TmlType readstackT()
{
	return ((*StackTop).var.tmlvar);
}


/*ѹջ��ʵ�ֹ��̣�ѹ�����ָ�����ڵ��ָ��ĵ�ַ*/
void  PushPA(TreeNode** t)
{
	StackNodePA* p = NULL;
	p = (StackNodePA*)malloc(sizeof(StackNodePA));
	p->pointer = t;
	p->underNode = StackTopPA;
	StackTopPA = p;
	paSTACKEMPTY = false;
}
/*��ջ��ʵ�ֹ���*/
TreeNode** PopPA()
{
	StackNodePA* p = NULL;
	TreeNode** backpointer;
	p = StackTopPA;
	backpointer = p->pointer;
	StackTopPA = StackTopPA->underNode;
	free(p);
	if (StackTopPA == NULL)
		paSTACKEMPTY = true;

	return backpointer;
}


/*������ջ��ѹջʵ�ֹ��̣�ѹ�����ָ�����ڵ��ָ��*/
void  PushOp(TreeNode* t)
{
	StackNodeP* p = NULL;
	p = (StackNodeP*)malloc(sizeof(StackNodeP));
	p->pointer = t;
	p->underNode = OpStackTop;
	OpStackTop = p;
	OpSTACKEMPTY = FALSE;
}

/*������ջ�ĵ�ջʵ�ֹ���*/
TreeNode* PopOp()
{
	StackNodeP* p = NULL;
	TreeNode* backpointer;
	p = OpStackTop;
	backpointer = p->pointer;
	OpStackTop = OpStackTop->underNode;
	free(p);
	if (OpStackTop == NULL)
		OpSTACKEMPTY = TRUE;

	return backpointer;
}

LexType ReadOpStack()
{
	TreeNode* t1;
	t1 = OpStackTop->pointer;
	return (t1->attr.ExpAttr.op);
}
/*������ջ��ѹջʵ�ֹ��̣�ѹ�����ָ�����ڵ��ָ��*/
void  PushNum(TreeNode* t)
{
	StackNodeP* p = NULL;
	p = (StackNodeP*)malloc(sizeof(StackNodeP));
	p->pointer = t;
	p->underNode = NumStackTop;
	NumStackTop = p;
	NumSTACKEMPTY = false;
}
/*������ջ�ĵ�ջʵ�ֹ���*/
TreeNode* PopNum()
{
	StackNodeP* p = NULL;
	TreeNode* backpointer;
	p = NumStackTop;
	backpointer = p->pointer;
	NumStackTop = NumStackTop->underNode;
	free(p);
	if (NumStackTop == NULL)
		NumSTACKEMPTY = true;

	return backpointer;
}




/********************************************************
 *********�����Ǵ����﷨�����õĸ���ڵ������***********
 ********************************************************/

 /********************************************************/
 /* ������ newRootNode									*/
 /* ��  �� �����﷨�����ڵ㺯��			        		*/
 /* ˵  �� �ú���Ϊ�﷨������һ���µĸ����      		*/
 /*        �����﷨���ڵ��Ա��ʼ��						*/
 /********************************************************/
TreeNode* newRootNode(void)

{
	/* ���ڴ��ж�̬������䵥Ԫ������ָ��õ�Ԫ���﷨���������ָ��t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* �﷨���ڵ�ָ��tΪNULL,δ�ܳɹ������ڴ浥Ԫ *
	 * ��������Ϣ���к�linenoд���б��ļ�listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* �﷨���ڵ�ָ��t����NULL,�ɹ������ڴ浥Ԫ */
	else {

		/* ��ʼ�����﷨���ڵ�t���ӽڵ�child[i]ΪNULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* ��ʼ�����﷨���ڵ�t�ֵܽڵ�siblingΪNULL */
		t->sibling = NULL;

		/* ָ�����﷨���ڵ�t��Ա:�������nodekindΪ�������ProK */
		t->nodekind = ProK;

		/* ָ�����﷨���ڵ�t��Ա:Դ�����к�linenoΪȫ�ֱ���lineno */
		t->lineno = lineno;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}

	}
	/* ���������﷨�����ڵ�ָ��t */
	return t;
}


/********************************************************/
/* ������ newPheadNode									*/
/* ��  �� ��������ͷ�����﷨���ڵ㺯��					*/
/* ˵  �� �ú���Ϊ�﷨������һ���µĳ���ͷ���ͽ��		*/
/*        �����﷨���ڵ��Ա��ʼ��						*/
/********************************************************/
TreeNode* newPheadNode(void)

{
	/* ���ڴ��ж�̬������䵥Ԫ������ָ��õ�Ԫ���﷨���������ָ��t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* �﷨���ڵ�ָ��tΪNULL,δ�ܳɹ������ڴ浥Ԫ *
	 * ��������Ϣ���к�linenoд���б��ļ�listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* �﷨���ڵ�ָ��t����NULL,�ɹ������ڴ浥Ԫ */
	else {

		/* ��ʼ�����﷨���ڵ�t���ӽڵ�child[i]ΪNULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* ��ʼ�����﷨���ڵ�t�ֵܽڵ�siblingΪNULL */
		t->sibling = NULL;

		/* ָ�����﷨���ڵ�t��Ա:�������nodekindΪ�������PheadK */
		t->nodekind = PheadK;

		/* ָ�����﷨���ڵ�t��Ա:Դ�����к�linenoΪȫ�ֱ���lineno */
		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}


	}
	/* �������س���ͷ�����﷨���ڵ�ָ��t */
	return t;
}


/********************************************************/
/* ������ newDecANode									*/
/* ��  �� ���������﷨���ڵ㺯��,û��ָ������Ľڵ����� */
/*        ����,���﷨���ĵڶ���			                */
/* ˵  �� �ú���Ϊ�﷨������һ���µĽ��      	     	*/
/*        �����﷨���ڵ��Ա��ʼ��						*/
/********************************************************/
TreeNode* newDecANode(NodeKind kind)

{
	/* ���ڴ��ж�̬������䵥Ԫ������ָ��õ�Ԫ���﷨���������ָ��t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* �﷨���ڵ�ָ��tΪNULL,δ�ܳɹ������ڴ浥Ԫ *
	 * ��������Ϣ���к�linenoд���б��ļ�listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* �﷨���ڵ�ָ��t����NULL,�ɹ������ڴ浥Ԫ */
	else {

		/* ��ʼ�����﷨���ڵ�t���ӽڵ�child[i]ΪNULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* ��ʼ�����﷨���ڵ�t�ֵܽڵ�siblingΪNULL */
		t->sibling = NULL;

		/* ָ�����﷨���ڵ�t��Ա:�������nodekindΪ����kind */
		t->nodekind = kind;

		/* ָ�����﷨���ڵ�t��Ա:Դ�����к�linenoΪȫ�ֱ���lineno */
		t->lineno = lineno;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}
	}
	/* ���������﷨�����ڵ�ָ��t */
	return t;
}



//����


/********************************************************/
/* ������ newTypeNode									*/
/* ��  �� ���ͱ�־�﷨���ڵ㴴������					*/
/* ˵  �� �ú���Ϊ�﷨������һ���µ����ͱ�־��㣬      */
/*        ��ʾ����֮�µ�������Ϊ����������        		*/
/*        �����﷨���ڵ�ĳ�Ա��ʼ��					*/
/********************************************************/
TreeNode* newTypeNode()

{
	/* �ڴ��ж�̬������䵥Ԫ������ָ��õ�Ԫ���﷨���ڵ�����ָ��t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* �﷨���ڵ�ָ��tΪNULL,δ�ܳɹ������ڴ浥Ԫ		*
	 * ��������Ϣ���к�linenoд���б��ļ�listing		*/
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);

	/* �﷨���ڵ�ָ��t����NULL,�ڴ浥Ԫ�Ѿ��ɹ����� */
	else {

		/* ��ʼ�����﷨���ڵ�t���ӽڵ�child[i]ΪNULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* ��ʼ�����﷨���ڵ�t�ֵܽڵ�siblingΪNULL(��) */
		t->sibling = NULL;

		/* ָ�����﷨���ڵ�t��Ա: �������nodekindΪ���ʽ����ExpK */
		t->nodekind = TypeK;


		/* ָ�����﷨���ڵ�t��Ա: Դ�����к�linenoΪȫ�ֱ���lineno */
		t->lineno = lineno;

		t->idnum = 0;
		/*��ʼ�����ű��ַָ��*/
		for (int i = 0; i < 10; i++)
			t->table[i] = NULL;

	}

	/* �������ر��ʽ�����﷨�����ָ��t */
	return t;
}

/********************************************************/
/* ������ newVarNode									*/
/* ��  �� ����������־�﷨���ڵ㴴������				*/
/* ˵  �� �ú���Ϊ�﷨������һ���µı���������־��㣬
/*         ��ʾ����֮�µ�������Ϊ����������           	*/
/*        �����﷨���ڵ�ĳ�Ա��ʼ��					*/
/********************************************************/
TreeNode* newVarNode()

{
	/* �ڴ��ж�̬������䵥Ԫ������ָ��õ�Ԫ���﷨���ڵ�����ָ��t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* �﷨���ڵ�ָ��tΪNULL,δ�ܳɹ������ڴ浥Ԫ		*
	 * ��������Ϣ���к�linenoд���б��ļ�listing		*/
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);

	/* �﷨���ڵ�ָ��t����NULL,�ڴ浥Ԫ�Ѿ��ɹ����� */
	else {

		/* ��ʼ�����﷨���ڵ�t���ӽڵ�child[i]ΪNULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* ��ʼ�����﷨���ڵ�t�ֵܽڵ�siblingΪNULL(��) */
		t->sibling = NULL;

		/* ָ�����﷨���ڵ�t��Ա: �������nodekindΪ���ʽ����ExpK */
		t->nodekind = VarK;

		/* ָ�����﷨���ڵ�t��Ա: Դ�����к�linenoΪȫ�ֱ���lineno */
		t->lineno = lineno;

		t->idnum = 0;
		/*��ʼ�����ű��ַָ��*/
		for (int i = 0; i < 10; i++)
			t->table[i] = NULL;
	}

	/* �������ر��ʽ�����﷨�����ָ��t */
	return t;
}







/********************************************************/
/* ������ newDecNode									*/
/* ��  �� �������������﷨���ڵ㺯��					*/
/* ˵  �� �ú���Ϊ�﷨������һ���µ��������ͽ��		*/
/*        �����﷨���ڵ��Ա��ʼ��						*/
/********************************************************/
TreeNode* newDecNode(void)

{
	/* ���ڴ��ж�̬������䵥Ԫ������ָ��õ�Ԫ���﷨���������ָ��t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* �﷨���ڵ�ָ��tΪNULL,δ�ܳɹ������ڴ浥Ԫ *
	 * ��������Ϣ���к�linenoд���б��ļ�listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}

	/* �﷨���ڵ�ָ��t����NULL,�ɹ������ڴ浥Ԫ */
	else {

		/* ��ʼ�����﷨���ڵ�t���ӽڵ�child[i]ΪNULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* ��ʼ�����﷨���ڵ�t�ֵܽڵ�siblingΪNULL */
		t->sibling = NULL;

		/* ָ�����﷨���ڵ�t��Ա:�������nodekindΪ�������DecK*/
		t->nodekind = DecK;

		/* ָ�����﷨���ڵ�t��Ա:Դ�����к�linenoΪȫ�ֱ���lineno */
		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}

	}
	/* �����������������﷨���ڵ�ָ��t */
	return t;
}


/********************************************************/
/* ������ newProcNode									*/
/* ��  �� �������������﷨���ڵ㺯��					*/
/* ˵  �� �ú���Ϊ�﷨������һ���µĺ������ͽ��		*/
/*        �����﷨���ڵ��Ա��ʼ��						*/
/********************************************************/
TreeNode* newProcNode(void)

{
	/* ���ڴ��ж�̬������䵥Ԫ������ָ��õ�Ԫ���﷨���������ָ��t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* �﷨���ڵ�ָ��tΪNULL,δ�ܳɹ������ڴ浥Ԫ *
	 * ��������Ϣ���к�linenoд���б��ļ�listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* �﷨���ڵ�ָ��t����NULL,�ɹ������ڴ浥Ԫ */
	else {

		/* ��ʼ�����﷨���ڵ�t���ӽڵ�child[i]ΪNULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* ��ʼ�����﷨���ڵ�t�ֵܽڵ�siblingΪNULL */
		t->sibling = NULL;

		/* ָ�����﷨���ڵ�t��Ա:�������nodekindΪ�������ProcDecK */
		t->nodekind = ProcDecK;


		/* ָ�����﷨���ڵ�t��Ա:Դ�����к�linenoΪȫ�ֱ���lineno */
		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}

	}
	/* ����������������﷨���ڵ�ָ��t */
	return t;
}

/********************************************************/
/* ������ newStmlNode									*/
/* ��  �� ��������־�����﷨���ڵ㺯��				*/
/* ˵  �� �ú���Ϊ�﷨������һ���µ�����־���ͽ��	*/
/*        �����﷨���ڵ��Ա��ʼ��						*/
/********************************************************/
TreeNode* newStmlNode(void)

{
	/* ���ڴ��ж�̬������䵥Ԫ������ָ��õ�Ԫ���﷨���������ָ��t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* �﷨���ڵ�ָ��tΪNULL,δ�ܳɹ������ڴ浥Ԫ *
	 * ��������Ϣ���к�linenoд���б��ļ�listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* �﷨���ڵ�ָ��t����NULL,�ɹ������ڴ浥Ԫ */
	else {

		/* ��ʼ�����﷨���ڵ�t���ӽڵ�child[i]ΪNULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* ��ʼ�����﷨���ڵ�t�ֵܽڵ�siblingΪNULL */
		t->sibling = NULL;

		/* ָ�����﷨���ڵ�t��Ա:�������nodekindΪ�������StmLK */
		t->nodekind = StmLK;

		/* ָ�����﷨���ڵ�t��Ա:Դ�����к�linenoΪȫ�ֱ���lineno */
		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}

	}
	/*����������������﷨���ڵ�ָ��t*/
	return t;
}

/********************************************************/
/* ������ newStmtNode									*/
/* ��  �� ������������﷨���ڵ㺯��					*/
/* ˵  �� �ú���Ϊ�﷨������һ���µ�������ͽ��		*/
/*        �����﷨���ڵ��Ա��ʼ��						*/
/********************************************************/
TreeNode* newStmtNode(StmtKind kind)

{
	/* ���ڴ��ж�̬������䵥Ԫ������ָ��õ�Ԫ���﷨���������ָ��t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* �﷨���ڵ�ָ��tΪNULL,δ�ܳɹ������ڴ浥Ԫ *
	 * ��������Ϣ���к�linenoд���б��ļ�listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* �﷨���ڵ�ָ��t����NULL,�ɹ������ڴ浥Ԫ */
	else {

		/* ��ʼ�����﷨���ڵ�t���ӽڵ�child[i]ΪNULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* ��ʼ�����﷨���ڵ�t�ֵܽڵ�siblingΪNULL */
		t->sibling = NULL;

		/* ָ�����﷨���ڵ�t��Ա:�������nodekindΪ�������StmtK */
		t->nodekind = StmtK;

		/* ָ�����﷨���ڵ�t��Ա:�������kind.stmtΪ������������kind */
		t->kind.stmt = kind;

		/* ָ�����﷨���ڵ�t��Ա:Դ�����к�linenoΪȫ�ֱ���lineno */
		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}
	}
	/* ����������������﷨���ڵ�ָ��t */
	return t;
}


/********************************************************/
/* ������ newExpNode									*/
/* ��  �� ���ʽ�����﷨���ڵ㴴������					*/
/* ˵  �� �ú���Ϊ�﷨������һ���µı��ʽ���ͽ��		*/
/*        �����﷨���ڵ�ĳ�Ա��ʼ��					*/
/********************************************************/
TreeNode* newExpNode(ExpKind kind)

{
	/* �ڴ��ж�̬������䵥Ԫ������ָ��õ�Ԫ���﷨���ڵ�����ָ��t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* �﷨���ڵ�ָ��tΪNULL,δ�ܳɹ������ڴ浥Ԫ		*
	 * ��������Ϣ���к�linenoд���б��ļ�listing		*/
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* �﷨���ڵ�ָ��t����NULL,�ڴ浥Ԫ�Ѿ��ɹ����� */
	else {

		/* ��ʼ�����﷨���ڵ�t���ӽڵ�child[i]ΪNULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* ��ʼ�����﷨���ڵ�t�ֵܽڵ�siblingΪNULL(��) */
		t->sibling = NULL;

		/* ָ�����﷨���ڵ�t��Ա: �������nodekindΪ���ʽ����ExpK */
		t->nodekind = ExpK;

		/* ָ�����﷨���ڵ�t��Ա: ���ʽ����kind.expΪ������������kind */
		t->kind.exp = kind;

		/* ָ�����﷨���ڵ�t��Ա: Դ�����к�linenoΪȫ�ֱ���lineno */
		t->lineno = lineno;

		/* ָ�����﷨���ڵ�t��Ա: ���ʽΪ��������ʱ�ı�������varkind *
		/* ΪIdV.*/
		t->attr.ExpAttr.varkind = IdV;

		/* ָ�����﷨���ڵ�t��Ա: ���ͼ������typeΪVoid */
		t->attr.ExpAttr.type = Void;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}
	}

	/* �������ر��ʽ�����﷨�����ָ��t */
	return t;
}


/* ��̬����indentno�ں���printTree��	*
 * ���ڴ洢��ǰ������������,��ʼΪ0		*/
static int indentno = 0;

/** ����/���������궨�� **/
#define INDENT indentno+=4
#define UNINDENT indentno-=4


/********************************************************/
/* ������ printSpaces									*/
/* ��  �� �ո��ӡ����									*/
/* ˵  �� �ú�����ӡָ�������ո�,���ڱ�ʾ��������		*/
/********************************************************/
static void printSpaces(void)

{
	int i;

	/* ������������indentno��ӡ�ո��������	*
	 * ����������indentno���ܱ��ַǸ�			*/
	for (i = 0; i < indentno; i++)
		fprintf(listing, " ");

}

/******************************************************/
/* ������ printTree                                   */
/* ��  �� ���﷨���������ʾ��listing�ļ���           */
/* ˵  �� �ú��������˺���������������������          */
/******************************************************/
void  printTree(TreeNode* tree)
{
	int i;

	/* ����������,ÿ�ν����﷨���ڵ㶼������������ */
	INDENT;

	/* �������������﷨���ڵ�ָ��tree��NULL(��) */
	while (tree != NULL)
	{
		/*��ӡ�к�*/
		if (tree->lineno == 0)
			printTab(9);
		else
			switch ((int)(tree->lineno / 10))
			{
			case 0:
				fprintf(listing, "line:%d", tree->lineno);
				printTab(3);
				break;
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
				fprintf(listing, "line:%d", tree->lineno);
				printTab(2);
				break;
			default:
				fprintf(listing, "line:%d", tree->lineno);
				printTab(1);
			}

		/* ���ú���printSpaces,��ӡ��Ӧ�Ŀո�,�������� */
		printSpaces();

		switch (tree->nodekind)
		{
		case ProK:
			fprintf(listing, "%s  ", "ProK"); break;
		case PheadK:
		{fprintf(listing, "%s  ", "PheadK");
		fprintf(listing, "%s  ", tree->name[0]);
		}
		break;
		case DecK:
		{  fprintf(listing, "%s  ", "DecK");
		if (tree->attr.ProcAttr.paramt == varparamType)
			fprintf(listing, "%s  ", "var param:");
		if (tree->attr.ProcAttr.paramt == valparamType)
			fprintf(listing, "%s  ", "value param:");
		switch (tree->kind.dec)
		{
		case  ArrayK:
		{
			fprintf(listing, "%s  ", "ArrayK");
			fprintf(listing, "%d  ", tree->attr.ArrayAttr.up);
			fprintf(listing, "%d  ", tree->attr.ArrayAttr.low);
			if (tree->attr.ArrayAttr.childtype == CharK)
				fprintf(listing, "%s  ", "Chark");
			else if (tree->attr.ArrayAttr.childtype == IntegerK)
				fprintf(listing, "%s  ", "IntegerK");
		}; break;
		case  CharK:
			fprintf(listing, "%s  ", "CharK"); break;
		case  IntegerK:
			fprintf(listing, "%s  ", "IntegerK"); break;
		case  RecordK:
			fprintf(listing, "%s  ", "RecordK"); break;
		case  IdK:
			fprintf(listing, "%s  ", "IdK");
			fprintf(listing, "%s  ", tree->attr.type_name);
			break;
		default:
			fprintf(listing, "error1!");
			Error = TRUE;
		};
		if (tree->idnum != 0)
			for (int i = 0; i <= (tree->idnum); i++)
			{
				fprintf(listing, "%s  ", tree->name[i]);

			}
		else
		{
			fprintf(listing, "wrong!no var!\n");
			Error = TRUE;
		}
		} break;
		case TypeK:
			fprintf(listing, "%s  ", "TypeK"); break;

		case VarK:
			fprintf(listing, "%s  ", "VarK");
			if (tree->table[0] != NULL)
				fprintf(listing, "%d  %d  ", tree->table[0]->attrIR.More.VarAttr.off, tree->table[0]->attrIR.More.VarAttr.level);
			break;

		case ProcDecK:
			fprintf(listing, "%s  ", "ProcDecK");
			fprintf(listing, "%s  ", tree->name[0]);
			if (tree->table[0] != NULL)
				fprintf(listing, "%d %d %d  ", tree->table[0]->attrIR.More.ProcAttr.mOff, tree->table[0]->attrIR.More.ProcAttr.nOff, tree->table[0]->attrIR.More.ProcAttr.level);
			break;

		case StmLK:
			fprintf(listing, "%s  ", "StmLk"); break;

		case StmtK:
		{ fprintf(listing, "%s  ", "StmtK");
		switch (tree->kind.stmt)
		{
		case IfK:
			fprintf(listing, "%s  ", "If"); break;
		case WhileK:
			fprintf(listing, "%s  ", "While"); break;

		case AssignK:
			fprintf(listing, "%s  ", "Assign");
			break;

		case ReadK:
			fprintf(listing, "%s  ", "Read");
			fprintf(listing, "%s  ", tree->name[0]);
			if (tree->table[0] != NULL)
				fprintf(listing, "%d   %d  ", tree->table[0]->attrIR.More.VarAttr.off, tree->table[0]->attrIR.More.VarAttr.level);
			break;

		case WriteK:
			fprintf(listing, "%s  ", "Write"); break;

		case CallK:
			fprintf(listing, "%s  ", "Call");
			fprintf(listing, "%s  ", tree->name[0]);
			break;

		case ReturnK:
			fprintf(listing, "%s  ", "Return"); break;

		default:
			fprintf(listing, "error2!");
			Error = TRUE;
		}
		}; break;
		case ExpK:
		{ fprintf(listing, "%s  ", "ExpK");
		switch (tree->kind.exp)
		{
		case OpK:
		{ fprintf(listing, "%s  ", "Op");
		switch (tree->attr.ExpAttr.op)
		{
		case EQ:   fprintf(listing, "%s  ", "="); break;
		case LT:   fprintf(listing, "%s  ", "<"); break;
		case PLUS: fprintf(listing, "%s  ", "+"); break;
		case MINUS:fprintf(listing, "%s  ", "-"); break;
		case TIMES:fprintf(listing, "%s  ", "*"); break;
		case OVER: fprintf(listing, "%s  ", "/"); break;
		default:
			fprintf(listing, "error3!");
			Error = TRUE;
		}

		if (tree->attr.ExpAttr.varkind == ArrayMembV)
		{
			fprintf(listing, "ArrayMember  ");
			fprintf(listing, "%s  ", tree->name[0]);
		}
		}; break;
		case ConstK:
			fprintf(listing, "%s  ", "Const");
			switch (tree->attr.ExpAttr.varkind)
			{
			case IdV:
				fprintf(listing, "Id  ");
				fprintf(listing, "%s  ", tree->name[0]);
				break;
			case FieldMembV:
				fprintf(listing, "FieldMember  ");
				fprintf(listing, "%s  ", tree->name[0]);
				break;
			case ArrayMembV:
				fprintf(listing, "ArrayMember  ");
				fprintf(listing, "%s  ", tree->name[0]);
				break;
			default:
				fprintf(listing, "var type error!");
				Error = TRUE;
			}

			fprintf(listing, "%d  ", tree->attr.ExpAttr.val);
			break;
		case VariK:
			fprintf(listing, "%s  ", "Vari");
			switch (tree->attr.ExpAttr.varkind)
			{
			case IdV:
				fprintf(listing, "Id  ");
				fprintf(listing, "%s  ", tree->name[0]);
				break;
			case FieldMembV:
				fprintf(listing, "FieldMember  ");
				fprintf(listing, "%s  ", tree->name[0]);
				break;
			case ArrayMembV:
				fprintf(listing, "ArrayMember  ");
				fprintf(listing, "%s  ", tree->name[0]);
				break;
			default:
				fprintf(listing, "var type error!");
				Error = TRUE;
			}
			if (tree->table[0] != NULL)
				fprintf(listing, "%d   %d  ", tree->table[0]->attrIR.More.VarAttr.off, tree->table[0]->attrIR.More.VarAttr.level);

			break;
		default:
			fprintf(listing, "error4!");
			Error = TRUE;
		}
		}; break;
		default:
			fprintf(listing, "error5!");
			Error = TRUE;
		}

		fprintf(listing, "\n");

		/* ���﷨�����tree�ĸ��ӽ��ݹ����printTree���� *
		 * ����д���б��ļ�listing						   */
		for (i = 0; i < MAXCHILDREN; i++)
			printTree(tree->child[i]);

		/* ���﷨�����tree���ֵܽڵ�ݹ����printTree���� *
		 * ����д���б��ļ�listing						   */
		tree = tree->sibling;
	}

	/* ����������,ÿ���˳��﷨���ڵ�ʱ�������� */
	UNINDENT;
}



/**************************************************/
/****************�ͷ�ָ��ռ䲿��******************/
/**************************************************/

void freeDec(TreeNode* p);

void freeStm(TreeNode* p);

void freeExp(TreeNode* t);
/***********************************************************/
/* ������  freeTree                                        */
/* ��  ��  ͨ���ݹ�����ͷ�ָ��ռ�                        */
/* ˵  ��                    		    				   */
/***********************************************************/
void freeTree(TreeNode* t)
{
	TreeNode* p = NULL;

	if (t != NULL)
		free(t->child[0]);

	p = t->child[1];

	freeDec(p);

	p = t->child[2]->child[0];

	freeStm(p);

}
/***********************************************************/
/* ������  freeDec                                         */
/* ��  ��  ͨ���ݹ�����ͷ���������ָ��ռ�                */
/* ˵  ��                    		    				   */
/***********************************************************/
void freeDec(TreeNode* p)
{
	TreeNode* p1 = NULL;
	TreeNode* p2 = NULL;

	while (p != NULL)
	{
		switch (p->nodekind)
		{
		case TypeK:
		case VarK:
			/*p1ָ�����������ڵ������߱��������ڵ���*/
			p1 = p->child[0];
			while (p1 != NULL)
			{
				p2 = p1->sibling;
				free(p1);
				p1 = p2;
			}
			break;
		case ProcDecK:
			/*p1ָ���������ڵ�ĵ�һ�����ӽڵ㣭���βνڵ�*/
			p1 = p->child[0];
			while (p1 != NULL)
			{
				p2 = p1->sibling;
				free(p1);
				p1 = p2;
			}
			/*p1ָ���������ڵ�ĵڶ������ӽ�㣭�������ڵ�*/
			p1 = p->child[1];
			freeDec(p1);
			/*p1ָ���������ڵ���������ӽڵ㣭��������ڵ�*/
			p1 = p->child[2];
			freeStm(p1);
			break;
		}
		p1 = p->sibling;
		free(p);
		p = p1;
	}
}

/***********************************************************/
/* ������  freeStm                                         */
/* ��  ��  ͨ���ݹ�����ͷ��������ָ��ռ�                */
/* ˵  ��                    		    				   */
/***********************************************************/
void freeStm(TreeNode* p)
{
	TreeNode* t = p;
	TreeNode* p1 = NULL;
	TreeNode* p2 = NULL;
	while (t != NULL)
	{
		switch (t->kind.stmt)
		{
		case IfK:
			/*ɾ���������ʽ�ڵ�*/
			freeExp(t->child[0]);
			/*ɾ��then������в���*/
			freeStm(t->child[1]);
			/*ɾ��else������в���*/
			freeStm(t->child[2]);
			break;
		case WhileK:
			/*ɾ���������ʽ�ڵ�*/
			freeExp(t->child[0]);
			/*ɾ��while������в���*/
			freeStm(t->child[1]);
			break;
		case AssignK:
			/*ɾ����ֵ�����*/
			p1 = t->child[0];
			freeExp(p1);
			/*ɾ����ֵ���Ҳ�*/
			p1 = t->child[1];
			freeExp(p1);
			break;
		case ReadK:
			break;
		case WriteK:
			/*ɾ�����ӽڵ�*/
			freeExp(t->child[0]);
			break;
		case CallK:
			/*ɾ�������(���ú�����)*/
			freeExp(t->child[0]);
			/*ɾ���Ҷ��ӽڵ�(ʵ����)*/
			p1 = t->child[1];
			while (p1 != NULL)
			{
				p2 = p1->sibling;
				freeExp(p1);
				p1 = p2;
			}
			break;

		case ReturnK:
			break;
		}
		p1 = t->sibling;
		free(t);
		t = p1;
	}
}

/***********************************************************/
/* ������  freeExp                                         */
/* ��  ��  ͨ���ݹ�����ͷű��ʽ����ָ��ռ�              */
/* ˵  ��                    		    				   */
/***********************************************************/
void freeExp(TreeNode* t)
{
	TreeNode* p1 = NULL;
	TreeNode* p2 = NULL;

	switch (t->kind.exp)
	{
	case OpK:
		/*ɾ���������*/
		freeExp(t->child[0]);
		/*ɾ���Ҳ�����*/
		freeExp(t->child[1]);
		break;
	case ConstK:
		/*ֱ��ɾ���ýڵ�*/
		free(t);
		break;
	case VariK:
		p1 = t;
		while (p1 != NULL)
		{
			p2 = p1->child[0];
			free(p1);
			p1 = p2;
		}
		break;
	}
}

/***********************************************************/
/* ������  freeTable                                       */
/* ��  ��  ͨ���ݹ�����ͷŷ��ű�ռ�                      */
/* ˵  ��                    		    				   */
/***********************************************************/
void freeTable(void)
{
	SymbTable* p1 = NULL;
	SymbTable* p2 = NULL;
	int i = 0;

	while (scope[i] != NULL)
	{
		p1 = scope[i];
		while (p1 != NULL)
		{
			p2 = p1->next;
			free(p1);
			p1 = p2;
		}
		i++;
	}
}
