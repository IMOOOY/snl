
#include "globals.h"

#include "stdio.h"

#include "string.h"

#include "symbTable.h"

int  fp_num = 0;    /*输出时记录token个数的变量*/


void printTokenlist()
{
	TokenType  token;
	int m;

	fp = fopen("Tokenlist", "rb");  /*打开文件*/

	if (fp == NULL)
	{
		printf("can not open the file:Tokenlist!\n");
		Error = TRUE;
		//exit(0);
	}

	for (m = 1; m <= Tokennum; m++)
	{
		fread(&token, TOKENLEN, 1, fp);

		fprintf(listing, "\t%d: ", token.lineshow);/*打印行号*/

		switch (token.Lex)
		{
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

			fprintf(listing,     /*打印保留字*/
				"reserved word: %s\n", token.Sem);
			break;

		case ASSIGN: fprintf(listing, ":=\n"); break;

		case LT: fprintf(listing, "<\n"); break;

		case EQ: fprintf(listing, "=\n"); break;

		case LPAREN: fprintf(listing, "(\n"); break;

		case RPAREN: fprintf(listing, ")\n"); break;

		case SEMI: fprintf(listing, ";\n"); break;

		case PLUS: fprintf(listing, "+\n"); break;

		case MINUS: fprintf(listing, "-\n"); break;

		case TIMES: fprintf(listing, "*\n"); break;

		case OVER: fprintf(listing, "/\n");  break;

		case DOT:  fprintf(listing, ".\n"); break;

		case COMMA: fprintf(listing, ",\n"); break;

		case LMIDPAREN: fprintf(listing, "[\n"); break;

		case RMIDPAREN: fprintf(listing, "]\n"); break;

		case UNDERANGE: fprintf(listing, "..\n"); break;

		case ENDFILE: fprintf(listing, "EOF\n"); break;

		case INTC:
			fprintf(listing,
				"NUM, val= %s\n", token.Sem);
			break;

		case CHARC:
			fprintf(listing,
				"INCHAR, char=%s\n", token.Sem);
			break;

		case ID:
			fprintf(listing,
				"ID, name= %s\n", token.Sem);
			break;

		case ERROR:
			fprintf(listing,
				"ERROR: %s\n", token.Sem);
			break;

		default:
			fprintf(listing, "Unknown token: %d\n", token.Lex);
			break;

		}
		fseek(fp, m * TOKENLEN, 0);
	}
	fprintf(listing, "\n");
	fclose(fp);
}

void ReadNextToken(TokenType* p)
{
	FILE* fp2;
	fp2 = fopen("Tokenlist", "rb");
	if (fp == NULL)
	{
		printf("cannot create file Tokenlist!\n");
		Error = TRUE;
	}
	fseek(fp2, fp_num * sizeof(TokenType), 0);
	fread(p, sizeof(TokenType), 1, fp2);
	fp_num++;
	fclose(fp2);
}

char* copyString(char* s)

{
	int n;
	char* t;

	if (s == NULL) return NULL;

	n = strlen(s) + 1;

	t = (char*)malloc(n);

	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	else strcpy(t, s);

	return t;
}


void ChainToFile(ChainNodeType* Chainhead)
{
	int num = 1;
	ChainNodeType* currentP = Chainhead;
	fp = fopen("Tokenlist", "wb+");
	if (fp == NULL)
	{
		printf("cannot create file Tokenlist!\n");
		Error = TRUE;
		//exit(0);
	}

	fp = fopen("Tokenlist", "ab");   /*按追加方式打开文件*/
	if (fp == NULL)
	{
		printf("cannot open file Tokenlist!\n");
		Error = TRUE;
		//exit(0);
	}
	do
	{
		fwrite(currentP, TOKENLEN, 1, fp);
		currentP = currentP->nextToken;
		num++;
	} while (currentP != NULL);

	fclose(fp);  /*关闭文件*/
}

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

int readStackflag()
{
	int j;
	j = StackTop->flag;
	return(j);
}

NontmlType readstackN()
{
	return ((*StackTop).var.Ntmlvar);
}

TmlType readstackT()
{
	return ((*StackTop).var.tmlvar);
}


void  PushPA(TreeNode** t)
{
	StackNodePA* p = NULL;
	p = (StackNodePA*)malloc(sizeof(StackNodePA));
	p->pointer = t;
	p->underNode = StackTopPA;
	StackTopPA = p;
	paSTACKEMPTY = false;
}
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


void  PushOp(TreeNode* t)
{
	StackNodeP* p = NULL;
	p = (StackNodeP*)malloc(sizeof(StackNodeP));
	p->pointer = t;
	p->underNode = OpStackTop;
	OpStackTop = p;
	OpSTACKEMPTY = FALSE;
}

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
void  PushNum(TreeNode* t)
{
	StackNodeP* p = NULL;
	p = (StackNodeP*)malloc(sizeof(StackNodeP));
	p->pointer = t;
	p->underNode = NumStackTop;
	NumStackTop = p;
	NumSTACKEMPTY = false;
}
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

