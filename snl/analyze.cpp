/****************************************************/
/* 文件 analyze.cpp				  					*/
/* 说明 类PASCAL语言编译器语义分析程序  			*/
/* 主题 编译器结构:原理和实例						*/
/****************************************************/

/***********  该文件所包含的头文件  ****************/

#include "globals.h"	

#include "util.h"

#include "symbTable.h"		

#include "scanner.h"	

#include "parse.h"

#include "analyze.h"		

#include "string.h"

/*scope栈的层数*/
int Level = -1;

/*在同层的变量偏移*/
int Off;

/*主程序的noff偏移*/
int mainOff;


SymbTable* scope[SCOPESIZE];   /*scope栈*/

TypeIR* intPtr = NULL;			/*该指针一直指向整数类型的内部表示*/

TypeIR* charPtr = NULL;		/*该指针一直指向字符类型的内部表示*/

TypeIR* boolPtr = NULL;		/*该指针一直指向布尔类型的内部表示*/

TypeIR* arrayVar(TreeNode* t);
/*该函数用于处理数组类型变量*/

TypeIR* recordVar(TreeNode* t);
/*该函数用于处理记录类型变量*/

/***********************类型处理*****************************/



/*************************************************************/
/* 函数名  initialize                                        */
/* 功  能  该函数初始化整数类型，字符类型，布尔类型的内部表示*/
/* 说  明  由于这三种类型均为基本类型，内部表示固定。        */
/*************************************************************/
void initialize(void)
{
	intPtr = NewTy(intTy);

	charPtr = NewTy(charTy);

	boolPtr = NewTy(boolTy);

	/*scope栈的各层指针设为空*/
	for (int i = 0; i < SCOPESIZE; i++)
		scope[i] = NULL;
}

/************************************************************/
/* 函数名  TypeProcess                                      */
/* 功  能  该函数用来完成类型分析的工作                     */
/* 说  明  处理语法树的当前结点类型。构造出当前类型的内部表 */
/*         示，并将其地址返回给Ptr类型内部表示的地址.       */
/************************************************************/
TypeIR* TypeProcess(TreeNode* t, DecKind deckind)
{
	TypeIR* Ptr = NULL;
	switch (deckind)
	{
	case IdK:
		Ptr = nameType(t); break;         /*类型为自定义标识符*/
	case ArrayK:
		Ptr = arrayType(t); break;        /*类型为数组类型*/
	case RecordK:
		Ptr = recordType(t); break;       /*类型为记录类型*/
	case IntegerK:
		Ptr = intPtr; break;              /*类型为整数类型*/
	case CharK:
		Ptr = charPtr; break;             /*类型为字符类型*/
	}
	return Ptr;
}


/************************************************************/
/* 函数名  nameType                                         */
/* 功  能  该函数用来在符号表中寻找已定义的类型名字         */
/* 说  明  调用寻找表项地址函数FindEntry，返回找到的表项地址*/
/*		   指针entry。如果present为FALSE，则发生无声明错误。*/
/*         如果符号表中的该标识符的属性信息不是类型，则非类 */
/*         型标识符。该函数返回指针指向符号表中的该标识符的 */
/*	       类型内部表示。								    */
/************************************************************/
TypeIR* nameType(TreeNode* t)
{
	TypeIR* Ptr = NULL;
	SymbTable* entry = NULL;
	int present;


	/*类型标识符也需要往前层查找*/
	present = FindEntry(t->attr.type_name, &entry);

	if (present == TRUE)
	{
		/*检查该标识符是否为类型标识符*/
		if (entry->attrIR.kind != typeKind)
			ErrorPrompt(t->lineno, t->attr.type_name, (char*)"used before typed!\n");
		else
			Ptr = entry->attrIR.idtype;
	}
	else/*没有找到该标识符*/
	{
		ErrorPrompt(t->lineno, t->attr.type_name, (char*)"type name is not declared!\n");
	}
	return Ptr;
}

