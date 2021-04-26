

#include "globals.h"	

#include "util.h"		

#include "scanner.h"	

#include "parse.h"		

#include "string.h"

static TokenType token;

char* temp_name;

static int line0;



static TreeNode* program(void);

static TreeNode* programHead(void);

static TreeNode* declarePart(void);

static TreeNode* typeDec(void);

static TreeNode* typeDeclaration(void);

static TreeNode* typeDecList(void);

static TreeNode* typeDecMore(void);

static void  typeId(TreeNode* t);

static void  typeName(TreeNode* t);

static void  baseType(TreeNode* t);

static void  structureType(TreeNode* t);

static void  arrayType(TreeNode* t);

static void  recType(TreeNode* t);

static TreeNode* fieldDecList(void);

static TreeNode* fieldDecMore(void);

static void  idList(TreeNode* t);

static void  idMore(TreeNode* t);

static TreeNode* varDec(void);

static TreeNode* varDeclaration(void);

static TreeNode* varDecList(void);

static TreeNode* varDecMore(void);

static void  varIdList(TreeNode* t);

static void  varIdMore(TreeNode* t);

static TreeNode* procDec(void);

static TreeNode* procDeclaration(void);

static void  paramList(TreeNode* t);

static TreeNode* paramDecList(void);

static TreeNode* param(void);

static TreeNode* paramMore(void);

static void  formList(TreeNode* t);

static void  fidMore(TreeNode* t);

static TreeNode* procDecPart(void);

static TreeNode* procBody(void);

static TreeNode* programBody(void);

static TreeNode* stmList(void);

static TreeNode* stmMore(void);

static TreeNode* stm(void);

static TreeNode* assCall(void);

static TreeNode* assignmentRest(void);

static TreeNode* conditionalStm(void);

static TreeNode* loopStm(void);

static TreeNode* inputStm(void);

static TreeNode* outputStm(void);

static TreeNode* returnStm(void);

static TreeNode* callStmRest(void);

static TreeNode* actParamList(void);

static TreeNode* actParamMore(void);

static TreeNode* exp(void);			/* 处理表达式函数 */

static TreeNode* simple_exp(void);		/* 处理简单表达式函数 */

static TreeNode* term(void);			/* 处理项函数 */

static TreeNode* factor(void);			/* 处理因子函数 */

static TreeNode* variable(void);

static void variMore(TreeNode* t);

static TreeNode* fieldvar(void);

static void fieldvarMore(TreeNode* t);



//MARK: 语法分析功能函数
static void syntaxError(char* message)

{
	fprintf(listing, "\n>>> error :   ");
	fprintf(listing, "Syntax error at line %d: %s\n", token.lineshow, message);
	Error = TRUE;
}

static void match(LexType expected)

{
	if (token.Lex == expected)
	{
		ReadNextToken(&token);
		line0 = token.lineshow;
	}
	else
	{
		syntaxError("not match error ");
		fprintf(listing, "'%s'\n", token.Sem);
		ReadNextToken(&token);
		exit(0);
	}

}

TreeNode* program(void)
{
	TreeNode* t = programHead();
	TreeNode* q = declarePart();
	TreeNode* s = programBody();

	TreeNode* root = newRootNode();
	if (root != NULL)
	{
		root->lineno = 0;
		if (t != NULL) root->child[0] = t;
		else syntaxError("a program head is expected!");
		if (q != NULL) root->child[1] = q;
		if (s != NULL) root->child[2] = s;
		else syntaxError("a program body is expected!");
	}
	match(DOT);

	return root;
}

TreeNode* programHead(void)
{
	TreeNode* t = newPheadNode();
	match(PROGRAM);
	if ((t != NULL) && (token.Lex == ID))
	{
		t->lineno = 0;
		strcpy(t->name[0], token.Sem);
	}
	match(ID);
	return t;
}

//MARK: 声明部分

