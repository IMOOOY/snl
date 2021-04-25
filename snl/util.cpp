/****************************************************/
/* 文件	util.cpp									*/
/* 说明 类PASCAL语言编译器功能函数的实现			*/
/* 主题 编译器结构:原理和实例						*/
/****************************************************/

/* 头文件globals.h定义了全局类型与变量 */
#include "globals.h"

#include "stdio.h"

#include "string.h"

#include "symbTable.h"

int  fp_num = 0;    /*输出时记录token个数的变量*/



/*****************************************************************/
/* 函数名 printTokenlist     					        		 */
/* 功  能 将文件tokenlist中的信息作为返回值                      */
/*        一般，listing指向标准输出。                            */
/* 说  明 用于显示词法分析结果                                   */
/*****************************************************************/
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

		/* 对函数参数Lex给定单词进行分类处理 */
		switch (token.Lex)
		{
			/* 单词token为保留字,将保留字词元以指定格式写入列表文件listing */
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

			/* 单词token为特殊符号：ASSIGN (赋值)，将":="写入文件listing */
		case ASSIGN: fprintf(listing, ":=\n"); break;

			/* 单词token为特殊符号：LT (小于)，将"<"写入文件listing */
		case LT: fprintf(listing, "<\n"); break;

			/* 单词token为特殊符号：EQ (等于)，将"="写入文件listing */
		case EQ: fprintf(listing, "=\n"); break;

			/* 单词token为特殊符号：LPAREN (左括号)，将"("写入文件listing */
		case LPAREN: fprintf(listing, "(\n"); break;

			/* 单词token为特殊符号：RPAREN (右括号)，将")"写入文件listing */
		case RPAREN: fprintf(listing, ")\n"); break;

			/* 单词token为特殊符号：SEMI (分号)，将";"写入文件listing */
		case SEMI: fprintf(listing, ";\n"); break;

			/* 单词token为特殊符号：PLUS (加号)，将"+"写入文件listing */
		case PLUS: fprintf(listing, "+\n"); break;

			/* 单词token为特殊符号；MINUS (减号)，将"-"写入文件listing */
		case MINUS: fprintf(listing, "-\n"); break;

			/* 单词token为特殊符号：TIMES (乘号)，将"*"写入文件listing */
		case TIMES: fprintf(listing, "*\n"); break;

			/* 单词token为特殊符号：OVER (除号)，将"/"写入文件listing */
		case OVER: fprintf(listing, "/\n");  break;

		case DOT:  fprintf(listing, ".\n"); break;

		case COMMA: fprintf(listing, ",\n"); break;

		case LMIDPAREN: fprintf(listing, "[\n"); break;

		case RMIDPAREN: fprintf(listing, "]\n"); break;

		case UNDERANGE: fprintf(listing, "..\n"); break;

			/* 单词token为簿记单词符号：ENDFILE (文件结尾)，将EOF写入文件listing */
		case ENDFILE: fprintf(listing, "EOF\n"); break;

			/* 单词token为多字符单词符号：NUM (数字)，将数值写入文件listing */
		case INTC:
			fprintf(listing,
				"NUM, val= %s\n", token.Sem);
			break;

		case CHARC:
			fprintf(listing,
				"INCHAR, char=%s\n", token.Sem);
			break;

			/* 单词token为多字符单词符号：ID (标识符)，将标识符名写入文件listing */
		case ID:
			fprintf(listing,
				"ID, name= %s\n", token.Sem);
			break;

			/* 单词token为簿记单词符号：ERROR (错误)，将错误信息写入文件listing */
		case ERROR:
			fprintf(listing,
				"ERROR: %s\n", token.Sem);
			break;

			/* 单词token为其他未知单词，未知信息写入文件listing,此种情况不应发生 */
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
/* 函数名 ReadNextToken							        		 */
/* 功  能 将文件tokenlist中的信息作为返回值                      */
/*        一般，listing指向标准输出。                            */
/* 说  明 返回值为TokenType类型，用于语法分析中                  */
/*****************************************************************/
void ReadNextToken(TokenType* p)
{
	FILE* fp2;
	/*按只读方式打开文件*/
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
/* 函数名 copyString									*/
/* 功  能 字符串复制函数								*/
/* 说  明 该函数为已存在的字串分配内存单元,并将其复制	*/
/********************************************************/
char* copyString(char* s)

{
	int n;
	char* t;

	/* 函数参数s所给字串为NULL(空), 函数返回NULL */
	if (s == NULL) return NULL;

	/* 函数参数s所给字串非空，计算字串s长度+1赋给临时变量n */
	n = strlen(s) + 1;

	/* 动态分配内存单元,指定单元长度为n,t为指向该单元的指针 */
	t = (char*)malloc(n);

	/* 单元指针t为NULL(空),未能成功分配			*
	 * 将出错信息及行号lineno写入列表文件listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* 调用库函数string.h,复制给定字串s到新字串单元t */
	else strcpy(t, s);

	/* 函数返回复制得到的新字串指针t */
	return t;
}


/*****************************************************************/
/* 函数名 ChainToFile   										 */
/* 功  能 将链表中的Token结点依次存入文件中                      */
/* 说  明  参数p是指针变量，指向Token链表的表头                  */
/*****************************************************************/
void ChainToFile(ChainNodeType* Chainhead)
{
	int num = 1;
	ChainNodeType* currentP = Chainhead;
	/*创建一个新的文件"Tokenlist",以存储Token序列*/
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
	/*从表头到表尾，依次将所有的Token写入文件*/
	do
	{
		fwrite(currentP, TOKENLEN, 1, fp);
		currentP = currentP->nextToken;
		num++;
	} while (currentP != NULL);

	fclose(fp);  /*关闭文件*/
}

/********************************************************
 *********以下是LL1语法分析方法所用到的栈操作************
 ********************************************************/
 /*用链表实现栈操作，栈中存放的是终极符和非终极符，上面的两个枚举
   类型*/
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

/*读栈顶标志，看是终极符还是非终极符*/
int readStackflag()
{
	int j;
	j = StackTop->flag;
	return(j);
}

/*非终极符时*/
NontmlType readstackN()
{
	return ((*StackTop).var.Ntmlvar);
}

/*终极符*/
TmlType readstackT()
{
	return ((*StackTop).var.tmlvar);
}


/*压栈的实现过程，压入的是指向树节点的指针的地址*/
void  PushPA(TreeNode** t)
{
	StackNodePA* p = NULL;
	p = (StackNodePA*)malloc(sizeof(StackNodePA));
	p->pointer = t;
	p->underNode = StackTopPA;
	StackTopPA = p;
	paSTACKEMPTY = false;
}
/*弹栈的实现过程*/
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


/*操作符栈的压栈实现过程，压入的是指向树节点的指针*/
void  PushOp(TreeNode* t)
{
	StackNodeP* p = NULL;
	p = (StackNodeP*)malloc(sizeof(StackNodeP));
	p->pointer = t;
	p->underNode = OpStackTop;
	OpStackTop = p;
	OpSTACKEMPTY = FALSE;
}

/*操作符栈的弹栈实现过程*/
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
/*操作数栈的压栈实现过程，压入的是指向树节点的指针*/
void  PushNum(TreeNode* t)
{
	StackNodeP* p = NULL;
	p = (StackNodeP*)malloc(sizeof(StackNodeP));
	p->pointer = t;
	p->underNode = NumStackTop;
	NumStackTop = p;
	NumSTACKEMPTY = false;
}
/*操作数栈的弹栈实现过程*/
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
 *********以下是创建语法树所用的各类节点的申请***********
 ********************************************************/

 /********************************************************/
 /* 函数名 newRootNode									*/
 /* 功  能 创建语法树根节点函数			        		*/
 /* 说  明 该函数为语法树创建一个新的根结点      		*/
 /*        并将语法树节点成员初始化						*/
 /********************************************************/
TreeNode* newRootNode(void)

{
	/* 在内存中动态申请分配单元，返回指向该单元的语法树结点类型指针t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* 语法树节点指针t为NULL,未能成功分配内存单元 *
	 * 将出错信息及行号lineno写入列表文件listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* 语法树节点指针t不是NULL,成功分配内存单元 */
	else {

		/* 初始化新语法树节点t各子节点child[i]为NULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* 初始化新语法树节点t兄弟节点sibling为NULL */
		t->sibling = NULL;

		/* 指定新语法树节点t成员:结点类型nodekind为语句类型ProK */
		t->nodekind = ProK;

		/* 指定新语法树节点t成员:源代码行号lineno为全局变量lineno */
		t->lineno = lineno;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}

	}
	/* 函数返回语法树根节点指针t */
	return t;
}


/********************************************************/
/* 函数名 newPheadNode									*/
/* 功  能 创建程序头类型语法树节点函数					*/
/* 说  明 该函数为语法树创建一个新的程序头类型结点		*/
/*        并将语法树节点成员初始化						*/
/********************************************************/
TreeNode* newPheadNode(void)

{
	/* 在内存中动态申请分配单元，返回指向该单元的语法树结点类型指针t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* 语法树节点指针t为NULL,未能成功分配内存单元 *
	 * 将出错信息及行号lineno写入列表文件listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* 语法树节点指针t不是NULL,成功分配内存单元 */
	else {

		/* 初始化新语法树节点t各子节点child[i]为NULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* 初始化新语法树节点t兄弟节点sibling为NULL */
		t->sibling = NULL;

		/* 指定新语法树节点t成员:结点类型nodekind为语句类型PheadK */
		t->nodekind = PheadK;

		/* 指定新语法树节点t成员:源代码行号lineno为全局变量lineno */
		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}


	}
	/* 函数返回程序头类型语法树节点指针t */
	return t;
}


