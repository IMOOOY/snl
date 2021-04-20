/****************************************************/
/* 文件 symbTable.cpp       	  					*/
/* 说明 类PASCAL语言编译器符号表处理程序  			*/
/* 主题 编译器结构:原理和实例						*/
/****************************************************/

/* 头文件globals.h定义了全局类型与变量 */
#include "globals.h"

#include "stdio.h"

#include "string.h"

#include "util.h"



static void printTy(TypeIR* ty);

static void printVar(SymbTable* entry);

static void printProc(SymbTable* entry);

//static void printTab(int tabnum);


/**********************************************************/
/****************   符号表相关操作   **********************/
/**********************************************************/


/********************************************************/
/* 函数名  PrintFieldTable								*/
/* 功  能  打印纪录类型的域表							*/
/* 说  明										        */
/********************************************************/
void   PrintFieldChain(fieldChain* currentP)
{
	fprintf(listing, "\n--------------Field  chain--------------------\n");
	fieldChain* t = currentP;
	while (t != NULL)
	{ /*输出标识符名字*/
		fprintf(listing, "%s:  ", t->id);
		/*输出标识符的类型信息*/

		switch (t->UnitType->kind)
		{
		case  intTy:  fprintf(listing, "intTy     ");   break;
		case  charTy:	fprintf(listing, "charTy    ");  break;
		case  arrayTy: fprintf(listing, "arrayTy   "); break;
		case  recordTy:fprintf(listing, "recordTy  "); break;
		default: fprintf(listing, "error  type!  "); break;
		}
		fprintf(listing, "off = %d\n", t->off);

		t = t->Next;
	}
}
/********************************************************/
/* 函数名  PrintOneLayer								*/
/* 功  能  打印符号表的一层								*/
/* 说  明  有符号表打印函数PrintSymbTable调用	        */
/********************************************************/
void  PrintOneLayer(int level)
{
	SymbTable* t = scope[level];
	fprintf(listing, "\n--level:%d --------------------------------\n", level);

	while (t != NULL)
	{ /*输出标识符名字*/
		fprintf(listing, "%s:   ", t->idName);
		AttributeIR* Attrib = &(t->attrIR);
		/*输出标识符的类型信息，过程标识符除外*/
		if (Attrib->idtype != NULL)  /*过程标识符*/
			switch (Attrib->idtype->kind)
			{
			case  intTy:  fprintf(listing, "intTy  ");   break;
			case  charTy:	fprintf(listing, "charTy  ");  break;
			case  arrayTy: fprintf(listing, "arrayTy  "); break;
			case  recordTy:fprintf(listing, "recordTy  "); break;
			default: fprintf(listing, "error  type!  "); break;
			}
		/*输出标识符的类别，并根据不同类型输出不同其它属性*/
		switch (Attrib->kind)
		{
		case  typeKind:
			fprintf(listing, "typekind  "); break;
		case  varKind:
			fprintf(listing, "varkind  ");
			fprintf(listing, "Level = %d  ", Attrib->More.VarAttr.level);
			fprintf(listing, "Offset= %d  ", Attrib->More.VarAttr.off);

			switch (Attrib->More.VarAttr.access)
			{
			case  dir:  fprintf(listing, "dir  "); break;
			case  indir: fprintf(listing, "indir  "); break;
			default:fprintf(listing, "errorkind  ");  break;
			}
			break;
		case  procKind:
			fprintf(listing, "funckind   ");
			fprintf(listing, "Level= %d  ", Attrib->More.ProcAttr.level);
			fprintf(listing, "Noff= %d  ", Attrib->More.ProcAttr.nOff);
			break;
		default:fprintf(listing, "error  ");
		}
		fprintf(listing, "\n");
		t = t->next;
	}
}






/********************************************************/
/* 函数名  PrintSymbTable								*/
/* 功  能  打印生成的符号表								*/
/* 说  明										        */
/********************************************************/
void   PrintSymbTable()
{ /*层数从0开始*/
	int  level = 0;
	while (scope[level] != NULL)
	{
		PrintOneLayer(level);
		level++;
	}
}















