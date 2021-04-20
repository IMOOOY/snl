/****************************************************/
/* �ļ� parse.c										*/
/* ˵�� TINY���������﷨������ʵ��					*/
/* ���� �������ṹ��ԭ����ʵ��						*/
/****************************************************/

/***********  ���ļ���������ͷ�ļ�  ****************/

#include "globals.h"	

#include "util.h"		

#include "scanner.h"	

#include "parse.h"		

#include "string.h"

static TokenType token;

char* temp_name;

static int line0;

/********** �ݹ���õĺ���ԭ�� ************/

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

static TreeNode* exp(void);			/* ��������ʽ���� */

static TreeNode* simple_exp(void);		/* �����򵥱���ʽ���� */

static TreeNode* term(void);			/* ������� */

static TreeNode* factor(void);			/* �������Ӻ��� */

static TreeNode* variable(void);

static void variMore(TreeNode* t);

static TreeNode* fieldvar(void);

static void fieldvarMore(TreeNode* t);



/************ �﷨�������ܺ��� **************/

/********************************************************************/
/* ������ syntaxError												*/
/* ��  �� �﷨����������											*/
/* ˵  �� ����������messageָ���Ĵ�����Ϣ��ʽ��д���б��ļ�listing	*/
/*		  ���ô���׷�ٱ�־ErrorΪTRUE								*/
/********************************************************************/
static void syntaxError(char* message)

{
	fprintf(listing, "\n>>> error :   ");
	fprintf(listing, "Syntax error at line %d: %s\n", token.lineshow, message);
	Error = TRUE;
}

/********************************************************************/
/* ������ match														*/
/* ��  �� �ռ���ƥ�䴦������										*/
/* ˵  �� ��������expected�����������ʷ����뵱ǰ���ʷ���token��ƥ��	*/
/*        �����ƥ��,�򱨷����������﷨����							*/
/********************************************************************/
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

/********************************************************************/
/* ������ program												    */
/* ��  �� �ܳ���Ĵ�������								        	*/
/* ����ʽ < program > ::= programHead declarePart programBody .     */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/*        �﷨���ĸ��ڵ�ĵ�һ���ӽڵ�ָ�����ͷ����programHead,    */
/*        DeclaraPartΪprogramHead���ֵܽڵ�,�����岿��programBody  */
/*        ΪdeclarePart���ֵܽڵ�.                                  */
/********************************************************************/
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

/********************************************************************/
/* ������ programHead											    */
/* ��  �� ����ͷ�Ĵ�������								        	*/
/* ����ʽ < programHead > ::= PROGRAM  ProgramName                  */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/*************************��������***********************************/