/********************************************************/
/* 函数名 newDecANode									*/
/* 功  能 创建声明语法树节点函数,没有指明具体的节点声明 */
/*        类型,在语法树的第二层			                */
/* 说  明 该函数为语法树创建一个新的结点      	     	*/
/*        并将语法树节点成员初始化						*/
/********************************************************/
TreeNode* newDecANode(NodeKind kind)

{
	/* 在内存中动态申请分配单元，返回指向该单元的语法树结点类型指针t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* 语法树节点指针t为NULL,未能成功分配内存单元 *
	 * 将出错信息及行号lineno写入列表文件listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* 语法树节点指针t不是NULL,成功分配内存单元 */
	else {

		/* 初始化新语法树节点t各子节点child[i]为NULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* 初始化新语法树节点t兄弟节点sibling为NULL */
		t->sibling = NULL;

		/* 指定新语法树节点t成员:结点类型nodekind为参数kind */
		t->nodekind = kind;

		/* 指定新语法树节点t成员:源代码行号lineno为全局变量lineno */
		t->lineno = lineno;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}
	}
	/* 函数返回语法树根节点指针t */
	return t;
}



//尝试


/********************************************************/
/* 函数名 newTypeNode									*/
/* 功  能 类型标志语法树节点创建函数					*/
/* 说  明 该函数为语法树创建一个新的类型标志结点，      */
/*        表示在它之下的声明都为类型声明，        		*/
/*        并将语法树节点的成员初始化					*/
/********************************************************/
TreeNode* newTypeNode()

