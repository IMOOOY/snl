/****************************************************/
/* 文件 parseLL1.cpp								*/
/* 说明 类pascal编译器的语法分析器实现				*/
/* 主题 编译器结构：原理和实例						*/
/* 说明 采用LL1分析方法                             */
/****************************************************/


/***********  该文件所包含的头文件  ****************/

#include "globals.h"	/* 该头文件定义了全局类与变量 */

#include "util.h"		/* 该头文件定义了功能函数 */

#include "string.h"

#include "scanner.h"	/* 该头文件定义了词法扫描器界面 */

#include "parseLL1.h"	/* 该头文件定义了语法分析器界面 */


/*当前单词*/
TokenType  currentToken;
/*当前单词行号，用于给出错误提示信息*/
extern int  lineno;
/*LL1分析表*/
int LL1Table[TABLESIZE][TABLESIZE];
/*纪录当前语法树节点*/
TreeNode* currentP = NULL;

/*为保存类型需要的临时变量*/
DecKind* temp = NULL;
/*保存当前指针，以便修改后，将其恢复*/
TreeNode* saveP = NULL;

/*纪录表达式中，未匹配的左括号数目*/
int  expflag = 0;

/*判断简单表达式处理结束，整个表达式是否处理结束标识*/
/*当是条件表达式时，取假值，简单表达式时，取真值*/
/*用于函数preocess84*/
int  getExpResult = TRUE;

/*仅用于数组变量，故初始化为假，遇到数组变量时，将其
  改变为真，以便在函数process84中，即算术表达式结束时，
  从语法树栈中弹出相应指针，将数组下标表达式的结构链入
  节点中*/
int  getExpResult2 = FALSE;


/*符号栈顶指针*/
StackNode* StackTop;
/*栈空标志*/
int STACKEMPTY;

/*语法树栈顶指针*/
StackNodePA* StackTopPA;
/*栈空标志*/
int  paSTACKEMPTY;

/*操作符栈的栈顶指针*/
StackNodeP* OpStackTop = NULL;
/*操作符栈空标志*/
int OpSTACKEMPTY = TRUE;

/*操作数栈的栈顶指针*/
StackNodeP* NumStackTop = NULL;
/*操作数栈空标志*/
int NumSTACKEMPTY = TRUE;



/************<语法分析功能函数> **************/

/********************************************************/
/* 函数名  CreatLL1Table								*/
/* 功  能  创建LL1分析表								*/
/* 说  明  初始数组（表）中的每一项都为0；根据LL1文法   */
/*         给数组赋值（填表）；填好后，若值为0，		*/
/*         表示无产生式可选，其他，为选中的产生式  		*/
/********************************************************/