/***********************************************************/
/* 函数名 NewTable                                         */
/* 功  能 创建当前空符号表                                 */
/* 说  明 遇到新的无声明的标识符时创建新的空符号表，并返回 */
/*		  指向它的指针							           */
/***********************************************************/
SymbTable* NewTable(void)
{
	/* 内存中动态申请分配单元，返回指向该单元的符号表类型指针t */
	SymbTable* table = (SymbTable*)malloc(sizeof(SymbTable));

	/* 符号表类型指针table为NULL,未能成功分配内存单元	*
	 * 将出错信息及行号lineno写入列表文件listing		*/
	if (table == NULL)
	{
		fprintf(listing, "Out of memory error !");
		Error = TRUE;
	}
	table->next = NULL;

	table->attrIR.kind = typeKind;
	table->attrIR.idtype = NULL;
	table->next = NULL;
	table->attrIR.More.VarAttr.isParam = false;



	/* 符号表类型指针table不是NULL,内存单元已经成功分配 */
	return table;
}

/**********************************************************/
/*函数名 CreatTable                                       */
/*功  能 创建空符号表                                     */
/*说  明 当进入一个新的局部化单位时，调用本子程序。功能是:*/
/*		 建立一个空符号表table，层数加1，偏移初始化为0。  */
/**********************************************************/
void CreatTable(void)
{

	Level = Level + 1;                 /*层数加一*/
	scope[Level] = NULL;	          /*申请了新的一层scope栈的空间*/
	Off = INITOFF;			          /*偏移初始化*/
}

//void printTable();
/***********************************************************/
/* 函数名 DestroyTable                                     */
/* 功  能 撤销当前符号表                                   */
/* 说  明 退出一个局部化区时，调用本子程序。功能是层数减1，*/
/*		  并撤销当前符号表                                 */
/***********************************************************/
void  DestroyTable()
{
	/*如果语义分析跟踪标志为TURE，则将语法分析产生的符号表显示出来*/
	/*if ((TraceTable)&&(Error==FALSE))
	{
		printTable();
		getchar();
	}*/
	Level = Level - 1;
}

/***********************************************************/
/* 函数名 Enter                                            */
/* 功  能 登记标识符和属性                                 */
/* 说  明 Enter的输入是一个标识符id和一个属性attrib以及    */
/*		  符号表指针entry，而完成的任务是把给定id和属性    */
/*        Atrrib登记到符号表中，并返回登记项的地址。在登   */
/*		  记时应检查在本层中是否有重复声明错误，为此声明   */
/*        Enter返回类型为bool，如果已有id项则该变量返回1   */
/*        值，否则返回0。                                  */
/***********************************************************/
int  Enter(char* id, AttributeIR* attribP, SymbTable** entry)
{
	int present = FALSE;
	int result = 1;
	SymbTable* curentry = scope[Level];
	SymbTable* prentry = scope[Level];

	if (scope[Level] == NULL)
	{
		curentry = NewTable();
		scope[Level] = curentry;
	}
	else
	{
		while (curentry != NULL)
		{
			prentry = curentry;
			result = strcmp(id, curentry->idName);
			if (result == 0)
			{
				fprintf(listing, "repetition declaration error !");
				Error = TRUE;
				present = TRUE;
			}
			else
				curentry = (prentry->next);
		}   /*在该层符号表内检查是否有重复定义错误*/

		if (present == FALSE)
		{
			curentry = NewTable();
			prentry->next = curentry;
		}
	}

	/*将标识符名和属性登记到表中*/
	strcpy(curentry->idName, id);

	curentry->attrIR.idtype = attribP->idtype;
	curentry->attrIR.kind = attribP->kind;
	switch (attribP->kind)
	{
	case  typeKind: break;
	case  varKind:
		curentry->attrIR.More.VarAttr.level = attribP->More.VarAttr.level;
		curentry->attrIR.More.VarAttr.off = attribP->More.VarAttr.off;
		curentry->attrIR.More.VarAttr.access = attribP->More.VarAttr.access;
		break;
	case  procKind:
		curentry->attrIR.More.ProcAttr.level = attribP->More.ProcAttr.level;
		curentry->attrIR.More.ProcAttr.param = attribP->More.ProcAttr.param;
		break;
	default:break;
	}
	(*entry) = curentry;

	return present;
}