{
	/* 内存中动态申请分配单元，返回指向该单元的语法树节点类型指针t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* 语法树节点指针t为NULL,未能成功分配内存单元		*
	 * 将出错信息及行号lineno写入列表文件listing		*/
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);

	/* 语法树节点指针t不是NULL,内存单元已经成功分配 */
	else {

		/* 初始化新语法树节点t各子节点child[i]为NULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* 初始化新语法树节点t兄弟节点sibling为NULL(空) */
		t->sibling = NULL;

		/* 指定新语法树节点t成员: 结点类型nodekind为表达式类型ExpK */
		t->nodekind = TypeK;


		/* 指定新语法树节点t成员: 源代码行号lineno为全局变量lineno */
		t->lineno = lineno;

		t->idnum = 0;
		/*初始化符号表地址指针*/
		for (int i = 0; i < 10; i++)
			t->table[i] = NULL;

	}

	/* 函数返回表达式类型语法树结点指针t */
	return t;
}

/********************************************************/
/* 函数名 newVarNode									*/
/* 功  能 变量声明标志语法树节点创建函数				*/
/* 说  明 该函数为语法树创建一个新的变量声明标志结点，
/*         表示在它之下的声明都为变量声明，           	*/
/*        并将语法树节点的成员初始化					*/
/********************************************************/
TreeNode* newVarNode()