TreeNode* newRootNode(void)

{
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	else {

		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		t->sibling = NULL;

		t->nodekind = ProK;

		t->lineno = lineno;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}

	}
	return t;
}

TreeNode* newPheadNode(void)

{
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	else {

		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		t->sibling = NULL;

		t->nodekind = PheadK;

		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}


	}
	return t;
}

TreeNode* newDecANode(NodeKind kind)

{
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	else {

		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		t->sibling = NULL;

		t->nodekind = kind;

		t->lineno = lineno;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}
	}
	return t;
}

TreeNode* newTypeNode()

{
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);

	else {

		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		t->sibling = NULL;

		t->nodekind = TypeK;


		t->lineno = lineno;

		t->idnum = 0;
		for (int i = 0; i < 10; i++)
			t->table[i] = NULL;

	}

	return t;
}

TreeNode* newVarNode()

{
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);

	else {

		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		t->sibling = NULL;

		t->nodekind = VarK;

		t->lineno = lineno;

		t->idnum = 0;
		for (int i = 0; i < 10; i++)
			t->table[i] = NULL;
	}

	return t;
}


TreeNode* newDecNode(void)

{
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}

	else {

		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		t->sibling = NULL;

		t->nodekind = DecK;

		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}

	}
	return t;
}

TreeNode* newProcNode(void)

{
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;


	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	else {

		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		t->sibling = NULL;

		t->nodekind = ProcDecK;


		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}

	}
	return t;
}

TreeNode* newStmlNode(void)

{
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	else {

		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		t->sibling = NULL;

		t->nodekind = StmLK;

		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}

	}
	return t;
}


TreeNode* newStmtNode(StmtKind kind)

{
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	else {

		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		t->sibling = NULL;

		t->nodekind = StmtK;

		t->kind.stmt = kind;

		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}
	}
	return t;
}


TreeNode* newExpNode(ExpKind kind)

{
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	else {

		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		t->sibling = NULL;

		t->nodekind = ExpK;

		t->kind.exp = kind;

		t->lineno = lineno;

		t->attr.ExpAttr.varkind = IdV;

		t->attr.ExpAttr.type = Void;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}
	}

	return t;
}


static int indentno = 0;


#define INDENT indentno+=4
#define UNINDENT indentno-=4



static void printSpaces(void)

{
	int i;


	for (i = 0; i < indentno; i++)
		fprintf(listing, " ");

}


void  printTree(TreeNode* tree)
{
	int i;

	INDENT;

	while (tree != NULL)
	{

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

		for (i = 0; i < MAXCHILDREN; i++)
			printTree(tree->child[i]);

		tree = tree->sibling;
	}

	UNINDENT;
}




void freeDec(TreeNode* p);

void freeStm(TreeNode* p);

void freeExp(TreeNode* t);

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
			/*p1指向类型声明节点链或者变量声明节点链*/
			p1 = p->child[0];
			while (p1 != NULL)
			{
				p2 = p1->sibling;
				free(p1);
				p1 = p2;
			}
			break;
		case ProcDecK:
			p1 = p->child[0];
			while (p1 != NULL)
			{
				p2 = p1->sibling;
				free(p1);
				p1 = p2;
			}
			p1 = p->child[1];
			freeDec(p1);
			p1 = p->child[2];
			freeStm(p1);
			break;
		}
		p1 = p->sibling;
		free(p);
		p = p1;
	}
}

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
			/*删除条件表达式节点*/
			freeExp(t->child[0]);
			/*删除then语句序列部分*/
			freeStm(t->child[1]);
			/*删除else语句序列部分*/
			freeStm(t->child[2]);
			break;
		case WhileK:
			/*删除条件表达式节点*/
			freeExp(t->child[0]);
			/*删除while语句序列部分*/
			freeStm(t->child[1]);
			break;
		case AssignK:
			/*删除赋值号左侧*/
			p1 = t->child[0];
			freeExp(p1);
			/*删除赋值号右侧*/
			p1 = t->child[1];
			freeExp(p1);
			break;
		case ReadK:
			break;
		case WriteK:
			/*删除儿子节点*/
			freeExp(t->child[0]);
			break;
		case CallK:
			/*删除左儿子(调用函数名)*/
			freeExp(t->child[0]);
			/*删除右儿子节点(实参链)*/
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

void freeExp(TreeNode* t)
{
	TreeNode* p1 = NULL;
	TreeNode* p2 = NULL;

	switch (t->kind.exp)
	{
	case OpK:
		/*删除左操作数*/
		freeExp(t->child[0]);
		/*删除右操作数*/
		freeExp(t->child[1]);
		break;
	case ConstK:
		/*直接删除该节点*/
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