/************************************************************/
/* 函数名  arrayType                                        */
/* 功  能  该函数处理数组类型的内部表示                     */
/* 说  明  类型为数组类型时，需要检查下标是否合法。         */
/************************************************************/
TypeIR* arrayType(TreeNode* t)
{
	TypeIR* Ptr0 = NULL;
	TypeIR* Ptr1 = NULL;
	TypeIR* Ptr = NULL;

	/*检查数组上界是否小于下界*/
	if ((t->attr.ArrayAttr.low) > (t->attr.ArrayAttr.up))
	{
		ErrorPrompt(t->lineno, (char *)"", (char*)"array subscript error!\n");
		Error = TRUE;
	}
	else
	{
		Ptr0 = TypeProcess(t, IntegerK);
		/*调用类型分析函数，处理下标类型*/
		Ptr1 = TypeProcess(t, t->attr.ArrayAttr.childtype);
		/*调用类型分析函数，处理元素类型*/
		Ptr = NewTy(arrayTy);
		/*指向一新创建的类型信息表*/
		Ptr->size = ((t->attr.ArrayAttr.up) - (t->attr.ArrayAttr.low) + 1) * (Ptr1->size);
		/*计算本类型长度*/

/*填写其他信息*/
		Ptr->More.ArrayAttr.indexTy = Ptr0;
		Ptr->More.ArrayAttr.elemTy = Ptr1;
		Ptr->More.ArrayAttr.low = t->attr.ArrayAttr.low;
		Ptr->More.ArrayAttr.up = t->attr.ArrayAttr.up;
	}
	return Ptr;
}


/************************************************************/
/* 函数名  recordType                                       */
/* 功  能  该函数处理记录类型的内部表示                     */
/* 说  明  类型为记录类型时，是由记录体组成的。其内部节点需 */
/*		   要包括3个信息:一是空间大小size；二是类型种类标志 */
/*		   recordTy;三是体部分的节点地址body。记录类型中的  */
/*		   域名都是标识符的定义性出现，因此需要记录其属性。 */
/************************************************************/
TypeIR* recordType(TreeNode* t)
{
	TypeIR* Ptr = NewTy(recordTy);  /*新建记录类型的节点*/

	t = t->child[0];                /*从语法数的儿子节点读取域信息*/


	fieldChain* Ptr2 = NULL;
	fieldChain* Ptr1 = NULL;

	fieldChain* body = NULL;

	while (t != NULL)				/*循环处理*/
	{
		/*填写ptr2指向的内容节点*
		 *此处循环是处理此种情况int a,b; */
		for (int i = 0; i < t->idnum; i++)
		{
			/*申请新的域类型单元结构Ptr2*/
			Ptr2 = NewBody();
			if (body == NULL)
				body = Ptr1 = Ptr2;

			/*填写Ptr2的各个成员内容*/
			strcpy(Ptr2->id, t->name[i]);
			Ptr2->UnitType = TypeProcess(t, t->kind.dec);

			Ptr2->Next = NULL;

			/*如果Ptr1!=Ptr2，那么将指针后移*/
			if (Ptr2 != Ptr1)
			{
				/*计算新申请的单元off*/
				Ptr2->off = (Ptr1->off) + (Ptr1->UnitType->size);
				Ptr1->Next = Ptr2;
				Ptr1 = Ptr2;
			}
		}
		/*处理完同类型的变量后，取语法树的兄弟节点*/
		t = t->sibling;
	}

	/*处理记录类型内部结构*/

	/*取Ptr2的off为最后整个记录的size*/
	Ptr->size = Ptr2->off + (Ptr2->UnitType->size);
	/*将域链链入记录类型的body部分*/
	Ptr->More.body = body;

	return Ptr;
}



/************************************************************/



/*********************声明的语义分析*************************/

/************************************************************/
/* 函数名  TypeDecPart                                      */
/* 功  能  该函数处理类型声明的语义分析                     */
/* 说  明  遇到类型T时，构造其内部节点TPtr；对于"idname=T"构*/
/*		   造符号表项；检查本层类型声明中是否有重复定义错误.*/
/************************************************************/
void TypeDecPart(TreeNode* t)
{
	int present = FALSE;

	AttributeIR  attrIr;

	SymbTable* entry = NULL;

	/*添属性作为参数*/
	attrIr.kind = typeKind;

	/*遍历语法树的兄弟节点*/
	while (t != NULL)
	{
		/*调用记录属性函数，返回是否重复声明错和入口地址*/
		present = Enter(t->name[0], &attrIr, &entry);

		if (present != FALSE)
		{
			ErrorPrompt(t->lineno, t->name[0], (char*)"is repetation declared!\n");
			entry = NULL;
		}
		else
			entry->attrIR.idtype = TypeProcess(t, t->kind.dec);
		t = t->sibling;
	}
}