{
	/* 内存中动态申请分配单元，返回指向该单元的语法树节点类型指针t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* 语法树节点指针t为NULL,未能成功分配内存单元		*
	 * 将出错信息及行号lineno写入列表文件listing		*/
	if (t == NULL)
		fprintf(listing, "Out of memory error at line %d\n", lineno);

	/* 语法树节点指针t不是NULL,内存单元已经成功分配 */
	else {

		/* 初始化新语法树节点t各子节点child[i]为NULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* 初始化新语法树节点t兄弟节点sibling为NULL(空) */
		t->sibling = NULL;

		/* 指定新语法树节点t成员: 结点类型nodekind为表达式类型ExpK */
		t->nodekind = VarK;

		/* 指定新语法树节点t成员: 源代码行号lineno为全局变量lineno */
		t->lineno = lineno;

		t->idnum = 0;
		/*初始化符号表地址指针*/
		for (int i = 0; i < 10; i++)
			t->table[i] = NULL;
	}

	/* 函数返回表达式类型语法树结点指针t */
	return t;
}







/********************************************************/
/* 函数名 newDecNode									*/
/* 功  能 创建声明类型语法树节点函数					*/
/* 说  明 该函数为语法树创建一个新的声明类型结点		*/
/*        并将语法树节点成员初始化						*/
/********************************************************/
TreeNode* newDecNode(void)

{
	/* 在内存中动态申请分配单元，返回指向该单元的语法树结点类型指针t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* 语法树节点指针t为NULL,未能成功分配内存单元 *
	 * 将出错信息及行号lineno写入列表文件listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}

	/* 语法树节点指针t不是NULL,成功分配内存单元 */
	else {

		/* 初始化新语法树节点t各子节点child[i]为NULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* 初始化新语法树节点t兄弟节点sibling为NULL */
		t->sibling = NULL;

		/* 指定新语法树节点t成员:结点类型nodekind为语句类型DecK*/
		t->nodekind = DecK;

		/* 指定新语法树节点t成员:源代码行号lineno为全局变量lineno */
		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}

	}
	/* 函数返回声明类型语法树节点指针t */
	return t;
}


/********************************************************/
/* 函数名 newProcNode									*/
/* 功  能 创建函数类型语法树节点函数					*/
/* 说  明 该函数为语法树创建一个新的函数类型结点		*/
/*        并将语法树节点成员初始化						*/
/********************************************************/
TreeNode* newProcNode(void)

{
	/* 在内存中动态申请分配单元，返回指向该单元的语法树结点类型指针t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* 语法树节点指针t为NULL,未能成功分配内存单元 *
	 * 将出错信息及行号lineno写入列表文件listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* 语法树节点指针t不是NULL,成功分配内存单元 */
	else {

		/* 初始化新语法树节点t各子节点child[i]为NULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* 初始化新语法树节点t兄弟节点sibling为NULL */
		t->sibling = NULL;

		/* 指定新语法树节点t成员:结点类型nodekind为语句类型ProcDecK */
		t->nodekind = ProcDecK;


		/* 指定新语法树节点t成员:源代码行号lineno为全局变量lineno */
		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}

	}
	/* 函数返回语句类型语法树节点指针t */
	return t;
}

