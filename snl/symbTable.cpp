/****************************************************/
/* �ļ� symbTable.cpp       	  					*/
/* ˵�� ��PASCAL���Ա��������ű������  			*/
/* ���� �������ṹ:ԭ���ʵ��						*/
/****************************************************/

/* ͷ�ļ�globals.h������ȫ����������� */
#include "globals.h"

#include "stdio.h"

#include "string.h"

#include "util.h"



static void printTy(TypeIR* ty);

static void printVar(SymbTable* entry);

static void printProc(SymbTable* entry);

//static void printTab(int tabnum);


/**********************************************************/
/****************   ���ű���ز���   **********************/
/**********************************************************/


/********************************************************/
/* ������  PrintFieldTable								*/
/* ��  ��  ��ӡ��¼���͵����							*/
/* ˵  ��										        */
/********************************************************/
void   PrintFieldChain(fieldChain* currentP)
{
	fprintf(listing, "\n--------------Field  chain--------------------\n");
	fieldChain* t = currentP;
	while (t != NULL)
	{ /*�����ʶ������*/
		fprintf(listing, "%s:  ", t->id);
		/*�����ʶ����������Ϣ*/

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
/* ������  PrintOneLayer								*/
/* ��  ��  ��ӡ���ű��һ��								*/
/* ˵  ��  �з��ű��ӡ����PrintSymbTable����	        */
/********************************************************/
void  PrintOneLayer(int level)
{
	SymbTable* t = scope[level];
	fprintf(listing, "\n--level:%d --------------------------------\n", level);

	while (t != NULL)
	{ /*�����ʶ������*/
		fprintf(listing, "%s:   ", t->idName);
		AttributeIR* Attrib = &(t->attrIR);
		/*�����ʶ����������Ϣ�����̱�ʶ������*/
		if (Attrib->idtype != NULL)  /*���̱�ʶ��*/
			switch (Attrib->idtype->kind)
			{
			case  intTy:  fprintf(listing, "intTy  ");   break;
			case  charTy:	fprintf(listing, "charTy  ");  break;
			case  arrayTy: fprintf(listing, "arrayTy  "); break;
			case  recordTy:fprintf(listing, "recordTy  "); break;
			default: fprintf(listing, "error  type!  "); break;
			}
		/*�����ʶ������𣬲����ݲ�ͬ���������ͬ��������*/
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
/* ������  PrintSymbTable								*/
/* ��  ��  ��ӡ���ɵķ��ű�								*/
/* ˵  ��										        */
/********************************************************/
void   PrintSymbTable()
{ /*������0��ʼ*/
	int  level = 0;
	while (scope[level] != NULL)
	{
		PrintOneLayer(level);
		level++;
	}
}















/***********************************************************/
/* ������ NewTable                                         */
/* ��  �� ������ǰ�շ��ű�                                 */
/* ˵  �� �����µ��������ı�ʶ��ʱ�����µĿշ��ű������� */
/*		  ָ������ָ��							           */
/***********************************************************/
SymbTable* NewTable(void)
{
	/* �ڴ��ж�̬������䵥Ԫ������ָ��õ�Ԫ�ķ��ű�����ָ��t */
	SymbTable* table = (SymbTable*)malloc(sizeof(SymbTable));

	/* ���ű�����ָ��tableΪNULL,δ�ܳɹ������ڴ浥Ԫ	*
	 * ��������Ϣ���к�linenoд���б��ļ�listing		*/
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



	/* ���ű�����ָ��table����NULL,�ڴ浥Ԫ�Ѿ��ɹ����� */
	return table;
}

/**********************************************************/
/*������ CreatTable                                       */
/*��  �� �����շ��ű�                                     */
/*˵  �� ������һ���µľֲ�����λʱ�����ñ��ӳ��򡣹�����:*/
/*		 ����һ���շ��ű�table��������1��ƫ�Ƴ�ʼ��Ϊ0��  */
/**********************************************************/
void CreatTable(void)
{

	Level = Level + 1;                 /*������һ*/
	scope[Level] = NULL;	          /*�������µ�һ��scopeջ�Ŀռ�*/
	Off = INITOFF;			          /*ƫ�Ƴ�ʼ��*/
}

//void printTable();
/***********************************************************/
/* ������ DestroyTable                                     */
/* ��  �� ������ǰ���ű�                                   */
/* ˵  �� �˳�һ���ֲ�����ʱ�����ñ��ӳ��򡣹����ǲ�����1��*/
/*		  ��������ǰ���ű�                                 */
/***********************************************************/
void  DestroyTable()
{
	/*�������������ٱ�־ΪTURE�����﷨���������ķ��ű���ʾ����*/
	/*if ((TraceTable)&&(Error==FALSE))
	{
		printTable();
		getchar();
	}*/
	Level = Level - 1;
}

/***********************************************************/
/* ������ Enter                                            */
/* ��  �� �ǼǱ�ʶ��������                                 */
/* ˵  �� Enter��������һ����ʶ��id��һ������attrib�Լ�    */
/*		  ���ű�ָ��entry������ɵ������ǰѸ���id������    */
/*        Atrrib�Ǽǵ����ű��У������صǼ���ĵ�ַ���ڵ�   */
/*		  ��ʱӦ����ڱ������Ƿ����ظ���������Ϊ������   */
/*        Enter��������Ϊbool���������id����ñ�������1   */
/*        ֵ�����򷵻�0��                                  */
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
		}   /*�ڸò���ű��ڼ���Ƿ����ظ��������*/

		if (present == FALSE)
		{
			curentry = NewTable();
			prentry->next = curentry;
		}
	}

	/*����ʶ���������ԵǼǵ�����*/
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
/* ������ FindEntry                                        */
/* ��  �� Ѱ�ұ����ַ                                     */
/* ˵  �� �Ը����ı�ʶ��id (idΪ�ַ�������) ���������ַ,*/
/*        ����entry��ʵ�ε�Ԫ�з��ر����ַ��������ű���û*/
/*		  �����ҵ�id��,�򷵻�presentΪ0,�����еĲ���entry*/
/*        ��ֵΪָ��ñ����ַ��ָ��;����,present��ֵΪ1�� */
/***********************************************************/
int FindEntry(char* id, SymbTable** entry)
{
	int present = FALSE;    /*����ֵ*/
	int result = 1;         /*��ʶ�����ֱȽϽ��*/
	int lev = Level;		/*��ʱ��¼�����ı���*/

	SymbTable* findentry = scope[lev];

	while ((lev != -1) && (present != TRUE))
	{
		while ((findentry != NULL) && (present != TRUE))
		{
			result = strcmp(id, findentry->idName);
			if (result == 0)
				present = TRUE;
			/*�����ʶ��������ͬ���򷵻�TRUE*/
			else
				findentry = findentry->next;
			/*���û�ҵ�������������еĲ���*/
		}
		if (present != TRUE)
		{
			lev = lev - 1;
			findentry = scope[lev];

		}
	}/*����ڱ�����û�в鵽����ת����һ���ֲ��������м�������*/
	if (present != TRUE)
	{
		(*entry) = NULL;
	}
	else
		(*entry) = findentry;

	return present;
}


/***********************************************************/
/* ������ FindAtrr                                         */
/* ��  �� ���Բ�ѯ                                         */
/* ˵  �� �Ը��������ַ�����������ֵ�������䷵�ظ�Atrrib */
/*        ��ʵ�ε�Ԫ�С�                                   */
/***********************************************************/
AttributeIR FindAttr(SymbTable* entry)
{
	AttributeIR attrIr = entry->attrIR;
	return attrIr;
}


/***********************************************************/
/* ������ Compat                                           */
/* ��  �� �ж������Ƿ�����                                 */
/* ˵  �� ����TINY������ֻ���������͡��ַ����͡��������ͺ� */
/*        ��¼���ͣ����������ݵ������͵ȼۣ�ֻ���ж�ÿ���� */
/*        �����͵��ڲ���ʾ������ָ��ֵ�Ƿ���ͬ���ɡ�       */
/***********************************************************/
int   Compat(TypeIR* tp1, TypeIR* tp2)
{
	int  present;
	if (tp1 != tp2)
		present = FALSE;  /*���Ͳ���*/
	else
		present = TRUE;   /*���͵ȼ�*/
	return present;
}

/***********************************************************/
/* ������ NewTy	                                           */
/* ��  �� ������ǰ�������ڲ���ʾ                           */
/* ˵  �� ����Ϊ���ͣ��������ظ����͵��ڲ���ʾ�ĵ�ַ       */
/***********************************************************/
TypeIR* NewTy(TypeKind  kind)
{
	/* �ڴ��ж�̬������䵥Ԫ��
	   ����ָ��õ�Ԫ�������ڲ���ʾ����ָ��t */
	TypeIR* table = (TypeIR*)malloc(sizeof(TypeIR));

	/* �����ڲ���ʾ����ָ��tableΪNULL,
	   δ�ܳɹ������ڴ浥Ԫ����ʾ��ʾ������Ϣ*/
	if (table == NULL)
	{
		fprintf(listing, "Out of memory error !");
		Error = TRUE;
	}
	/* �����ڲ���ʾ����ָ��table����NULL,�ڴ浥Ԫ�Ѿ��ɹ����� */
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
/* ������ NewBody                                          */
/* ��  �� ������ǰ�ռ�¼�������������                     */
/* ˵  �� �������ظ����͵��µ�����ĵ�Ԫ��ַ               */
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
/* ������ NewParam	                                       */
/* ��  �� ������ǰ���β�����                               */
/* ˵  �� ��������������ĵ�Ԫ��ַ                         */
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
/* ������ ErrorPrompt                                      */
/* ��  �� ������ʾ                                         */
/* ˵  �� ������ļ�����ʾ������ʾ������ȫ����Error��ֵΪ1 */
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
/* ������ printTab                                         */
/* ��  �� ��ӡ�ո�                                         */
/* ˵  �� ������ļ��д�ӡ����Ϊ����tabnum�Ŀո�           */
/***********************************************************/
void printTab(int tabnum)
{
	for (int i = 0; i < tabnum; i++)
		fprintf(listing, " ");
}


/***********************************************************/
/* ������  printTable                                      */
/* ��  ��  �ѷ��ű�������ļ�����ʾ����                    */
/* ˵  ��  �ֲ���ʾ���ű����ݣ�����������ʾ�÷��ŵ������ڲ�*/
/*         ��ʾ                                            */
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
/* ������  printTy                                         */
/* ��  ��  �������ڲ���ʾ������ļ�����ʾ����              */
/* ˵  ��  ��ʾ������������С�������������Ϣ              */
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
/* ������  printVar                                        */
/* ��  ��  �ѱ����ڲ���ʾ������ļ�����ʾ����              */
/* ˵  ��  ��ʾ��������ƣ�ƫ�ƣ�������������              */
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
/* ������  printProc                                       */
/* ��  ��  �ѹ����ڲ���ʾ������ļ�����ʾ����              */
/* ˵  ��  ��ʾ������������                                */
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
/* ������  FindField	  								*/
/* ��  ��  ���Ҽ�¼������								*/
/* ˵  ��  ����ֵΪ�Ƿ��ҵ���־������Entry���ش�������  */
/*		   ��¼������е�λ��.							*/
/********************************************************/
bool  FindField(char* Id, fieldChain* head, fieldChain** Entry)
{
	bool  present = false;
	/*��¼��ǰ�ڵ�*/
	fieldChain* currentItem = head;
	/*�ӱ�ͷ��ʼ���������ʶ����ֱ���ҵ��򵽴��β*/
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