/************************************************************/
/* 函数名  VarDecPart                                       */
/* 功  能  该函数处理变量声明的语义分析                     */
/* 说  明  调用变量处理分析函数                             */
/************************************************************/
void VarDecPart(TreeNode* t)
{
	varDecList(t);
}

/************************************************************/
/* 函数名  varDecList                                       */
/* 功  能  该函数处理变量声明的语义分析                     */
/* 说  明  当遇到变量表识符id时，把id登记到符号表中；检查重 */
/*         复性定义；遇到类型时，构造其内部表示。           */
/************************************************************/
void  varDecList(TreeNode* t)
{
	AttributeIR  attrIr;
	int present = FALSE;

	SymbTable* entry = NULL;

	while (t != NULL)	/*循环过程*/
	{
		attrIr.kind = varKind;
		for (int i = 0; i < (t->idnum); i++)
		{
			attrIr.idtype = TypeProcess(t, t->kind.dec);

			/*判断识值参还是变参acess(dir,indir)*/
			if (t->attr.ProcAttr.paramt == varparamType)
			{
				attrIr.More.VarAttr.access = indir;
				attrIr.More.VarAttr.level = Level;
				/*计算形参的偏移*/

				attrIr.More.VarAttr.off = Off;
				Off = Off + 1;
			}/*如果是变参，则偏移加1*/

			else
			{
				attrIr.More.VarAttr.access = dir;
				attrIr.More.VarAttr.level = Level;
				/*计算值参的偏移*/
				if (attrIr.idtype != NULL)

				{
					attrIr.More.VarAttr.off = Off;
					Off = Off + (attrIr.idtype->size);
				}
			}/*其他情况均为值参，偏移加变量类型的size*/

			/*登记该变量的属性及名字,并返回其类型内部指针*/
			present = Enter(t->name[i], &attrIr, &entry);
			if (present != FALSE)
			{
				ErrorPrompt(t->lineno, t->name[i], (char*)" is defined repetation!\n");
			}
			else
				t->table[i] = entry;

		}
		if (t != NULL)
			t = t->sibling;
	}

	/*如果是主程序，则记录此时偏移，用于目标代码生成时的displayOff*/
	if (Level == 0)
	{
		mainOff = Off;
		/*存储主程序AR的display表的偏移到全局变量*/
		StoreNoff = Off;
	}
	/*如果不是主程序，则记录此时偏移，用于下面填写过程信息表的noff信息*/
	else
		savedOff = Off;

}

/************************************************************/
/* 函数名  procDec                                      */
/* 功  能  该函数处理过程声明的语义分析                     */
/* 说  明  在当前层符号表中填写过程标识符的属性；在新层符号 */
/*         表中填写形参标识符的属性。						*/
/************************************************************/
void  procDecPart(TreeNode* t)
{
	TreeNode* p = t;
	SymbTable* entry = HeadProcess(t);   /*处理过程头*/

	t = t->child[1];
	/*如果过程内部存在声明部分，则处理声明部分*/
	while (t != NULL)
	{
		switch (t->nodekind)
		{
		case  TypeK:     TypeDecPart(t->child[0]);  break;
		case  VarK:     VarDecPart(t->child[0]);   break;

			/*如果声明部分有函数声明，则跳出循环，先填写noff和moff等信息，*
			 *再处理函数声明的循环处理，否则无法保存noff和moff的值。      */
		case  ProcDecK:  break;
		default:
			ErrorPrompt(t->lineno, (char*)"", (char*)"no this node kind in syntax tree!");
			break;
		}
		if (t->nodekind == ProcDecK)
			break;
		else
			t = t->sibling;
	}
	entry->attrIR.More.ProcAttr.nOff = savedOff;
	entry->attrIR.More.ProcAttr.mOff = entry->attrIR.More.ProcAttr.nOff + entry->attrIR.More.ProcAttr.level + 1;
	/*过程活动记录的长度等于nOff加上display表的长度*
	 *diplay表的长度等于过程所在层数加一           */

	 /*处理程序的声明部分*/
	while (t != NULL)
	{
		procDecPart(t);
		t = t->sibling;
	}
	t = p;
	Body(t->child[2]);/*处理Block*/

	/*函数部分结束，删除进入形参时，新建立的符号表*/
	if (Level != -1)
		DestroyTable();/*结束当前scope*/
}