/***********************************************************/
/* 函数名 FindEntry                                        */
/* 功  能 寻找表项地址                                     */
/* 说  明 对给定的标识符id (id为字符串类型) 求出其表项地址,*/
/*        并在entry的实参单元中返回表项地址。如果符号表里没*/
/*		  有所找的id项,则返回present为0,则函数中的参数entry*/
/*        赋值为指向该表项地址的指针;否则,present赋值为1。 */
/***********************************************************/
int FindEntry(char* id, SymbTable** entry)
{
	int present = FALSE;    /*返回值*/
	int result = 1;         /*标识符名字比较结果*/
	int lev = Level;		/*临时记录层数的变量*/

	SymbTable* findentry = scope[lev];

	while ((lev != -1) && (present != TRUE))
	{
		while ((findentry != NULL) && (present != TRUE))
		{
			result = strcmp(id, findentry->idName);
			if (result == 0)
				present = TRUE;
			/*如果标识符名字相同，则返回TRUE*/
			else
				findentry = findentry->next;
			/*如果没找到，则继续链表中的查找*/
		}
		if (present != TRUE)
		{
			lev = lev - 1;
			findentry = scope[lev];

		}
	}/*如果在本层中没有查到，则转到上一个局部化区域中继续查找*/
	if (present != TRUE)
	{
		(*entry) = NULL;
	}
	else
		(*entry) = findentry;

	return present;
}


/***********************************************************/
/* 函数名 FindAtrr                                         */
/* 功  能 属性查询                                         */
/* 说  明 对给定表项地址，求出其属性值，并将其返回给Atrrib */
/*        的实参单元中。                                   */
/***********************************************************/
AttributeIR FindAttr(SymbTable* entry)
{
	AttributeIR attrIr = entry->attrIR;
	return attrIr;
}


/***********************************************************/
/* 函数名 Compat                                           */
/* 功  能 判断类型是否相容                                 */
/* 说  明 由于TINY语言中只有整数类型、字符类型、数组类型和 */
/*        记录类型，故类型相容等于类型等价，只需判断每个结 */
/*        构类型的内部表示产生的指针值是否相同即可。       */
/***********************************************************/
int   Compat(TypeIR* tp1, TypeIR* tp2)
{
	int  present;
	if (tp1 != tp2)
		present = FALSE;  /*类型不等*/
	else
		present = TRUE;   /*类型等价*/
	return present;
}

/***********************************************************/
/* 函数名 NewTy	                                           */
/* 功  能 创建当前空类型内部表示                           */
/* 说  明 参数为类型，函数返回该类型的内部表示的地址       */
/***********************************************************/
TypeIR* NewTy(TypeKind  kind)
{
	/* 内存中动态申请分配单元，
	   返回指向该单元的类型内部表示类型指针t */
	TypeIR* table = (TypeIR*)malloc(sizeof(TypeIR));

	/* 类型内部表示类型指针table为NULL,
	   未能成功分配内存单元将显示提示错误信息*/
	if (table == NULL)
	{
		fprintf(listing, "Out of memory error !");
		Error = TRUE;
	}
	/* 类型内部表示类型指针table不是NULL,内存单元已经成功分配 */
	else
		switch (kind)
		{
		case intTy:
		case charTy:
		case boolTy:
			table->kind = kind;
			table->size = 1;
			break;
		case arrayTy:
			table->kind = arrayTy;
			table->More.ArrayAttr.indexTy = NULL;
			table->More.ArrayAttr.elemTy = NULL;
			break;
		case recordTy:
			table->kind = recordTy;
			table->More.body = NULL;
			break;
		}
	return table;
}