/********************************************************************/
/* ������ declarePart											    */
/* ��  �� �������ֵĴ�������								     	*/
/* ����ʽ < declarePart > ::= typeDec  varDec  procDec              */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
TreeNode* declarePart(void)
{
	/*����*/
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
	/*����*/
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
	/*����*/
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



/**************************������������******************************/

/********************************************************************/
/* ������ typeDec									     		    */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < typeDec > ::= �� | TypeDeclaration                      */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ TypeDeclaration									  	    */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < TypeDeclaration > ::= TYPE  TypeDecList                 */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ TypeDecList		 							  	        */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < TypeDecList > ::= typeId = typeName ; typeDecMore       */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ typeDecMore		 							            */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < typeDecMore > ::=    �� | TypeDecList                   */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ typeId		 							  	            */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < typeId > ::= id                                         */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ typeName		 							  	            */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < typeName > ::= baseType | structureType | id            */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ baseType		 							  	            */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < baseType > ::=  INTEGER | CHAR                          */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ structureType		 							            */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < structureType > ::=  arrayType | recType                */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ arrayType		 							                */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < arrayType > ::=  ARRAY [low..top] OF baseType           */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ recType		 							                */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < recType > ::=  RECORD fieldDecList END                  */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ fieldDecList		 							            */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < fieldDecList > ::=   baseType idList ; fieldDecMore     */
/*                             | arrayType idList; fieldDecMore     */
/*˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�   */
/********************************************************************/
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


/********************************************************************/
/* ������ fieldDecMore		 							            */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < fieldDecMore > ::=  �� | fieldDecList                   */
/*˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�   */
/********************************************************************/
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


/********************************************************************/
/* ������ idList		 						     	            */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < idList > ::=  id  idMore                                */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ idMore		 						     	            */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < idMore > ::=  �� |  , idList                            */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/**************************������������******************************/

/********************************************************************/
/* ������ varDec		 						     	            */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < varDec > ::=  �� |  varDeclaration                      */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ varDeclaration		 						            */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < varDeclaration > ::=  VAR  varDecList                   */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
TreeNode* varDeclaration(void)
{
	match(VAR);
	TreeNode* t = varDecList();
	if (t == NULL)
		syntaxError("a var declaration is expected!");
	return t;
}


/********************************************************************/
/* ������ varDecList		 						                */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < varDecList > ::=  typeName varIdList; varDecMore        */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ varDecMore		 						                */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < varDecMore > ::=  �� |  varDecList                      */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ varIdList		 						                    */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < varIdList > ::=  id  varIdMore                          */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ varIdMore		 						                    */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < varIdMore > ::=  �� |  , varIdList                      */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/****************************������������****************************/

/********************************************************************/
/* ������ procDec		 						                    */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < procDec > ::=  �� |  procDeclaration                    */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ procDeclaration		 						            */
/* ��  �� �����������ֵĴ�������						        	*/
/* ����ʽ < procDeclaration > ::=  PROCEDURE                        */
/*                                 ProcName(paramList);             */
/*                                 procDecPart                      */
/*                                 procBody                         */
/*                                 procDec                          */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/*        �����ĸ��ڵ����ڼ�¼�ú��������֣���һ���ӽڵ�ָ�������  */
/*        �㣬�ڶ����ڵ�ָ�����е��������ֽڵ㣻�������ڵ�ָ��  */
/*        ���塣
/********************************************************************/
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


/********************************************************************/
/* ������ paramList		 						                    */
/* ��  �� ���������в����������ֵĴ�������	        	        	*/
/* ����ʽ < paramList > ::=  �� |  paramDecList                     */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ paramDecList		 			    	                    */
/* ��  �� ���������в����������ֵĴ�������	        	        	*/
/* ����ʽ < paramDecList > ::=  param  paramMore                    */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ paramMore		 			    	                        */
/* ��  �� ���������в����������ֵĴ�������	        	        	*/
/* ����ʽ < paramMore > ::=  �� | ; paramDecList                     */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ param		 			    	                            */
/* ��  �� ���������в����������ֵĴ�������	        	        	*/
/* ����ʽ < param > ::=  typeName formList | VAR typeName formList  */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ formList		 			    	                        */
/* ��  �� ���������в����������ֵĴ�������	        	        	*/
/* ����ʽ < formList > ::=  id  fidMore                             */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ fidMore		 			    	                        */
/* ��  �� ���������в����������ֵĴ�������	        	        	*/
/* ����ʽ < fidMore > ::=   �� |  , formList                        */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ procDecPart		 			  	                        */
/* ��  �� �����е��������ֵĴ�������	             	        	*/
/* ����ʽ < procDecPart > ::=  declarePart                          */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
TreeNode* procDecPart(void)
{
	TreeNode* t = declarePart();
	return t;
}

/********************************************************************/
/* ������ procBody		 			  	                            */
/* ��  �� �����岿�ֵĴ�������	                    	        	*/
/* ����ʽ < procBody > ::=  programBody                             */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
TreeNode* procBody(void)
{
	TreeNode* t = programBody();
	if (t == NULL)
		syntaxError("a program body is requested!");
	return t;
}

/********************************************************************/
/* ������ programBody		 			  	                        */
/* ��  �� �����岿�ֵĴ�������	                    	        	*/
/* ����ʽ < programBody > ::=  BEGIN  stmList   END                 */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ stmList		 			  	                            */
/* ��  �� ��䲿�ֵĴ�������	                    	        	*/
/* ����ʽ < stmList > ::=  stm    stmMore                           */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
TreeNode* stmList(void)
{
	TreeNode* t = stm();
	TreeNode* p = stmMore();
	if (t != NULL)
		if (p != NULL)
			t->sibling = p;
	return t;
}

/********************************************************************/
/* ������ stmMore		 			  	                            */
/* ��  �� ��䲿�ֵĴ�������	                    	        	*/
/* ����ʽ < stmMore > ::=   �� |  ; stmList                         */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ stm   		 			  	                            */
/* ��  �� ��䲿�ֵĴ�������	                    	        	*/
/* ����ʽ < stm > ::=   conditionalStm   {IF}                       */
/*                    | loopStm          {WHILE}                    */
/*                    | inputStm         {READ}                     */
/*                    | outputStm        {WRITE}                    */
/*                    | returnStm        {RETURN}                   */
/*                    | id  assCall      {id}                       */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ assCall		 			  	                            */
/* ��  �� ��䲿�ֵĴ�������	                    	        	*/
/* ����ʽ < assCall > ::=   assignmentRest   {:=,LMIDPAREN,DOT}     */
/*                        | callStmRest      {(}                    */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ assignmentRest		 			                        */
/* ��  �� ��ֵ��䲿�ֵĴ�������	                    	        */
/* ����ʽ < assignmentRest > ::=  variMore : = exp                  */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
TreeNode* assignmentRest(void)
{
	TreeNode* t = newStmtNode(AssignK);

	/* ��ֵ���ڵ�ĵ�һ�����ӽڵ��¼��ֵ��������������*
	/* �ڶ������ӽ���¼��ֵ�����Ҳ����ʽ*/
	if (t != NULL)
	{
		t->lineno = line0;

		/*������һ�����ӽ�㣬Ϊ��������ʽ���ͽڵ�*/
		TreeNode* child1 = newExpNode(VariK);
		if (child1 != NULL)
		{
			child1->lineno = line0;
			strcpy(child1->name[0], temp_name);
			(child1->idnum)++;
			variMore(child1);
			t->child[0] = child1;
		}

		/*��ֵ��ƥ��*/
		match(ASSIGN);

		/*�����ڶ������ӽڵ�*/
		t->child[1] = exp();

	}
	return t;
}

/********************************************************************/
/* ������ conditionalStm		 			                        */
/* ��  �� ������䲿�ֵĴ�������	                    	        */
/* ����ʽ < conditionalStm > ::= IF exp THEN stmList ELSE stmList FI*/
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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


/********************************************************************/
/* ������ loopStm          		 			                        */
/* ��  �� ѭ����䲿�ֵĴ�������	                    	        */
/* ����ʽ < loopStm > ::=      WHILE exp DO stmList ENDWH           */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ inputStm          		     	                        */
/* ��  �� ������䲿�ֵĴ�������	                    	        */
/* ����ʽ < inputStm > ::=    READ(id)                              */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ outputStm          		     	                        */
/* ��  �� �����䲿�ֵĴ�������	                    	        */
/* ����ʽ < outputStm > ::=   WRITE(exp)                            */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ returnStm          		     	                        */
/* ��  �� ������䲿�ֵĴ�������	                    	        */
/* ����ʽ < returnStm > ::=   RETURN                                */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
TreeNode* returnStm(void)
{
	TreeNode* t = newStmtNode(ReturnK);
	match(RETURN);
	if (t != NULL)
		t->lineno = line0;
	return t;
}

/********************************************************************/
/* ������ callStmRest          		     	                        */
/* ��  �� ����������䲿�ֵĴ�������	                  	        */
/* ����ʽ < callStmRest > ::=  (actParamList)                       */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
TreeNode* callStmRest(void)
{
	TreeNode* t = newStmtNode(CallK);
	match(LPAREN);
	/*��������ʱ�����ӽڵ�ָ��ʵ��*/
	if (t != NULL)
	{
		t->lineno = line0;

		/*�������Ľ��Ҳ�ñ���ʽ���ͽ��*/
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

/********************************************************************/
/* ������ actParamList          		   	                        */
/* ��  �� ��������ʵ�β��ֵĴ�������	                	        */
/* ����ʽ < actParamList > ::=     �� |  exp actParamMore           */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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

/********************************************************************/
/* ������ actParamMore          		   	                        */
/* ��  �� ��������ʵ�β��ֵĴ�������	                	        */
/* ����ʽ < actParamMore > ::=     �� |  , actParamList             */
/* ˵  �� ���������ķ�����ʽ,������Ӧ�ĵݹ鴦������,�����﷨���ڵ�  */
/********************************************************************/
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



/*************************����ʽ����********************************/
/****************************************************************************/
/* ������ exp																*/
/* ��  �� ����ʽ��������													*/
/* ����ʽ < ����ʽ > ::= < �򵥱���ʽ > [< ��ϵ����� > < �򵥱���ʽ > ]	*/
/* ˵  �� �ú������ݲ���ʽ������Ӧ�ݹ鴦������,���ɱ���ʽ�����﷨���ڵ�		*/
/****************************************************************************/
TreeNode* exp(void)

{
	/* ���ü򵥱���ʽ��������simple_exp(),�����﷨���ڵ�ָ���t */
	TreeNode* t = simple_exp();

	/* ��ǰ����tokenΪ�߼����㵥��LT����EQ */
	if ((token.Lex == LT) || (token.Lex == EQ))
	{
		/* �����µ�OpK�����﷨���ڵ㣬���﷨���ڵ�ָ�븳��p */
		TreeNode* p = newExpNode(OpK);

		/* ���﷨���ڵ�p�����ɹ�,��ʼ��p��һ���ӽڵ��Աchild[0]
		 * ������ǰ����token(ΪEQ����LT)�����﷨���ڵ�p���������Աattr.op*/
		if (p != NULL)
		{
			p->lineno = line0;
			p->child[0] = t;
			p->attr.ExpAttr.op = token.Lex;

			/* ���µı���ʽ�����﷨���ڵ�p��Ϊ��������ֵt */
			t = p;
		}

		/* ��ǰ����token��ָ���߼����������(ΪEQ����LT)ƥ�� */
		match(token.Lex);

		/* �﷨���ڵ�t�ǿ�,���ü򵥱���ʽ��������simple_exp()	*
		 * ���������﷨���ڵ�ָ���t�ĵڶ��ӽڵ��Աchild[1]	*/
		if (t != NULL)
			t->child[1] = simple_exp();
	}

	/* �������ر���ʽ�����﷨���ڵ�t */
	return t;
}


/************************************************************************/
/* ������ simple_exp													*/
/* ��  �� �򵥱���ʽ��������											*/
/* ����ʽ < �򵥱���ʽ >::=	< �� > { < �ӷ������ > < �� > }			*/
/* ˵  �� �ú������ݲ���ʽ������Ӧ�ݹ鴦������,���ɱ���ʽ�����﷨���ڵ�	*/
/************************************************************************/
TreeNode* simple_exp(void)

{
	/* ����Ԫ��������term(),���������﷨���ڵ�ָ���t */
	TreeNode* t = term();

	/* ��ǰ����tokenΪ�ӷ����������PLUS��MINUS */
	while ((token.Lex == PLUS) || (token.Lex == MINUS))

	{
		/* ������OpK����ʽ�����﷨���ڵ㣬���﷨���ڵ�ָ�븳��p */
		TreeNode* p = newExpNode(OpK);

		/* �﷨���ڵ�p�����ɹ�,��ʼ��p��һ�ӽڵ��Աchild[0]	*
		 * �����﷨���ڵ�ָ���p���������Աattr.op				*/
		if (p != NULL)
		{
			p->lineno = line0;
			p->child[0] = t;
			p->attr.ExpAttr.op = token.Lex;

			/* ����������ֵt�����﷨���ڵ�p */
			t = p;

			/* ��ǰ����token��ָ���ӷ����㵥��(ΪPLUS��MINUS)ƥ�� */
			match(token.Lex);

			/* ����Ԫ��������term(),���������﷨���ڵ�ָ���t�ĵڶ��ӽڵ��Աchild[1] */
			t->child[1] = term();
		}
	}
	/* �������ر���ʽ�����﷨���ڵ�t */
	return t;
}



/****************************************************************************/
/* ������ term																*/
/* ��  �� �������														*/
/* ����ʽ < �� > ::= < ���� > { < �˷������ > < ���� > }					*/
/* ˵  �� �ú������ݲ���ʽ������Ӧ�ݹ鴦������,���ɱ���ʽ�����﷨���ڵ�		*/
/****************************************************************************/
TreeNode* term(void)

{
	/* �������Ӵ�������factor(),���������﷨���ڵ�ָ���t */
	TreeNode* t = factor();

	/* ��ǰ����tokenΪ�˷����������TIMES��OVER */
	while ((token.Lex == TIMES) || (token.Lex == OVER))

	{
		/* �����µ�OpK����ʽ�����﷨���ڵ�,�½ڵ�ָ�븳��p */
		treeNode* p = newExpNode(OpK);

		/* ���﷨���ڵ�p�����ɹ�,��ʼ����һ���ӽڵ��Աchild[0]Ϊt	*
		 * ����ǰ����token��ֵ���﷨���ڵ�p���������Աattr.op		*/
		if (p != NULL)
		{
			p->lineno = line0;
			p->child[0] = t;
			p->attr.ExpAttr.op = token.Lex;
			t = p;
		}

		/* ��ǰ����token��ָ���˷����������(ΪTIMES��OVER)ƥ�� */
		match(token.Lex);

		/* �������Ӵ�������factor(),���������﷨���ڵ�ָ�븳��p�ڶ����ӽڵ��Աchild[1] */
		p->child[1] = factor();

	}
	/* �������ر���ʽ�����﷨���ڵ�t */
	return t;
}


/****************************************************************************/
/* ������ factor															*/
/* ��  �� ���Ӵ�������														*/
/* ����ʽ factor ::= ( exp ) | INTC | variable                  			*/
/* ˵  �� �ú������ݲ���ʽ������Ӧ�ĵݹ鴦������,���ɱ���ʽ�����﷨���ڵ�	*/
/****************************************************************************/
TreeNode* factor(void)

{
	/* ���������﷨���ڵ�ָ��t��ʼΪΪNULL */
	TreeNode* t = NULL;

	switch (token.Lex)
	{
	case INTC:

		/* �����µ�ConstK����ʽ�����﷨���ڵ�,��ֵ��t */
		t = newExpNode(ConstK);

		/* ���﷨���ڵ�t�����ɹ�,��ǰ����tokenΪ���ֵ���NUM						*
		 * ����ǰ������tokenStringת��Ϊ�����������﷨���ڵ�t����ֵ��Աattr.val	*/
		if ((t != NULL) && (token.Lex == INTC))
		{
			t->lineno = line0;
			t->attr.ExpAttr.val = atoi(token.Sem);
		}

		/* ��ǰ����token�����ֵ���NUMƥ�� */
		match(INTC);
		break;

		/* ��ǰ����tokenΪ��ʶ������ID */
	case ID:

		/* �����µ�IdK����ʽ�����﷨���ڵ�t */
		t = variable();
		break;

		/* ��ǰ����tokenΪ�����ŵ���LPAREN */
	case LPAREN:

		/* ��ǰ����token�������ŵ���LPARENƥ�� */
		match(LPAREN);

		/* ���ñ���ʽ��������exp(),���������﷨���ڵ�ָ���t */
		t = exp();

		/* ��ǰ����token�������ŵ���RPARENƥ�� */
		match(RPAREN);

		break;

		/* ��ǰ����tokenΪ�������� */
	default:
		ReadNextToken(&token);
		syntaxError("unexpected token is here!");
		break;
	}
	/* �������ر���ʽ�����﷨���ڵ�t */
	return t;
}

/********************************************************************/
/* ������ variable													*/
/* ��  �� ������������												*/
/* ����ʽ variable   ::=   id variMore                   			*/
/* ˵  �� �ú������ݲ���ʽ,	�����������������﷨���ڵ�              */
/********************************************************************/
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


/********************************************************************/
/* ������ variMore													*/
/* ��  �� ������������												*/
/* ����ʽ variMore   ::=  ��                             			*/
/*                       | [exp]            {[}                     */
/*                       | . fieldvar       {DOT}                   */
/* ˵  �� �ú������ݲ���ʽ������Ӧ�ĵݹ鴦�������еļ��ֲ�ͬ����	*/
/********************************************************************/
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

		/*�����Ժ���������ʽ��ֵ���������������±����*/
		t->child[0] = exp();

		t->attr.ExpAttr.varkind = ArrayMembV;

		/*�˱���ʽΪ�����Ա��������*/
		t->child[0]->attr.ExpAttr.varkind = IdV;
		match(RMIDPAREN);
		break;
	case DOT:
		match(DOT);
		/*��һ������ָ�����Ա�������*/
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


/********************************************************************/
/* ������ fieldvar													*/
/* ��  �� ������������												*/
/* ����ʽ fieldvar   ::=  id  fieldvarMore                          */
/* ˵  �� �ú������ݲ���ʽ����������������������﷨���ڵ�       	*/
/********************************************************************/
TreeNode* fieldvar(void)
{
	/*ע�⣬�ɷ񽫴˴���IdEK��Ϊһ���µı�ʶ��������¼��¼���͵���*/
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

/********************************************************************/
/* ������ fieldvarMore  											*/
/* ��  �� ������������												*/
/* ����ʽ fieldvarMore   ::=  ��                             		*/
/*                           | [exp]            {[}                 */
/* ˵  �� �ú������ݲ���ʽ������Ӧ�ĵݹ鴦�������Ϊ�������͵����	*/
/********************************************************************/
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

		/*�����Ժ���������ʽ��ֵ���������������±����*/
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


/********************************************************************/
/* ������ parse														*/
/* ��  �� �﷨��������												*/
/* ˵  �� �ú����Ѵʷ�����������Ϊ�ӳ������,���õݹ��½���			*/
/*		  ���ݲ���ʽ���õݹ鴦������,����ΪԴ���򴴽��﷨������		*/
/********************************************************************/
TreeNode* parse(void)

{
	TreeNode* t = NULL;

	/* ���ļ�Tokenlist��ȡ�õ�һ������,���ʷ���Ϣ�͸�token */

	ReadNextToken(&token);

	/* ��ʼ���û����﷨������������,�ݹ��½����� */
	t = program();

	/* ��ǰ����token����ENDFILE,���������ļ�����ǰ��ǰ�������� */
	if (token.Lex != ENDFILE)
		syntaxError("Code ends before file\n");

	/* ���������﷨�����ڵ�t */
	return t;
}