/************************************************************/
/* 函数名  HeadProcess                                      */
/* 功  能  该函数处理函数头的语义分析                       */
/* 说  明  在当前层符号表中填写函数标识符的属性；在新层符号 */
/*         表中填写形参标识符的属性。其中过程的大小和代码都 */
/*         需以后回填。                                     */
/************************************************************/
SymbTable* HeadProcess(TreeNode* t)
{
	AttributeIR attrIr;
	int present = FALSE;
	SymbTable* entry = NULL;

	/*填属性*/
	attrIr.kind = procKind;
	attrIr.idtype = NULL;
	attrIr.More.ProcAttr.level = Level + 1;

	if (t != NULL)
	{
		/*登记函数的符号表项*/
		present = Enter(t->name[0], &attrIr, &entry);
		t->table[0] = entry;
		/*处理形参声明表*/
	}
	entry->attrIR.More.ProcAttr.param = ParaDecList(t);

	return entry;
}

/************************************************************/
/* 函数名  ParaDecList                                      */
/* 功  能  该函数处理函数头中的参数声明的语义分析           */
/* 说  明  在新的符号表中登记所有形参的表项，构造形参表项的 */
/*         地址表，并有para指向其。                         */
/************************************************************/
ParamTable* ParaDecList(TreeNode* t)
{
	TreeNode* p = NULL;
	ParamTable* Ptr1 = NULL;
	ParamTable* Ptr2 = NULL;
	ParamTable* head = NULL;

	if (t != NULL)
	{
		if (t->child[0] != NULL)
			p = t->child[0];   	/*程序声明节点的第一个儿子节点*/

		CreatTable();			/*进入新的局部化区*/

		Off = 7;                /*子程序中的变量初始偏移设为8*/

		VarDecPart(p);			/*变量声明部分*/

		SymbTable* Ptr0 = scope[Level];

		while (Ptr0 != NULL)         /*只要不为空，就访问其兄弟节点*/
		{
			/*构造形参符号表，并使其连接至符号表的param项*/
			Ptr2 = NewParam();
			if (head == NULL)
				head = Ptr1 = Ptr2;
			//Ptr0->attrIR.More.VarAttr.isParam = true;
			Ptr2->entry = Ptr0;
			Ptr2->next = NULL;

			if (Ptr2 != Ptr1)
			{
				Ptr1->next = Ptr2;
				Ptr1 = Ptr2;
			}
			Ptr0 = Ptr0->next;
		}
	}
	return head;   /*返回形参符号表的头指针*/
}


/*******************执行体部分的语义分析*********************/

/************************************************************/
/* 函数名  Body                                             */
/* 功  能  该函数处理执行体部分的语义分析                   */
/* 说  明  TINY编译系统的执行体部分即为语句序列，故只需处理 */
/*         语句序列部分。                                   */
/************************************************************/
void Body(TreeNode* t)
{
	if (t->nodekind == StmLK)
	{
		TreeNode* p = t->child[0];
		while (p != NULL)
		{
			statement(p);  /*调用语句状态处理函数*/
			p = p->sibling;   /*依次读入语法树语句序列的兄弟节点*/
		}
	}
}