/********************************************************/
/* 函数名 newStmlNode									*/
/* 功  能 创建语句标志类型语法树节点函数				*/
/* 说  明 该函数为语法树创建一个新的语句标志类型结点	*/
/*        并将语法树节点成员初始化						*/
/********************************************************/
TreeNode* newStmlNode(void)

{
	/* 在内存中动态申请分配单元，返回指向该单元的语法树结点类型指针t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* 语法树节点指针t为NULL,未能成功分配内存单元 *
	 * 将出错信息及行号lineno写入列表文件listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* 语法树节点指针t不是NULL,成功分配内存单元 */
	else {

		/* 初始化新语法树节点t各子节点child[i]为NULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* 初始化新语法树节点t兄弟节点sibling为NULL */
		t->sibling = NULL;

		/* 指定新语法树节点t成员:结点类型nodekind为语句类型StmLK */
		t->nodekind = StmLK;

		/* 指定新语法树节点t成员:源代码行号lineno为全局变量lineno */
		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}

	}
	/*函数返回语句类型语法树节点指针t*/
	return t;
}

/********************************************************/
/* 函数名 newStmtNode									*/
/* 功  能 创建语句类型语法树节点函数					*/
/* 说  明 该函数为语法树创建一个新的语句类型结点		*/
/*        并将语法树节点成员初始化						*/
/********************************************************/
TreeNode* newStmtNode(StmtKind kind)

{
	/* 在内存中动态申请分配单元，返回指向该单元的语法树结点类型指针t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* 语法树节点指针t为NULL,未能成功分配内存单元 *
	 * 将出错信息及行号lineno写入列表文件listing  */
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* 语法树节点指针t不是NULL,成功分配内存单元 */
	else {

		/* 初始化新语法树节点t各子节点child[i]为NULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* 初始化新语法树节点t兄弟节点sibling为NULL */
		t->sibling = NULL;

		/* 指定新语法树节点t成员:结点类型nodekind为语句类型StmtK */
		t->nodekind = StmtK;

		/* 指定新语法树节点t成员:语句类型kind.stmt为函数给定参数kind */
		t->kind.stmt = kind;

		/* 指定新语法树节点t成员:源代码行号lineno为全局变量lineno */
		t->lineno = lineno;

		t->idnum = 0;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}
	}
	/* 函数返回语句类型语法树节点指针t */
	return t;
}


/********************************************************/
/* 函数名 newExpNode									*/
/* 功  能 表达式类型语法树节点创建函数					*/
/* 说  明 该函数为语法树创建一个新的表达式类型结点		*/
/*        并将语法树节点的成员初始化					*/
/********************************************************/
TreeNode* newExpNode(ExpKind kind)

{
	/* 内存中动态申请分配单元，返回指向该单元的语法树节点类型指针t */
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));

	int i;

	/* 语法树节点指针t为NULL,未能成功分配内存单元		*
	 * 将出错信息及行号lineno写入列表文件listing		*/
	if (t == NULL)
	{
		fprintf(listing, "Out of memory error at line %d\n", lineno);
		Error = TRUE;
	}
	/* 语法树节点指针t不是NULL,内存单元已经成功分配 */
	else {

		/* 初始化新语法树节点t各子节点child[i]为NULL */
		for (i = 0; i < MAXCHILDREN; i++) t->child[i] = NULL;

		/* 初始化新语法树节点t兄弟节点sibling为NULL(空) */
		t->sibling = NULL;

		/* 指定新语法树节点t成员: 结点类型nodekind为表达式类型ExpK */
		t->nodekind = ExpK;

		/* 指定新语法树节点t成员: 表达式类型kind.exp为函数给定参数kind */
		t->kind.exp = kind;

		/* 指定新语法树节点t成员: 源代码行号lineno为全局变量lineno */
		t->lineno = lineno;

		/* 指定新语法树节点t成员: 表达式为变量类型时的变量类型varkind *
		/* 为IdV.*/
		t->attr.ExpAttr.varkind = IdV;

		/* 指定新语法树节点t成员: 类型检查类型type为Void */
		t->attr.ExpAttr.type = Void;

		for (i = 0; i < 10; i++)
		{
			strcpy(t->name[i], "\0");
			t->table[i] = NULL;
		}
	}

	/* 函数返回表达式类型语法树结点指针t */
	return t;
}