/***********************************************************/
/* 函数名 NewBody                                          */
/* 功  能 创建当前空记录类型中域的链表                     */
/* 说  明 函数返回该类型的新的链表的单元地址               */
/***********************************************************/
fieldChain* NewBody(void)
{
	fieldChain* Ptr = (fieldChain*)malloc(sizeof(fieldChain));

	if (Ptr == NULL)
	{
		fprintf(listing, "Out of memory error !");
		Error = TRUE;
	}
	else
	{
		Ptr->Next = NULL;
		Ptr->off = 0;
		Ptr->UnitType = NULL;
	}
	return Ptr;
}

/***********************************************************/
/* 函数名 NewParam	                                       */
/* 功  能 创建当前空形参链表                               */
/* 说  明 函数返回新申请的单元地址                         */
/***********************************************************/
ParamTable* NewParam(void)
{
	ParamTable* Ptr = (ParamTable*)malloc(sizeof(ParamTable));

	if (Ptr == NULL)
	{
		fprintf(listing, "Out of memory error !");
		Error = TRUE;
	}
	else
	{
		Ptr->entry = NULL;
		Ptr->next = NULL;
	}

	return Ptr;
}

/***********************************************************/
/* 函数名 ErrorPrompt                                      */
/* 功  能 错误提示                                         */
/* 说  明 在输出文件中显示错误提示，并给全局量Error赋值为1 */
/***********************************************************/
void ErrorPrompt(int line, char* name, char* message)
{
	fprintf(listing, ">>>Line %d:, %s %s", line, name, message);
	Error = TRUE;
    fprintf(listing, "\n\n===================================================================\n");
    fprintf(listing, "Program ends\n");
    fprintf(listing, "===================================================================\n");
	exit(0);
}


/***********************************************************/
/* 函数名 printTab                                         */
/* 功  能 打印空格                                         */
/* 说  明 在输出文件中打印个数为参数tabnum的空格           */
/***********************************************************/
void printTab(int tabnum)
{
	for (int i = 0; i < tabnum; i++)
		fprintf(listing, " ");
}