/************************************************************/
/* 函数名  statement                                        */
/* 功  能  该函数处理语句状态                               */
/* 说  明  根据语法树节点中的kind项判断应该转向处理哪个语句 */
/*         类型函数。                                       */
/************************************************************/
void statement(TreeNode* t)
{
	switch (t->kind.stmt)
	{
	case IfK:			ifstatment(t); break;
	case WhileK:		whilestatement(t); break;
	case AssignK:		assignstatement(t); break;
	case ReadK:		    readstatement(t); break;
	case WriteK:		writestatement(t); break;
	case CallK:		    callstatement(t); break;
	case ReturnK:		returnstatement(t); break;
	default:
		ErrorPrompt(t->lineno, (char*)"", (char*)"statement type error!\n");
		break;
	}
}

/************************************************************/
/* 函数名  Expr                                             */
/* 功  能  该函数处理表达式的分析                           */
/* 说  明  表达式语义分析的重点是检查运算分量的类型相容性， */
/*         求表达式的类型。其中参数Ekind用来表示实参是变参  */
/*         还是值参。    	                                */
/************************************************************/
TypeIR* Expr(TreeNode* t, AccessKind* Ekind)
{
	int present = FALSE;
	SymbTable* entry = NULL;

	TypeIR* Eptr0 = NULL;
	TypeIR* Eptr1 = NULL;
	TypeIR* Eptr = NULL;
	if (t != NULL)
		switch (t->kind.exp)
		{
		case ConstK:
			Eptr = TypeProcess(t, IntegerK);
			Eptr->kind = intTy;
			if (Ekind != NULL)
				(*Ekind) = dir;   /*直接变量*/
			break;
		case VariK:
			/*Var = id的情形*/
			if (t->child[0] == NULL)
			{
				/*在符号表中查找此标识符*/
				present = FindEntry(t->name[0], &entry);
				t->table[0] = entry;

				if (present != FALSE)
				{   /*id不是变量*/
					if (FindAttr(entry).kind != varKind)
					{
						ErrorPrompt(t->lineno, t->name[0], (char*)"is not variable error!\n");
						Eptr = NULL;
					}
					else
					{
						Eptr = entry->attrIR.idtype;
						if (Ekind != NULL)
							(*Ekind) = indir;  /*间接变量*/

					}
				}
				else /*标识符无声明*/
				{
					ErrorPrompt(t->lineno, t->name[0], (char*)"is not declarations!\n");
				}

			}
			else/*Var = Var0[E]的情形*/
			{
				if (t->attr.ExpAttr.varkind == ArrayMembV)
					Eptr = arrayVar(t);
				else /*Var = Var0.id的情形*/
					if (t->attr.ExpAttr.varkind == FieldMembV)
						Eptr = recordVar(t);
			}
			break;
		case OpK:
			/*递归调用儿子节点*/
			Eptr0 = Expr(t->child[0], NULL);
			if (Eptr0 == NULL)
				return NULL;
			Eptr1 = Expr(t->child[1], NULL);
			if (Eptr1 == NULL)
				return NULL;

			/*类型判别*/
			present = Compat(Eptr0, Eptr1);
			if (present != FALSE)
			{
				switch (t->attr.ExpAttr.op)
				{
				case LT:
				case EQ:
					Eptr = boolPtr;
					break;  /*条件表达式*/
				case PLUS:
				case MINUS:
				case TIMES:
				case OVER:
					Eptr = intPtr;
					break;  /*算数表达式*/
				}
				if (Ekind != NULL)
					(*Ekind) = dir; /*直接变量*/
			}
			else
				ErrorPrompt(t->lineno, (char*)"", (char*)"operator is not compat!\n");
			break;
		}
	return Eptr;
}