/* 静态变量indentno在函数printTree中	*
 * 用于存储当前子树缩进格数,初始为0		*/
static int indentno = 0;

/** 增量/减量缩进宏定义 **/
#define INDENT indentno+=4
#define UNINDENT indentno-=4


/********************************************************/
/* 函数名 printSpaces									*/
/* 功  能 空格打印函数									*/
/* 说  明 该函数打印指定数量空格,用于表示子树缩进		*/
/********************************************************/
static void printSpaces(void)

{
	int i;

	/* 按给定缩进量indentno打印空格进行缩进	*
	 * 其中缩进量indentno总能保持非负			*/
	for (i = 0; i < indentno; i++)
		fprintf(listing, " ");

}

/******************************************************/
/* 函数名 printTree                                   */
/* 功  能 把语法树输出，显示在listing文件中           */
/* 说  明 该函数运用了宏来定义增量减量的缩进          */
/******************************************************/
void  printTree(TreeNode* tree)
{
	int i;

	/* 增量缩进宏,每次进入语法树节点都进行增量缩进 */
	INDENT;

	/* 函数参数给定语法树节点指针tree非NULL(空) */
	while (tree != NULL)
	{
		/*打印行号*/
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

		/* 调用函数printSpaces,打印相应的空格,进行缩进 */
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

		/* 对语法树结点tree的各子结点递归调用printTree过程 *
		 * 缩进写入列表文件listing						   */
		for (i = 0; i < MAXCHILDREN; i++)
			printTree(tree->child[i]);

		/* 对语法树结点tree的兄弟节点递归调用printTree过程 *
		 * 缩进写入列表文件listing						   */
		tree = tree->sibling;
	}

	/* 减量缩进宏,每次退出语法树节点时减量缩进 */
	UNINDENT;
}



/**************************************************/
/****************释放指针空间部分******************/
/**************************************************/

void freeDec(TreeNode* p);

void freeStm(TreeNode* p);

void freeExp(TreeNode* t);
/***********************************************************/
/* 函数名  freeTree                                        */
/* 功  能  通过递归调用释放指针空间                        */
/* 说  明                    		    				   */
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
/* 函数名  freeDec                                         */
/* 功  能  通过递归调用释放声明类型指针空间                */
/* 说  明                    		    				   */
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
			/*p1指向函数声明节点的第一个儿子节点－－形参节点*/
			p1 = p->child[0];
			while (p1 != NULL)
			{
				p2 = p1->sibling;
				free(p1);
				p1 = p2;
			}
			/*p1指向函数声明节点的第二个儿子结点－－声明节点*/
			p1 = p->child[1];
			freeDec(p1);
			/*p1指向函数声明节点的三个儿子节点－－函数体节点*/
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
/* 函数名  freeStm                                         */
/* 功  能  通过递归调用释放语句类型指针空间                */
/* 说  明                    		    				   */
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

/***********************************************************/
/* 函数名  freeExp                                         */
/* 功  能  通过递归调用释放表达式类型指针空间              */
/* 说  明                    		    				   */
/***********************************************************/
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

/***********************************************************/
/* 函数名  freeTable                                       */
/* 功  能  通过递归调用释放符号表空间                      */
/* 说  明                    		    				   */
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