/***********************************************************/
/* 函数名  printTable                                      */
/* 功  能  把符号表在输出文件中显示出来                    */
/* 说  明  分层显示符号表内容，并在下面显示该符号的类型内部*/
/*         表示                                            */
/***********************************************************/
/*void printTable(void)
{
	SymbTable * table = NULL;
	TypeIR * tp = NULL;

	//fprintf(listing,"\n symble table:\n\n");

	/*while*if (scope[Level]!=NULL)
	{
		fprintf(listing,"\n------  level: %d  ------\n",Level);
		table = scope[Level];
		while (table!=NULL)
		{
			switch(table->attrIR.kind)
			{
			case typeKind:
				fprintf(listing,"typeDec:\n");
				printTab(5);
				fprintf(listing,"name->%s;\n",table->idName);
				printTab(5);
				tp = table->attrIR.idtype;
				if(tp!=NULL)
					printTy(tp);
				else
					fprintf(listing,"type error!\n");
				break;
			case varKind:
				fprintf(listing,"varDec:\n");
				printTab(5);
				printVar(table);
				break;
			case procKind:
				fprintf(listing,"procDec:\n");
				printTab(5);
				printProc(table);
				break;
			}
			table = table->next;
		}
	}
}

*/
/***********************************************************/
/* 函数名  printTy                                         */
/* 功  能  把类型内部表示在输出文件中显示出来              */
/* 说  明  显示其类型名，大小，及其他相关信息              */
/***********************************************************/
/*void printTy(TypeIR * ty)
{
	switch(ty->kind)
	{
	case intTy:
		fprintf(listing,"kind->intTy;\n");
		printTab(5);
		fprintf(listing,"size->%d;\n",ty->size);
		break;
	case charTy:
		fprintf(listing,"kind->charTy;\n");
		printTab(5);
		fprintf(listing,"size->%d;\n",ty->size);
		break;
	case arrayTy:
		fprintf(listing,"kind->arrayTy;\n");
		printTab(5);
		fprintf(listing,"size->%d;\n",ty->size);
		printTab(5);

		if(ty->More.ArrayAttr.indexTy->kind==intTy)
			fprintf(listing,"indextype->intTy;\n");
		else
			fprintf(listing,"indextype->charTy;\n");

		printTab(5);
		if(ty->More.ArrayAttr.elemTy->kind==intTy)
			fprintf(listing,"elemtype->intTy;\n");
		else
			fprintf(listing,"elemtype->charTy;\n");
		break;
	case recordTy:
		fprintf(listing,"kind->recordTy;\n");
		printTab(5);
		fprintf(listing,"size->%d;\n",ty->size);

		fieldChain * tyBody = ty->More.body;
		fprintf(listing,"\n");
		printTab(3);
		fprintf(listing,"field:\n");

		while(tyBody!=NULL)
		{
			printTab(5);
			fprintf(listing,"id->%s;\n",tyBody->id);
			printTab(5);
			fprintf(listing,"unit");
			printTy(tyBody->UnitType);
			printTab(5);
			fprintf(listing,"off->%d;\n",tyBody->off);
			tyBody = tyBody->Next;
			fprintf(listing,"\n");
		}
		break;
	}
}
*/
/***********************************************************/
/* 函数名  printVar                                        */
/* 功  能  把变量内部表示在输出文件中显示出来              */
/* 说  明  显示其变量名称，偏移，类型名及其他              */
/***********************************************************/
/*void printVar(SymbTable * entry)
{
	fprintf(listing,"name->%s;\n",entry->idName);

	AttributeIR  attr = entry->attrIR;
	printTab(5);
	fprintf(listing,"level->%d;\n",attr.More.VarAttr.level);
	printTab(5);
	fprintf(listing,"off->%d;\n",attr.More.VarAttr.off);
	printTab(5);
	if(attr.More.VarAttr.isParam==true)
		fprintf(listing,"param\n");
	else
		fprintf(listing,"not param\n");
	printTab(5);
	if(attr.More.VarAttr.access==dir)
		fprintf(listing,"access->dir;\n");
	else
		fprintf(listing,"access->indir;\n");

	printTab(5);
	fprintf(listing,"\n");

	if(entry->attrIR.idtype!=NULL)
	{
		printTab(5);
		printTy(entry->attrIR.idtype);
	}
}


*/
/***********************************************************/
/* 函数名  printProc                                       */
/* 功  能  把过程内部表示在输出文件中显示出来              */
/* 说  明  显示过程名，层数                                */
/***********************************************************/
/*void printProc(SymbTable * entry)
{

	SymbTable * entry0 = NULL;

	fprintf(listing,"name->%s;\n",entry->idName);

	AttributeIR  attr = entry->attrIR;
	printTab(5);
	fprintf(listing,"level->%d;\n",attr.More.ProcAttr.level);
	ParamTable * ptable = attr.More.ProcAttr.param;
	printTab(5);

	fprintf(listing,"noff->%d; \n",entry->attrIR.More.ProcAttr.nOff);
	printTab(5);
	fprintf(listing,"moff->%d; \n",entry->attrIR.More.ProcAttr.mOff);
}

*/
/********************************************************/
/* 函数名  FindField	  								*/
/* 功  能  查找纪录的域名								*/
/* 说  明  返回值为是否找到标志，变量Entry返回此域名在  */
/*		   纪录的域表中的位置.							*/
/********************************************************/
bool  FindField(char* Id, fieldChain* head, fieldChain** Entry)
{
	bool  present = false;
	/*记录当前节点*/
	fieldChain* currentItem = head;
	/*从表头开始查找这个标识符，直到找到或到达表尾*/
	while ((currentItem != NULL) && (present == false))
	{
		if (strcmp(currentItem->id, Id) == 0)
		{
			present = true;
			if (Entry != NULL)
				(*Entry) = currentItem;
		}
		else  currentItem = currentItem->Next;
	}

	return(present);
}