/************************************************************/
/* 函数名  arrayVar                                         */
/* 功  能  该函数处理数组变量的下标分析                     */
/* 说  明  检查var := var0[E]中var0是不是数组类型变量，E是不*/
/*         是和数组的下标变量类型匹配。                     */
/************************************************************/
TypeIR* arrayVar(TreeNode* t)
{
	int present = FALSE;
	SymbTable* entry = NULL;

	TypeIR* Eptr0 = NULL;
	TypeIR* Eptr1 = NULL;
	TypeIR* Eptr = NULL;


	/*在符号表中查找此标识符*/

	present = FindEntry(t->name[0], &entry);
	t->table[0] = entry;
	/*找到*/
	if (present != FALSE)
	{
		/*Var0不是变量*/
		if (FindAttr(entry).kind != varKind)
		{
			ErrorPrompt(t->lineno, t->name[0], (char*)"is not variable error!\n");
			Eptr = NULL;
		}
		else/*Var0不是数组类型变量*/
			if (FindAttr(entry).idtype != NULL)
				if (FindAttr(entry).idtype->kind != arrayTy)
				{
					ErrorPrompt(t->lineno, t->name[0], (char*)"is not array variable error !\n");
					Eptr = NULL;
				}
				else
				{
					/*检查E的类型是否与下标类型相符*/
					Eptr0 = entry->attrIR.idtype->More.ArrayAttr.indexTy;
					if (Eptr0 == NULL)
						return NULL;
					Eptr1 = Expr(t->child[0], NULL);//intPtr;
					if (Eptr1 == NULL)
						return NULL;
					present = Compat(Eptr0, Eptr1);
					if (present != TRUE)
					{
						ErrorPrompt(t->lineno, (char*)"", (char*)"type is not matched with the array member error !\n");
						Eptr = NULL;
					}
					else
						Eptr = entry->attrIR.idtype->More.ArrayAttr.elemTy;
				}
	}
	else/*标识符无声明*/
		ErrorPrompt(t->lineno, t->name[0], (char*)"is not declarations!\n");
	return Eptr;
}


/************************************************************/
/* 函数名  recordVar                                        */
/* 功  能  该函数处理记录变量中域的分析                     */
/* 说  明  检查var:=var0.id中的var0是不是记录类型变量，id是 */
/*         不是该记录类型中的域成员。                       */
/************************************************************/
TypeIR* recordVar(TreeNode* t)
{
	int present = FALSE;
	int result = TRUE;
	SymbTable* entry = NULL;

	TypeIR* Eptr0 = NULL;
	TypeIR* Eptr1 = NULL;
	TypeIR* Eptr = NULL;
	fieldchain* currentP = NULL;


	/*在符号表中查找此标识符*/
	present = FindEntry(t->name[0], &entry);
	t->table[0] = entry;
	/*找到*/
	if (present != FALSE)
	{
		/*Var0不是变量*/
		if (FindAttr(entry).kind != varKind)
		{
			ErrorPrompt(t->lineno, t->name[0], (char*)"is not variable error!\n");
			Eptr = NULL;
		}
		else/*Var0不是记录类型变量*/
			if (FindAttr(entry).idtype->kind != recordTy)
			{
				ErrorPrompt(t->lineno, t->name[0], (char*)"is not record variable error !\n");
				Eptr = NULL;
			}
			else/*检查id是否是合法域名*/
			{
				Eptr0 = entry->attrIR.idtype;
				currentP = Eptr0->More.body;
				while ((currentP != NULL) && (result != FALSE))
				{
					result = strcmp(t->child[0]->name[0], currentP->id);
					/*如果相等*/
					if (result == FALSE)
						Eptr = currentP->UnitType;
					else
						currentP = currentP->Next;
				}
				if (currentP == NULL)
					if (result != FALSE)
					{
						ErrorPrompt(t->child[0]->lineno, t->child[0]->name[0],
							(char*)"is not field type!\n");
						Eptr = NULL;
					}
					else/*如果id是数组变量*/
						if (t->child[0]->child[0] != NULL)
							Eptr = arrayVar(t->child[0]);
			}
	}
	else/*标识符无声明*/
		ErrorPrompt(t->lineno, t->name[0], (char*)"is not declarations!\n");
	return Eptr;
}