void CreatLL1Table()
{
	int i, j;

	/*初始化LL1表元素*/
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
/* 函数名 gettoken					     							*/
/* 功  能 从Token序列中取出一个Token	                            */
/* 说  明 从文件中存的Token序列中依次取一个单词，作为当前单词.      */
/********************************************************************/
int fpnum = 0;
void gettoken(TokenType* p)
{
	FILE* fp2;
	/*按只读方式打开文件*/
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
/* 函数名 syntaxError												*/
/* 功  能 语法错误处理函数											*/
/* 说  明 将函数参数message指定的错误信息格式化写入列表文件listing	*/
/*		  设置错误追踪标志Error为TRUE								*/
/********************************************************************/
static void syntaxError(char* message)

{
	fprintf(listing, "\n>>> ");

	/* 将出错行号lineno和语法错误信息message格式化写入文件listing */
	fprintf(listing, "Syntax error at line %d: %s", lineno, message);

	/* 设置错误追踪标志Error为TRUE,防止错误进一步传递 */
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
/* 函数名 process2() 				     							*/
/* 功  能 处理程序头，并生成程序头节点Phead.                        */
/* 说  明 产生式为：PROGRAM  ProgramName							*/
/********************************************************************/
void process2()
{
	Push(1, ProgramName);
	Push(2, PROGRAM);

	TreeNode** t = PopPA();
	currentP = newPheadNode();
	(*t) = currentP;
	/*程序头节点没有兄弟节点，下面的声明用用根节点的child[1]指向*/

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
	currentP = newTypeNode();  /*生成Type作为标志的结点，它的子结点都是
								 类型声明*/
	(*t) = currentP;    /*头结点的兄弟结点指针指向此结点*/
	PushPA(&((*currentP).sibling));  /* 压入指向变量声明节点的指针*/
	PushPA(&((*currentP).child[0])); /*压入指向第一个类型声明节点的指针*/
}

void process8()
{
	Push(1, TypeDecMore);
	Push(2, SEMI);
	Push(1, TypeName);
	Push(2, EQ);
	Push(1, TypeId);

	TreeNode** t = PopPA();
	currentP = newDecNode(); /*生成一个表示类型声明的结点，
							   不添任何信息*/

	(*t) = currentP; /*若是第一个，则是Type类型的子结点指向当前结点，
					   否则，是上一个类型声明的兄弟结点*/

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

	/*由于数组基类型的问题，这里不能直接用currentP->kind.dec=IntegerK;
	  而应该这么做，以适应所有情形*/
	temp = &(currentP->kind.dec);

}
void process13()
{
	Push(1, StructureType);
}

void process14()
{
	Push(2, ID);

	/*声明的类型部分为类型标识符*/
	(*currentP).kind.dec = IdK;
	strcpy(currentP->attr.type_name, currentToken.Sem);
}

void process15()
{
	Push(2, INTEGER);

	/*声明的类型部分为整数类型*/
	(*temp) = IntegerK;
}

void process16()
{
	Push(2, CHAR);

	/*声明的类型部分为子符类型*/
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

	/*声明的类型为数组类型*/
	(*currentP).kind.dec = ArrayK;
	temp = &(currentP->attr.ArrayAttr.childtype);

}
void process20()
{
	Push(2, INTC);

	/*存储数组的下届*/
	(*currentP).attr.ArrayAttr.low = atoi(currentToken.Sem);
}
void process21()
{
	Push(2, INTC);

	/*存储数组的上届*/
	(*currentP).attr.ArrayAttr.up = atoi(currentToken.Sem);
}
void process22()
{
	Push(2, END);
	Push(1, FieldDecList);
	Push(2, RECORD);

	/*声明的类型部分为记录类型*/
	(*currentP).kind.dec = RecordK;

	saveP = currentP; /*压入当前节点，是为了处理完后回到当前节点，
					  主要是为了变量声明部分标识符部分在后面，考虑*/
	PushPA(&((*currentP).child[0]));
}
void process23()
{
	Push(1, FieldDecMore);
	Push(2, SEMI);
	Push(1, IdList);
	Push(1, BaseType);

	TreeNode** t = PopPA();
	currentP = newDecNode(); /*生成记录类型的下一个域，不添任何信息*/
	temp = (&(currentP->kind.dec));
	(*t) = currentP; /*若是第一个，则是record类型的子结点指向当前结点，
						否则，是上一个纪录域声明的兄弟结点*/
	PushPA(&((*currentP).sibling));

}

void process24()
{
	Push(1, FieldDecMore);
	Push(2, SEMI);
	Push(1, IdList);
	Push(1, ArrayType);

	TreeNode** t = PopPA();
	currentP = newDecNode(); /*生成记录类型的下一个域，不添任何信息*/
	(*t) = currentP; /*若是第一个，则是record类型的子结点指向当前结点，
					   否则，是上一个纪录域声明的兄弟结点*/
	PushPA(&((*currentP).sibling));

}

void process25()
{
	/*后面没有记录类型的下一个域了，恢复当前纪录类型节点的指针*/
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

	/*纪录一个域中各个变量的语义信息*/
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

	currentP = newVarNode();   /*生成一个标志变量声明的节点*/
	TreeNode** t = PopPA();
	(*t) = currentP;
	PushPA(&((*currentP).sibling));  /*压入指向函数声明的指针*/
	PushPA(&((*currentP).child[0])); /*压入指向第一个变量声明节点的指针*/
}
void process33()
{
	Push(1, VarDecMore);
	Push(2, SEMI);
	Push(1, VarIdList);
	Push(1, TypeName);

	TreeNode** t = PopPA();
	currentP = newDecNode();/*建立一个新的声明节点，这里表示变量声明*/
	(*t) = currentP;       /*若是第一个节点，则变量声明的头指针指向它，
							 否则它是前一个变量声明的后继*/
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

	PushPA(&(currentP->child[2])); /*指向语句序列*/

	PushPA(&(currentP->child[1]));  /*指向函数的声明部分*/

	PushPA(&(currentP->child[0]));  /*指向参数声明部分*/
}

void process42()
{   /*弹出过程节点的兄弟节点指针*/
	//PopPA( );  /*为了统一处理，不能现在弹出*/
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
	/*形参部分为空，弹出指向形参的指针*/
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
	/*函数的参数类型是值类型*/
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
	/*函数的参数类型是变量类型*/
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

	/*注意，若没有声明部分，则弹出的是程序或过程根节点中指向
	  声明部分的指针child[1];若有声明部分，则弹出的是语句序列前
	  的最后一个声明标识节点的兄弟指针；不管是哪种情况，都正好
	  需要弹出语法树栈中的一个指针*/
	PopPA();

	/*建立语句序列标识节点*/
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

	/*赋值语句左部变量节点*/
	TreeNode* t = newExpNode(VariK);
	strcpy(t->name[0], currentToken.Sem);
	t->idnum++;

	/*赋值语句的child[0]指向左部的变量节点*/
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
	/*过程调用语句左部是标识符变量，表示过程名*/
	currentP->child[0]->attr.ExpAttr.varkind = IdV;

	currentP->kind.stmt = CallK;
}
void process69()
{
	Push(1, Exp);
	Push(2, ASSIGN);
	Push(1, VariMore);
	/*压入指向赋值右部的指针*/
	PushPA(&(currentP->child[1]));
	/*当前指针指向赋值左部*/
	currentP = currentP->child[0];

	TreeNode* t = newExpNode(OpK);
	t->attr.ExpAttr.op = END;
	PushOp(t);  //操作符栈的栈底存入一个特殊的操作符作为标志
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
	PushOp(t);  //操作符栈的栈底存入一个特殊的操作符作为标志
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
	PushOp(t);  //操作符栈的栈底存入一个特殊的操作符作为标志
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


/*设定操作符的优先级，值越大优先级越高*/
int  Priosity(LexType  op)
{
	int  pri = 0;
	switch (op)
	{
	case END:
		pri = -1; break;//栈底标识，优先级最低
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

/*********************表达式部分************************/

void process81()
{
	Push(1, OtherRelE);
	Push(1, Exp);

	TreeNode* t = newExpNode(OpK);
	t->attr.ExpAttr.op = END;
	PushOp(t);  //操作符栈的栈底存入一个特殊的操作符作为标志	

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
		/*如果操作符栈顶运算符的优先级高于或等于当前读到的操作符*/
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
	/*处理完关系操作符右部的表达式时，要弹语法树栈，故
	  设置getExpResult为真*/
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
		//说明当前右括号是表达式中的一部分
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
		PopOp(); //弹出左括号
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
			PopOp();//弹出栈底标志
			currentP = PopNum();

			TreeNode** t = PopPA();
			(*t) = currentP;

			/*处理完数组变量，标志恢复初始值假，
			  遇到下一个数组下标表达式时，再将其设置为真值*/
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
		/*如果操作符栈顶运算符的优先级高于或等于当前读到的操作符*/
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
		/*如果操作符栈顶运算符的优先级高于或等于当前读到的操作符*/
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
	t->attr.ExpAttr.op = currentToken.Lex; /*把左括号也压入栈中*/
	PushOp(t);
	expflag++;
}

void process90()
{
	Push(2, INTC);

	TreeNode* t = newExpNode(ConstK);
	t->attr.ExpAttr.val = atoi(currentToken.Sem);
	/*常数节点入操作数栈*/
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
	/*变量节点入操作数栈*/
	PushNum(currentP);

}

void process93()
{
	/*标识符变量*/
	currentP->attr.ExpAttr.varkind = IdV;
}

void process94()
{
	Push(2, RMIDPAREN);
	Push(1, Exp);
	Push(2, LMIDPAREN);
	/*数组成员变量*/
	currentP->attr.ExpAttr.varkind = ArrayMembV;
	PushPA(&currentP->child[0]);

	/*要进入表达式处理，初始化操作符栈*/
	//操作符栈的栈底存入一个特殊的操作符作为标志
	TreeNode* t = newExpNode(OpK);
	t->attr.ExpAttr.op = END;
	PushOp(t);

	/*要进入数组下标表达式处理，在函数process84处理中，要
	  操作语法树栈，故将标志getExpResult2设置为真值*/
	getExpResult2 = TRUE;


}

void process95()
{
	Push(1, FieldVar);
	Push(2, DOT);
	/*域成员变量*/
	currentP->attr.ExpAttr.varkind = FieldMembV;
	PushPA(&currentP->child[0]);
}

void process96()
{
	Push(1, FieldVarMore);
	Push(2, ID);

	/*纪录域的成员*/
	currentP = newExpNode(VariK);
	strcpy(currentP->name[0], currentToken.Sem);
	currentP->idnum++;

	TreeNode** t = PopPA();
	(*t) = currentP;


}

void process97()
{
	/*域成员是标识符变量*/
	currentP->attr.ExpAttr.varkind = IdV;
}

void process98()
{
	Push(2, RMIDPAREN);
	Push(1, Exp);
	Push(2, LMIDPAREN);
	/*域成员是数组变量*/
	currentP->attr.ExpAttr.varkind = ArrayMembV;
	/*指向数组成员表达式*/
	PushPA(&currentP->child[0]);

	//操作符栈的栈底存入一个特殊的操作符作为标志
	TreeNode* t = newExpNode(OpK);
	t->attr.ExpAttr.op = END;
	PushOp(t);

	/*要进入数组下标表达式处理，在函数process84处理中，要
	  操作语法树栈，故将标志getExpResult2设置为真值*/
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
/* 函数名  predict									*/
/* 功  能  选择产生式函数							*/
/* 说  明  										    */
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
/* 函数名  parseLL1									*/
/* 功  能  LL1语法分析主函数						*/
/* 说  明  										    */
/****************************************************/

TreeNode* parseLL1()
{

	NontmlType  stacktopN;

	TmlType     stacktopT;

	/*语法树的根节点*/
	TreeNode* rootPointer = NULL;

	int pnum = 0; //纪录选中的产生式编号

	CreatLL1Table();

	STACKEMPTY = TRUE;

	/*指向整个语法树根节点的指针，由它得到语法树*/
	rootPointer = newRootNode();

	/*从这里开始进行语法分析和语法树的生成*/
	PushPA(&(rootPointer->child[2]));
	PushPA(&(rootPointer->child[1]));
	PushPA(&(rootPointer->child[0]));

	Push(1, Program);

	/*取一个token*/
	gettoken(&currentToken);
	lineno = currentToken.lineshow;


	while (!(STACKEMPTY))
	{
		if (readStackflag() == 2) /*检测终极符是否匹配*/
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
		else {  /*根据非终极符和栈中符号进行预测*/
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