TreeNode* declarePart(void)
{
	/*类型*/
	TreeNode* typeP = newDecANode(TypeK);
	TreeNode* pp = typeP;

	if (typeP != NULL)
	{
		typeP->lineno = 0;
		TreeNode* tp1 = typeDec();
		if (tp1 != NULL)
			typeP->child[0] = tp1;
		else
		{
			free(typeP);
			typeP = NULL;
		}
	}
	/*变量*/
	TreeNode* varP = newDecANode(VarK);

	if (varP != NULL)
	{
		varP->lineno = 0;
		TreeNode* tp2 = varDec();
		if (tp2 != NULL)
			varP->child[0] = tp2;
		else
		{
			free(varP);
			varP = NULL;
		}
	}
	/*函数*/
	TreeNode* s = procDec();

	if (s == NULL) {}

	if (varP == NULL) { varP = s; }

	if (typeP == NULL) { pp = typeP = varP; }

	if (typeP != varP)
	{
		typeP->sibling = varP;
		typeP = varP;
	}
	if (varP != s)
	{
		varP->sibling = s;
		varP = s;
	}
	return pp;
}



//MARK: 类型声明部分

TreeNode* typeDec(void)
{
	TreeNode* t = NULL;
	switch (token.Lex)
	{
	case TYPE: t = typeDeclaration(); break;
	case VAR:
	case PROCEDURE:
	case BEGIN: break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	return t;
}


TreeNode* typeDeclaration(void)
{
	match(TYPE);
	TreeNode* t = typeDecList();
	if (t == NULL)
	{
		syntaxError("a type declaration is expected!");
	}
	return t;
}

TreeNode* typeDecList(void)
{
	TreeNode* t = newDecNode();
	if (t != NULL)
	{
		t->lineno = line0;
		typeId(t);
		match(EQ);
		typeName(t);
		match(SEMI);
		TreeNode* p = typeDecMore();
		if (p != NULL)
			t->sibling = p;
	}
	return t;
}

TreeNode* typeDecMore(void)
{
	TreeNode* t = NULL;
	switch (token.Lex)
	{
	case VAR:
	case PROCEDURE:
	case BEGIN:
		break;
	case ID:
		t = typeDecList();
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	return t;
}

void typeId(TreeNode* t)
{
	int tnum = (t->idnum);
	if ((token.Lex == ID) && (t != NULL))
	{
		strcpy(t->name[tnum], token.Sem);
		tnum = tnum + 1;
	}
	t->idnum = tnum;
	match(ID);
}


void typeName(TreeNode* t)
{
	if (t != NULL)
		switch (token.Lex)
		{
		case INTEGER:
		case CHAR:    baseType(t); break;
		case ARRAY:
		case RECORD:  structureType(t); break;
		case ID:
			t->kind.dec = IdK;
			strcpy(t->attr.type_name, token.Sem);
			match(ID);
			break;
		default:
			ReadNextToken(&token);
			syntaxError("unexpected token is here!");
			break;
		}
}


void baseType(TreeNode* t)
{
	switch (token.Lex)
	{
	case INTEGER:  match(INTEGER);
		t->kind.dec = IntegerK;
		break;

	case CHAR:     match(CHAR);
		t->kind.dec = CharK;
		break;

	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
}

void structureType(TreeNode* t)
{
	switch (token.Lex)
	{
	case ARRAY:
		arrayType(t);
		break;
	case RECORD:
		t->kind.dec = RecordK;
		recType(t);
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
}


void arrayType(TreeNode* t)
{
	match(ARRAY);
	match(LMIDPAREN);
	if (token.Lex == INTC)
	{
		t->attr.ArrayAttr.low = atoi(token.Sem);
	}
	match(INTC);
	match(UNDERANGE);
	if (token.Lex == INTC)
	{
		t->attr.ArrayAttr.up = atoi(token.Sem);
	}
	match(INTC);
	match(RMIDPAREN);
	match(OF);
	baseType(t);
	t->attr.ArrayAttr.childtype = t->kind.dec;
	t->kind.dec = ArrayK;
}


void recType(TreeNode* t)
{
	TreeNode* p = NULL;
	match(RECORD);
	p = fieldDecList();
	if (p != NULL)
		t->child[0] = p;
	else
		syntaxError("a record body is requested!");
	match(END);
}

TreeNode* fieldDecList(void)
{

	TreeNode* t = newDecNode();
	TreeNode* p = NULL;
	if (t != NULL)
	{
		t->lineno = line0;
		switch (token.Lex)
		{
		case INTEGER:
		case CHAR:
			baseType(t);
			idList(t);
			match(SEMI);
			p = fieldDecMore();
			break;
		case ARRAY:
			arrayType(t);
			idList(t);
			match(SEMI);
			p = fieldDecMore();
			break;
		default:
			ReadNextToken(&token);
			syntaxError("unexpected token is here!");
			break;
		}
		t->sibling = p;
	}
	return t;
}



TreeNode* fieldDecMore(void)
{
	TreeNode* t = NULL;
	switch (token.Lex)
	{
	case END: break;
	case INTEGER:
	case CHAR:
	case ARRAY:
		t = fieldDecList();
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	return t;
}


void idList(TreeNode* t)
{
	if (token.Lex == ID)
	{
		strcpy(t->name[(t->idnum)], token.Sem);
		match(ID);
		t->idnum = (t->idnum) + 1;
	}
	idMore(t);
}

void idMore(TreeNode* t)
{
	switch (token.Lex)
	{
	case SEMI:  break;
	case COMMA:
		match(COMMA);
		idList(t);
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
}


//MARK: 变量声明部分

TreeNode* varDec(void)
{
	TreeNode* t = NULL;
	switch (token.Lex)
	{
	case PROCEDURE:
	case BEGIN:     break;
	case VAR:
		t = varDeclaration();
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	return t;
}

TreeNode* varDeclaration(void)
{
	match(VAR);
	TreeNode* t = varDecList();
	if (t == NULL)
		syntaxError("a var declaration is expected!");
	return t;
}


TreeNode* varDecList(void)
{
	TreeNode* t = newDecNode();
	TreeNode* p = NULL;

	if (t != NULL)
	{
		t->lineno = line0;
		typeName(t);
		varIdList(t);
		match(SEMI);
		p = varDecMore();
		t->sibling = p;
	}
	return t;
}


TreeNode* varDecMore(void)
{
	TreeNode* t = NULL;

	switch (token.Lex)
	{
	case PROCEDURE:
	case BEGIN:
		break;
	case INTEGER:
	case CHAR:
	case ARRAY:
	case RECORD:
	case ID:
		t = varDecList();
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	return t;
}

void varIdList(TreeNode* t)
{
	if (token.Lex == ID)
	{
		strcpy(t->name[(t->idnum)], token.Sem);
		match(ID);
		t->idnum = (t->idnum) + 1;
	}
	else
	{
		syntaxError("a varid is expected here!");
		ReadNextToken(&token);
	}
	varIdMore(t);
}

void varIdMore(TreeNode* t)
{
	switch (token.Lex)
	{
	case SEMI:  break;
	case COMMA:
		match(COMMA);
		varIdList(t);
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
}

//MARK: 过程声明部分

TreeNode* procDec(void)
{
	TreeNode* t = NULL;
	switch (token.Lex)
	{
	case BEGIN:   break;
	case PROCEDURE:
		t = procDeclaration();
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	return t;
}

TreeNode* procDeclaration(void)
{
	TreeNode* t = newProcNode();
	match(PROCEDURE);
	if (t != NULL)
	{
		t->lineno = line0;
		if (token.Lex == ID)
		{
			strcpy(t->name[0], token.Sem);
			(t->idnum)++;
			match(ID);
		}
		match(LPAREN);
		paramList(t);
		match(RPAREN);
		match(SEMI);
		t->child[1] = procDecPart();
		t->child[2] = procBody();
		t->sibling = procDec();
	}
	return t;
}

void paramList(TreeNode* t)
{
	TreeNode* p = NULL;

	switch (token.Lex)
	{
	case RPAREN:  break;
	case INTEGER:
	case CHAR:
	case ARRAY:
	case RECORD:
	case ID:
	case VAR:
		p = paramDecList();
		t->child[0] = p;
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
}

TreeNode* paramDecList(void)
{
	TreeNode* t = param();
	TreeNode* p = paramMore();
	if (p != NULL)
	{
		t->sibling = p;
	}
	return t;
}


TreeNode* paramMore(void)
{
	TreeNode* t = NULL;
	switch (token.Lex)
	{
	case RPAREN:   break;
	case SEMI:
		match(SEMI);
		t = paramDecList();
		if (t == NULL)
			syntaxError("a param declaration is request!");
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	return t;
}

TreeNode* param(void)
{
	TreeNode* t = newDecNode();
	if (t != NULL)
	{
		t->lineno = line0;
		switch (token.Lex)
		{
		case INTEGER:
		case CHAR:
		case ARRAY:
		case RECORD:
		case ID:
			t->attr.ProcAttr.paramt = valparamType;
			typeName(t);
			formList(t);
			break;
		case VAR:
			match(VAR);
			t->attr.ProcAttr.paramt = varparamType;
			typeName(t);
			formList(t);
			break;
		default:
			ReadNextToken(&token);
			syntaxError("unexpected token is here!");
			break;
		}
	}
	return t;
}

void formList(TreeNode* t)
{
	if (token.Lex == ID)
	{
		strcpy(t->name[(t->idnum)], token.Sem);
		t->idnum = (t->idnum) + 1;
		match(ID);
	}
	fidMore(t);
}


void fidMore(TreeNode* t)
{
	switch (token.Lex)
	{
	case SEMI:
	case RPAREN:   break;
	case COMMA:
		match(COMMA);
		formList(t);
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
}


TreeNode* procDecPart(void)
{
	TreeNode* t = declarePart();
	return t;
}


TreeNode* procBody(void)
{
	TreeNode* t = programBody();
	if (t == NULL)
		syntaxError("a program body is requested!");
	return t;
}

TreeNode* programBody(void)
{
	TreeNode* t = newStmlNode();
	match(BEGIN);
	if (t != NULL)
	{
		t->lineno = 0;
		t->child[0] = stmList();
	}
	match(END);
	return t;
}

TreeNode* stmList(void)
{
	TreeNode* t = stm();
	TreeNode* p = stmMore();
	if (t != NULL)
		if (p != NULL)
			t->sibling = p;
	return t;
}
TreeNode* stmMore(void)
{
	TreeNode* t = NULL;
	switch (token.Lex)
	{
	case ELSE:
	case FI:
	case END:
	case ENDWH:	break;
	case SEMI:
		match(SEMI);
		t = stmList();
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	return t;
}

TreeNode* stm(void)
{

	TreeNode* t = NULL;
	switch (token.Lex)
	{
	case IF:
		t = conditionalStm();
		break;
	case WHILE:
		t = loopStm();
		break;
	case READ:
		t = inputStm();
		break;
	case WRITE:
		t = outputStm();
		break;
	case RETURN:
		t = returnStm();
		break;
	case ID:
		temp_name = copyString(token.Sem);
		match(ID);
		t = assCall();
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	return t;
}


TreeNode* assCall(void)
{
	TreeNode* t = NULL;
	switch (token.Lex)
	{
	case ASSIGN:
	case LMIDPAREN:
	case DOT:
		t = assignmentRest();
		break;
	case LPAREN:
		t = callStmRest();
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	return t;
}



TreeNode* assignmentRest(void)
{
	TreeNode* t = newStmtNode(AssignK);

	/* 赋值语句节点的第一个儿子节点记录赋值语句的左侧变量名，*
	/* 第二个儿子结点记录赋值语句的右侧表达式*/
	if (t != NULL)
	{
		t->lineno = line0;

		/*处理第一个儿子结点，为变量表达式类型节点*/
		TreeNode* child1 = newExpNode(VariK);
		if (child1 != NULL)
		{
			child1->lineno = line0;
			strcpy(child1->name[0], temp_name);
			(child1->idnum)++;
			variMore(child1);
			t->child[0] = child1;
		}

		/*赋值号匹配*/
		match(ASSIGN);

		/*处理第二个儿子节点*/
		t->child[1] = exp();

	}
	return t;
}



TreeNode* conditionalStm(void)
{
	TreeNode* t = newStmtNode(IfK);
	match(IF);
	if (t != NULL)
	{
		t->lineno = line0;
		t->child[0] = exp();
	}
	match(THEN);
	if (t != NULL)  t->child[1] = stmList();
	if (token.Lex == ELSE)
	{
		match(ELSE);
		if (t != NULL)
			t->child[2] = stmList();
	}
	match(FI);
	return t;
}


TreeNode* loopStm(void)
{
	TreeNode* t = newStmtNode(WhileK);
	match(WHILE);
	if (t != NULL)
	{
		t->lineno = line0;
		t->child[0] = exp();
		match(DO);
		t->child[1] = stmList();
		match(ENDWH);
	}
	return t;
}


TreeNode* inputStm(void)
{
	TreeNode* t = newStmtNode(ReadK);
	match(READ);
	match(LPAREN);
	if ((t != NULL) && (token.Lex == ID))
	{
		t->lineno = line0;
		strcpy(t->name[0], token.Sem);
		(t->idnum)++;
	}
	match(ID);
	match(RPAREN);
	return t;
}


TreeNode* outputStm(void)
{
	TreeNode* t = newStmtNode(WriteK);
	match(WRITE);
	match(LPAREN);
	if (t != NULL)
	{
		t->lineno = line0;
		t->child[0] = exp();
	}
	match(RPAREN);
	return t;
}


TreeNode* returnStm(void)
{
	TreeNode* t = newStmtNode(ReturnK);
	match(RETURN);
	if (t != NULL)
		t->lineno = line0;
	return t;
}


TreeNode* callStmRest(void)
{
	TreeNode* t = newStmtNode(CallK);
	match(LPAREN);
	/*函数调用时，其子节点指向实参*/
	if (t != NULL)
	{
		t->lineno = line0;

		/*函数名的结点也用表达式类型结点*/
		TreeNode* child0 = newExpNode(VariK);
		if (child0 != NULL)
		{
			child0->lineno = line0;
			strcpy(child0->name[0], temp_name);
			(child0->idnum)++;
			t->child[0] = child0;
		}
		t->child[1] = actParamList();
	}
	match(RPAREN);
	return t;
}

TreeNode* actParamList(void)
{
	TreeNode* t = NULL;

	switch (token.Lex)
	{
	case RPAREN:        break;
	case ID:
	case INTC:
		t = exp();
		if (t != NULL)
			t->sibling = actParamMore();
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	return t;
}

TreeNode* actParamMore(void)
{
	TreeNode* t = NULL;
	switch (token.Lex)
	{
	case RPAREN:   break;
	case COMMA:
		match(COMMA);
		t = actParamList();
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	return t;
}




TreeNode* exp(void)

{
	/* 调用简单表达式处理函数simple_exp(),返回语法树节点指针给t */
	TreeNode* t = simple_exp();

	/* 当前单词token为逻辑运算单词LT或者EQ */
	if ((token.Lex == LT) || (token.Lex == EQ))
	{
		/* 创建新的OpK类型语法树节点，新语法树节点指针赋给p */
		TreeNode* p = newExpNode(OpK);

		/* 新语法树节点p创建成功,初始化p第一个子节点成员child[0]
		 * 并将当前单词token(为EQ或者LT)赋给语法树节点p的运算符成员attr.op*/
		if (p != NULL)
		{
			p->lineno = line0;
			p->child[0] = t;
			p->attr.ExpAttr.op = token.Lex;

			/* 将新的表达式类型语法树节点p作为函数返回值t */
			t = p;
		}

		/* 当前单词token与指定逻辑运算符单词(为EQ或者LT)匹配 */
		match(token.Lex);

		/* 语法树节点t非空,调用简单表达式处理函数simple_exp()	*
		 * 函数返回语法树节点指针给t的第二子节点成员child[1]	*/
		if (t != NULL)
			t->child[1] = simple_exp();
	}

	/* 函数返回表达式类型语法树节点t */
	return t;
}


TreeNode* simple_exp(void)

{
	/* 调用元处理函数term(),函数返回语法树节点指针给t */
	TreeNode* t = term();

	/* 当前单词token为加法运算符单词PLUS或MINUS */
	while ((token.Lex == PLUS) || (token.Lex == MINUS))

	{
		/* 创建新OpK表达式类型语法树节点，新语法树节点指针赋给p */
		TreeNode* p = newExpNode(OpK);

		/* 语法树节点p创建成功,初始化p第一子节点成员child[0]	*
		 * 返回语法树节点指针给p的运算符成员attr.op				*/
		if (p != NULL)
		{
			p->lineno = line0;
			p->child[0] = t;
			p->attr.ExpAttr.op = token.Lex;

			/* 将函数返回值t赋成语法树节点p */
			t = p;

			/* 当前单词token与指定加法运算单词(为PLUS或MINUS)匹配 */
			match(token.Lex);

			/* 调用元处理函数term(),函数返回语法树节点指针给t的第二子节点成员child[1] */
			t->child[1] = term();
		}
	}
	/* 函数返回表达式类型语法树节点t */
	return t;
}



TreeNode* term(void)

{
	/* 调用因子处理函数factor(),函数返回语法树节点指针给t */
	TreeNode* t = factor();

	/* 当前单词token为乘法运算符单词TIMES或OVER */
	while ((token.Lex == TIMES) || (token.Lex == OVER))

	{
		/* 创建新的OpK表达式类型语法树节点,新节点指针赋给p */
		treeNode* p = newExpNode(OpK);

		/* 新语法树节点p创建成功,初始化第一个子节点成员child[0]为t	*
		 * 将当前单词token赋值给语法树节点p的运算符成员attr.op		*/
		if (p != NULL)
		{
			p->lineno = line0;
			p->child[0] = t;
			p->attr.ExpAttr.op = token.Lex;
			t = p;
		}

		/* 当前单词token与指定乘法运算符单词(为TIMES或OVER)匹配 */
		match(token.Lex);

		/* 调用因子处理函数factor(),函数返回语法树节点指针赋给p第二个子节点成员child[1] */
		p->child[1] = factor();

	}
	/* 函数返回表达式类型语法树节点t */
	return t;
}


TreeNode* factor(void)

{
	TreeNode* t = NULL;

	switch (token.Lex)
	{
	case INTC:

		t = newExpNode(ConstK);

		if ((t != NULL) && (token.Lex == INTC))
		{
			t->lineno = line0;
			t->attr.ExpAttr.val = atoi(token.Sem);
		}

		match(INTC);
		break;

	case ID:

		t = variable();
		break;

	case LPAREN:

		/* 当前单词token与左括号单词LPAREN匹配 */
		match(LPAREN);

		/* 调用表达式处理函数exp(),函数返回语法树节点指针给t */
		t = exp();

		/* 当前单词token与右括号单词RPAREN匹配 */
		match(RPAREN);

		break;

		/* 当前单词token为其它单词 */
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	/* 函数返回表达式类型语法树节点t */
	return t;
}


TreeNode* variable(void)
{
	TreeNode* t = newExpNode(VariK);

	if ((t != NULL) && (token.Lex == ID))
	{
		t->lineno = line0;
		strcpy(t->name[0], token.Sem);
		(t->idnum)++;
	}

	match(ID);
	variMore(t);
	return t;
}


void variMore(TreeNode* t)
{
	switch (token.Lex)
	{
	case ASSIGN:
	case TIMES:
	case EQ:
	case LT:
	case PLUS:
	case MINUS:
	case OVER:
	case RPAREN:
	case RMIDPAREN:
	case SEMI:
	case COMMA:
	case THEN:
	case ELSE:
	case FI:
	case DO:
	case ENDWH:
	case END:
		break;
	case LMIDPAREN:
		match(LMIDPAREN);

		/*用来以后求出其表达式的值，送入用于数组下标计算*/
		t->child[0] = exp();

		t->attr.ExpAttr.varkind = ArrayMembV;

		/*此表达式为数组成员变量类型*/
		t->child[0]->attr.ExpAttr.varkind = IdV;
		match(RMIDPAREN);
		break;
	case DOT:
		match(DOT);
		/*第一个儿子指向域成员变量结点*/
		t->child[0] = fieldvar();

		t->attr.ExpAttr.varkind = FieldMembV;

		t->child[0]->attr.ExpAttr.varkind = IdV;
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
}

TreeNode* fieldvar(void)
{
	/*注意，可否将此处的IdEK改为一个新的标识，用来记录记录类型的域*/
	TreeNode* t = newExpNode(VariK);

	if ((t != NULL) && (token.Lex == ID))
	{
		t->lineno = line0;
		strcpy(t->name[0], token.Sem);
		(t->idnum)++;
	}
	match(ID);

	fieldvarMore(t);

	return t;
}


void fieldvarMore(TreeNode* t)
{
	switch (token.Lex)
	{
	case ASSIGN:
	case TIMES:
	case EQ:
	case LT:
	case PLUS:
	case MINUS:
	case OVER:
	case RPAREN:
	case SEMI:
	case COMMA:
	case THEN:
	case ELSE:
	case FI:
	case DO:
	case ENDWH:
	case END:
		break;
	case LMIDPAREN:
		match(LMIDPAREN);

		/*用来以后求出其表达式的值，送入用于数组下标计算*/
		t->child[0] = exp();
		t->child[0]->attr.ExpAttr.varkind = ArrayMembV;
		match(RMIDPAREN);
		break;
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
}



TreeNode* parse(void)

{
	TreeNode* t = NULL;

	/* 从文件Tokenlist中取得第一个单词,将词法信息送给token */

	ReadNextToken(&token);

	/* 开始调用基本语法分析处理函数,递归下降处理 */
	t = program();

	/* 当前单词token不是ENDFILE,报代码在文件结束前提前结束错误 */
	if (token.Lex != ENDFILE)
		syntaxError("Code ends before file\n");

	/* 函数返回语法树根节点t */
	return t;
}