/************************************************************/
/* 函数名  assignstatement                                  */
/* 功  能  该函数处理赋值语句分析                           */
/* 说  明  赋值语句的语义分析的重点是检查赋值号两端分量的类 */
/*         型相容性。    	                                */
/************************************************************/
void assignstatement(TreeNode* t)
{
	SymbTable* entry = NULL;

	int present = FALSE;
	TypeIR* ptr = NULL;
	TypeIR* Eptr = NULL;

	TreeNode* child1 = NULL;
	TreeNode* child2 = NULL;

	child1 = t->child[0];
	child2 = t->child[1];

	if (child1->child[0] == NULL)
	{
		/*在符号表中查找此标识符*/
		present = FindEntry(child1->name[0], &entry);

		if (present != FALSE)
		{   /*id不是变量*/
			if (FindAttr(entry).kind != varKind)
			{
				ErrorPrompt(child1->lineno, child1->name[0], (char*)"is not variable error!\n");
				Eptr = NULL;
			}
			else
			{
				Eptr = entry->attrIR.idtype;
				child1->table[0] = entry;
			}
		}
		else /*标识符无声明*/
			ErrorPrompt(child1->lineno, child1->name[0], (char*)"is not declarations!\n");
	}
	else/*Var0[E]的情形*/
	{
		if (child1->attr.ExpAttr.varkind == ArrayMembV)
			Eptr = arrayVar(child1);
		else /*Var0.id的情形*/
			if (child1->attr.ExpAttr.varkind == FieldMembV)
				Eptr = recordVar(child1);
	}
	if (Eptr != NULL)
	{
		if ((t->nodekind == StmtK) && (t->kind.stmt == AssignK))
		{
			/*检查是不是赋值号两侧 类型等价*/
			ptr = Expr(child2, NULL);
			if (!Compat(ptr, Eptr))
				ErrorPrompt(t->lineno, (char*)"", (char*)"ass_expression error!\n");
		}
		/*赋值语句中不能出现函数调用*/
	}
}

/************************************************************/
/* 函数名  callstatement                                    */
/* 功  能  该函数处理函数调用语句分析                       */
/* 说  明  函数调用语句的语义分析首先检查符号表求出其属性中 */
/*         的Param部分（形参符号表项地址表），并用它检查形参*/
/*         和实参之间的对应关系是否正确。                   */
/************************************************************/
void callstatement(TreeNode* t)
{
	AccessKind  Ekind;
	int present = FALSE;
	SymbTable* entry = NULL;
	TreeNode* p = NULL;

	/*用id检查整个符号表*/
	present = FindEntry(t->child[0]->name[0], &entry);
	t->child[0]->table[0] = entry;

	/*未查到表示函数无声明*/
	if (present == FALSE)
	{
		ErrorPrompt(t->lineno, t->child[0]->name[0], (char*)"function is not declarationed!\n");
	}
	else
		/*id不是函数名*/
		if (FindAttr(entry).kind != procKind)
			ErrorPrompt(t->lineno, t->name[0], (char*)"is not function name!\n");
		else/*形实参匹配*/
		{
			p = t->child[1];
			/*paramP指向形参符号表的表头*/
			ParamTable* paramP = FindAttr(entry).More.ProcAttr.param;
			while ((p != NULL) && (paramP != NULL))
			{
				SymbTable* paraEntry = paramP->entry;
				TypeIR* Etp = Expr(p, &Ekind);/*实参*/
				/*参数类别不匹配*/
				if ((FindAttr(paraEntry).More.VarAttr.access == indir) && (Ekind == dir))
					ErrorPrompt(p->lineno, (char*)"", (char*)"param kind is not match!\n");
				else
					/*参数类型不匹配*/
					if ((FindAttr(paraEntry).idtype) != Etp)
						ErrorPrompt(p->lineno, (char*)"", (char*)"param type is not match!\n");
				p = p->sibling;
				paramP = paramP->next;
			}
			/*参数个数不匹配*/
			if ((p != NULL) || (paramP != NULL))
				ErrorPrompt(t->child[1]->lineno, (char*)"", (char*)"param num is not match!\n");
		}
}


/************************************************************/
/* 函数名  ifstatement                                      */
/* 功  能  该函数处理条件语句分析                           */
/* 说  明  分析语法树的三个儿子节点                         */
/************************************************************/
void ifstatment(TreeNode* t)
{
	AccessKind* Ekind = NULL;
	TypeIR* Etp = Expr(t->child[0], Ekind);
	if (Etp != NULL)
		/*处理条件表达式*/
		if (Etp->kind != boolTy)
			ErrorPrompt(t->lineno, (char*)"", (char*)"condition expressrion error!\n");  /*逻辑表达式错误*/
		else
		{
			TreeNode* p = t->child[1];
			/*处理then语句序列部分*/
			while (p != NULL)
			{
				statement(p);
				p = p->sibling;
			}
			t = t->child[2];		/*必有三儿子*/
			/*处理else语句不分*/
			while (t != NULL)
			{
				statement(t);
				t = t->sibling;
			}
		}
}


/************************************************************/
/* 函数名  whilestatement                                   */
/* 功  能  该函数处理循环语句分析                           */
/* 说  明  分析语法树的两个儿子节点                         */
/************************************************************/
void whilestatement(TreeNode* t)
{
	TypeIR* Etp = Expr(t->child[0], NULL);
	if (Etp != NULL)
		/*处理条件表达式部分*/
		if (Etp->kind != boolTy)
			ErrorPrompt(t->lineno, (char*)"", (char*)"condition expression error!\n");  /*逻辑表达式错误*/
		else
		{
			t = t->child[1];
			/*处理循环部分*/
			while (t != NULL)
			{
				statement(t);
				t = t->sibling;
			}
		}
}


/************************************************************/
/* 函数名  readstatement                                    */
/* 功  能  该函数处理输入语句分析                           */
/* 说  明  分析语法树节点，检查变量有无声明和是否为变量错误 */
/************************************************************/
void readstatement(TreeNode* t)
{
	SymbTable* entry = NULL;
	int present = FALSE;

	/*用id检查整个符号表*/
	present = FindEntry(t->name[0], &entry);
	t->table[0] = entry;

	/*未查到表示变量无声明*/
	if (present == FALSE)
		ErrorPrompt(t->lineno, t->name[0], (char*)" is not declarationed!\n");
	else
		/*不是变量标识符错误*/
		if (entry->attrIR.kind != varKind)
			ErrorPrompt(t->lineno, t->name[0], (char*)"is not var name!\n ");
}

/************************************************************/
/* 函数名  writestatement                                   */
/* 功  能  该函数处理输出语句分析                           */
/* 说  明  分析输出语句中的表达式是否合法                   */
/************************************************************/
void writestatement(TreeNode* t)
{
	TypeIR* Etp = Expr(t->child[0], NULL);
	if (Etp != NULL)
		/*如果表达式类型为bool类型，报错*/
		if (Etp->kind == boolTy)
			ErrorPrompt(t->lineno, (char*)"", (char*)"exprssion type error!");
}


/************************************************************/
/* 函数名  returnstatement                                  */
/* 功  能  该函数处理函数返回语句分析                       */
/* 说  明  分析函数返回语句是否在主程序中出现               */
/************************************************************/
void returnstatement(TreeNode* t)
{
	if (Level == 0)
		/*如果返回语句出现在主程序中，报错*/
		ErrorPrompt(t->lineno, (char*)"", (char*)"return statement error!");
}

/************************************************************/
/* 函数名  analyze                                          */
/* 功  能  该函数处理总的语义分析                           */
/* 说  明  对语法树进行分析                                 */
/************************************************************/
void analyze(TreeNode* t)
{
	SymbTable* entry = NULL;
	TreeNode* p = NULL;
	TreeNode* pp = t;

	/*创建符号表*/
	CreatTable();

	/*调用类型内部表示初始化函数*/
	initialize();

	/*语法树的声明节点*/
	p = t->child[1];
	while (p != NULL)
	{
		switch (p->nodekind)
		{
		case  TypeK:     TypeDecPart(p->child[0]);  break;
		case  VarK:     VarDecPart(p->child[0]);   break;
		case  ProcDecK:  procDecPart(p);        break;
		default:
			ErrorPrompt(p->lineno, (char*)"", (char*)"no this node kind in syntax tree!");
			break;
		}
		p = p->sibling;/*循环处理*/
	}

	/*程序体*/
	t = t->child[2];
	if (t->nodekind == StmLK)
		Body(t);

	/*撤销符号表*/
	if (Level != -1)
		DestroyTable();

	/*输出语义错误*/
	if (Error == TRUE)
		fprintf(listing, "\n语义错误:\n");
	/*如果无错误，则输出提示信息*/
//	else
//		fprintf(listing, "\n........ no error!\n");
}











