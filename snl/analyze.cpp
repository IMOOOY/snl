/****************************************************/
/* �ļ� analyze.cpp				  					*/
/* ˵�� ��PASCAL���Ա����������������  			*/
/* ���� �������ṹ:ԭ���ʵ��						*/
/****************************************************/

/***********  ���ļ���������ͷ�ļ�  ****************/

#include "globals.h"	

#include "util.h"

#include "symbTable.h"		

#include "scanner.h"	

#include "parse.h"

#include "analyze.h"		

#include "string.h"

/*scopeջ�Ĳ���*/
int Level = -1;

/*��ͬ��ı���ƫ��*/
int Off;

/*�������noffƫ��*/
int mainOff;


SymbTable* scope[SCOPESIZE];   /*scopeջ*/

TypeIR* intPtr = NULL;			/*��ָ��һֱָ���������͵��ڲ���ʾ*/

TypeIR* charPtr = NULL;		/*��ָ��һֱָ���ַ����͵��ڲ���ʾ*/

TypeIR* boolPtr = NULL;		/*��ָ��һֱָ�򲼶����͵��ڲ���ʾ*/

TypeIR* arrayVar(TreeNode* t);
/*�ú������ڴ����������ͱ���*/

TypeIR* recordVar(TreeNode* t);
/*�ú������ڴ����¼���ͱ���*/

/***********************���ʹ���*****************************/



/*************************************************************/
/* ������  initialize                                        */
/* ��  ��  �ú�����ʼ���������ͣ��ַ����ͣ��������͵��ڲ���ʾ*/
/* ˵  ��  �������������;�Ϊ�������ͣ��ڲ���ʾ�̶���        */
/*************************************************************/
void initialize(void)
{
	intPtr = NewTy(intTy);

	charPtr = NewTy(charTy);

	boolPtr = NewTy(boolTy);

	/*scopeջ�ĸ���ָ����Ϊ��*/
	for (int i = 0; i < SCOPESIZE; i++)
		scope[i] = NULL;
}

/************************************************************/
/* ������  TypeProcess                                      */
/* ��  ��  �ú�������������ͷ����Ĺ���                     */
/* ˵  ��  �����﷨���ĵ�ǰ������͡��������ǰ���͵��ڲ��� */
/*         ʾ���������ַ���ظ�Ptr�����ڲ���ʾ�ĵ�ַ.       */
/************************************************************/
TypeIR* TypeProcess(TreeNode* t, DecKind deckind)
{
	TypeIR* Ptr = NULL;
	switch (deckind)
	{
	case IdK:
		Ptr = nameType(t); break;         /*����Ϊ�Զ����ʶ��*/
	case ArrayK:
		Ptr = arrayType(t); break;        /*����Ϊ��������*/
	case RecordK:
		Ptr = recordType(t); break;       /*����Ϊ��¼����*/
	case IntegerK:
		Ptr = intPtr; break;              /*����Ϊ��������*/
	case CharK:
		Ptr = charPtr; break;             /*����Ϊ�ַ�����*/
	}
	return Ptr;
}


/************************************************************/
/* ������  nameType                                         */
/* ��  ��  �ú��������ڷ��ű���Ѱ���Ѷ������������         */
/* ˵  ��  ����Ѱ�ұ����ַ����FindEntry�������ҵ��ı����ַ*/
/*		   ָ��entry�����presentΪFALSE����������������*/
/*         ������ű��еĸñ�ʶ����������Ϣ�������ͣ������ */
/*         �ͱ�ʶ�����ú�������ָ��ָ����ű��еĸñ�ʶ���� */
/*	       �����ڲ���ʾ��								    */
/************************************************************/
TypeIR* nameType(TreeNode* t)
{
	TypeIR* Ptr = NULL;
	SymbTable* entry = NULL;
	int present;


	/*���ͱ�ʶ��Ҳ��Ҫ��ǰ�����*/
	present = FindEntry(t->attr.type_name, &entry);

	if (present == TRUE)
	{
		/*���ñ�ʶ���Ƿ�Ϊ���ͱ�ʶ��*/
		if (entry->attrIR.kind != typeKind)
			ErrorPrompt(t->lineno, t->attr.type_name, (char*)"used before typed!\n");
		else
			Ptr = entry->attrIR.idtype;
	}
	else/*û���ҵ��ñ�ʶ��*/
	{
		ErrorPrompt(t->lineno, t->attr.type_name, (char*)"type name is not declared!\n");
	}
	return Ptr;
}

/************************************************************/
/* ������  arrayType                                        */
/* ��  ��  �ú��������������͵��ڲ���ʾ                     */
/* ˵  ��  ����Ϊ��������ʱ����Ҫ����±��Ƿ�Ϸ���         */
/************************************************************/
TypeIR* arrayType(TreeNode* t)
{
	TypeIR* Ptr0 = NULL;
	TypeIR* Ptr1 = NULL;
	TypeIR* Ptr = NULL;

	/*��������Ͻ��Ƿ�С���½�*/
	if ((t->attr.ArrayAttr.low) > (t->attr.ArrayAttr.up))
	{
		ErrorPrompt(t->lineno, (char *)"", (char*)"array subscript error!\n");
		Error = TRUE;
	}
	else
	{
		Ptr0 = TypeProcess(t, IntegerK);
		/*�������ͷ��������������±�����*/
		Ptr1 = TypeProcess(t, t->attr.ArrayAttr.childtype);
		/*�������ͷ�������������Ԫ������*/
		Ptr = NewTy(arrayTy);
		/*ָ��һ�´�����������Ϣ��*/
		Ptr->size = ((t->attr.ArrayAttr.up) - (t->attr.ArrayAttr.low) + 1) * (Ptr1->size);
		/*���㱾���ͳ���*/

/*��д������Ϣ*/
		Ptr->More.ArrayAttr.indexTy = Ptr0;
		Ptr->More.ArrayAttr.elemTy = Ptr1;
		Ptr->More.ArrayAttr.low = t->attr.ArrayAttr.low;
		Ptr->More.ArrayAttr.up = t->attr.ArrayAttr.up;
	}
	return Ptr;
}


/************************************************************/
/* ������  recordType                                       */
/* ��  ��  �ú��������¼���͵��ڲ���ʾ                     */
/* ˵  ��  ����Ϊ��¼����ʱ�����ɼ�¼����ɵġ����ڲ��ڵ��� */
/*		   Ҫ����3����Ϣ:һ�ǿռ��Сsize���������������־ */
/*		   recordTy;�����岿�ֵĽڵ��ַbody����¼�����е�  */
/*		   �������Ǳ�ʶ���Ķ����Գ��֣������Ҫ��¼�����ԡ� */
/************************************************************/
TypeIR* recordType(TreeNode* t)
{
	TypeIR* Ptr = NewTy(recordTy);  /*�½���¼���͵Ľڵ�*/

	t = t->child[0];                /*���﷨���Ķ��ӽڵ��ȡ����Ϣ*/


	fieldChain* Ptr2 = NULL;
	fieldChain* Ptr1 = NULL;

	fieldChain* body = NULL;

	while (t != NULL)				/*ѭ������*/
	{
		/*��дptr2ָ������ݽڵ�*
		 *�˴�ѭ���Ǵ���������int a,b; */
		for (int i = 0; i < t->idnum; i++)
		{
			/*�����µ������͵�Ԫ�ṹPtr2*/
			Ptr2 = NewBody();
			if (body == NULL)
				body = Ptr1 = Ptr2;

			/*��дPtr2�ĸ�����Ա����*/
			strcpy(Ptr2->id, t->name[i]);
			Ptr2->UnitType = TypeProcess(t, t->kind.dec);

			Ptr2->Next = NULL;

			/*���Ptr1!=Ptr2����ô��ָ�����*/
			if (Ptr2 != Ptr1)
			{
				/*����������ĵ�Ԫoff*/
				Ptr2->off = (Ptr1->off) + (Ptr1->UnitType->size);
				Ptr1->Next = Ptr2;
				Ptr1 = Ptr2;
			}
		}
		/*������ͬ���͵ı�����ȡ�﷨�����ֵܽڵ�*/
		t = t->sibling;
	}

	/*�����¼�����ڲ��ṹ*/

	/*ȡPtr2��offΪ���������¼��size*/
	Ptr->size = Ptr2->off + (Ptr2->UnitType->size);
	/*�����������¼���͵�body����*/
	Ptr->More.body = body;

	return Ptr;
}



/************************************************************/



/*********************�������������*************************/

/************************************************************/
/* ������  TypeDecPart                                      */
/* ��  ��  �ú������������������������                     */
/* ˵  ��  ��������Tʱ���������ڲ��ڵ�TPtr������"idname=T"��*/
/*		   ����ű����鱾�������������Ƿ����ظ��������.*/
/************************************************************/
void TypeDecPart(TreeNode* t)
{
	int present = FALSE;

	AttributeIR  attrIr;

	SymbTable* entry = NULL;

	/*��������Ϊ����*/
	attrIr.kind = typeKind;

	/*�����﷨�����ֵܽڵ�*/
	while (t != NULL)
	{
		/*���ü�¼���Ժ����������Ƿ��ظ����������ڵ�ַ*/
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
/* ������  VarDecPart                                       */
/* ��  ��  �ú�����������������������                     */
/* ˵  ��  ���ñ��������������                             */
/************************************************************/
void VarDecPart(TreeNode* t)
{
	varDecList(t);
}

/************************************************************/
/* ������  varDecList                                       */
/* ��  ��  �ú�����������������������                     */
/* ˵  ��  ������������ʶ��idʱ����id�Ǽǵ����ű��У������ */
/*         ���Զ��壻��������ʱ���������ڲ���ʾ��           */
/************************************************************/
void  varDecList(TreeNode* t)
{
	AttributeIR  attrIr;
	int present = FALSE;

	SymbTable* entry = NULL;

	while (t != NULL)	/*ѭ������*/
	{
		attrIr.kind = varKind;
		for (int i = 0; i < (t->idnum); i++)
		{
			attrIr.idtype = TypeProcess(t, t->kind.dec);

			/*�ж�ʶֵ�λ��Ǳ��acess(dir,indir)*/
			if (t->attr.ProcAttr.paramt == varparamType)
			{
				attrIr.More.VarAttr.access = indir;
				attrIr.More.VarAttr.level = Level;
				/*�����βε�ƫ��*/

				attrIr.More.VarAttr.off = Off;
				Off = Off + 1;
			}/*����Ǳ�Σ���ƫ�Ƽ�1*/

			else
			{
				attrIr.More.VarAttr.access = dir;
				attrIr.More.VarAttr.level = Level;
				/*����ֵ�ε�ƫ��*/
				if (attrIr.idtype != NULL)

				{
					attrIr.More.VarAttr.off = Off;
					Off = Off + (attrIr.idtype->size);
				}
			}/*���������Ϊֵ�Σ�ƫ�Ƽӱ������͵�size*/

			/*�ǼǸñ��������Լ�����,�������������ڲ�ָ��*/
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

	/*��������������¼��ʱƫ�ƣ�����Ŀ���������ʱ��displayOff*/
	if (Level == 0)
	{
		mainOff = Off;
		/*�洢������AR��display���ƫ�Ƶ�ȫ�ֱ���*/
		StoreNoff = Off;
	}
	/*����������������¼��ʱƫ�ƣ�����������д������Ϣ���noff��Ϣ*/
	else
		savedOff = Off;

}

/************************************************************/
/* ������  procDec                                      */
/* ��  ��  �ú�����������������������                     */
/* ˵  ��  �ڵ�ǰ����ű�����д���̱�ʶ�������ԣ����²���� */
/*         ������д�βα�ʶ�������ԡ�						*/
/************************************************************/
void  procDecPart(TreeNode* t)
{
	TreeNode* p = t;
	SymbTable* entry = HeadProcess(t);   /*�������ͷ*/

	t = t->child[1];
	/*��������ڲ������������֣�������������*/
	while (t != NULL)
	{
		switch (t->nodekind)
		{
		case  TypeK:     TypeDecPart(t->child[0]);  break;
		case  VarK:     VarDecPart(t->child[0]);   break;

			/*������������к���������������ѭ��������дnoff��moff����Ϣ��*
			 *�ٴ�����������ѭ�����������޷�����noff��moff��ֵ��      */
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
	/*���̻��¼�ĳ��ȵ���nOff����display��ĳ���*
	 *diplay��ĳ��ȵ��ڹ������ڲ�����һ           */

	 /*����������������*/
	while (t != NULL)
	{
		procDecPart(t);
		t = t->sibling;
	}
	t = p;
	Body(t->child[2]);/*����Block*/

	/*�������ֽ�����ɾ�������β�ʱ���½����ķ��ű�*/
	if (Level != -1)
		DestroyTable();/*������ǰscope*/
}


/************************************************************/
/* ������  HeadProcess                                      */
/* ��  ��  �ú���������ͷ���������                       */
/* ˵  ��  �ڵ�ǰ����ű�����д������ʶ�������ԣ����²���� */
/*         ������д�βα�ʶ�������ԡ����й��̵Ĵ�С�ʹ��붼 */
/*         ���Ժ���                                     */
/************************************************************/
SymbTable* HeadProcess(TreeNode* t)
{
	AttributeIR attrIr;
	int present = FALSE;
	SymbTable* entry = NULL;

	/*������*/
	attrIr.kind = procKind;
	attrIr.idtype = NULL;
	attrIr.More.ProcAttr.level = Level + 1;

	if (t != NULL)
	{
		/*�ǼǺ����ķ��ű���*/
		present = Enter(t->name[0], &attrIr, &entry);
		t->table[0] = entry;
		/*�����β�������*/
	}
	entry->attrIR.More.ProcAttr.param = ParaDecList(t);

	return entry;
}

/************************************************************/
/* ������  ParaDecList                                      */
/* ��  ��  �ú���������ͷ�еĲ����������������           */
/* ˵  ��  ���µķ��ű��еǼ������βεı�������βα���� */
/*         ��ַ������paraָ���䡣                         */
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
			p = t->child[0];   	/*���������ڵ�ĵ�һ�����ӽڵ�*/

		CreatTable();			/*�����µľֲ�����*/

		Off = 7;                /*�ӳ����еı�����ʼƫ����Ϊ8*/

		VarDecPart(p);			/*������������*/

		SymbTable* Ptr0 = scope[Level];

		while (Ptr0 != NULL)         /*ֻҪ��Ϊ�գ��ͷ������ֵܽڵ�*/
		{
			/*�����βη��ű���ʹ�����������ű��param��*/
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
	return head;   /*�����βη��ű��ͷָ��*/
}


/*******************ִ���岿�ֵ��������*********************/

/************************************************************/
/* ������  Body                                             */
/* ��  ��  �ú�������ִ���岿�ֵ��������                   */
/* ˵  ��  TINY����ϵͳ��ִ���岿�ּ�Ϊ������У���ֻ�账�� */
/*         ������в��֡�                                   */
/************************************************************/
void Body(TreeNode* t)
{
	if (t->nodekind == StmLK)
	{
		TreeNode* p = t->child[0];
		while (p != NULL)
		{
			statement(p);  /*�������״̬������*/
			p = p->sibling;   /*���ζ����﷨��������е��ֵܽڵ�*/
		}
	}
}


/************************************************************/
/* ������  statement                                        */
/* ��  ��  �ú����������״̬                               */
/* ˵  ��  �����﷨���ڵ��е�kind���ж�Ӧ��ת�����ĸ���� */
/*         ���ͺ�����                                       */
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
/* ������  Expr                                             */
/* ��  ��  �ú���������ʽ�ķ���                           */
/* ˵  ��  ���ʽ����������ص��Ǽ��������������������ԣ� */
/*         ����ʽ�����͡����в���Ekind������ʾʵ���Ǳ��  */
/*         ����ֵ�Ρ�    	                                */
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
				(*Ekind) = dir;   /*ֱ�ӱ���*/
			break;
		case VariK:
			/*Var = id������*/
			if (t->child[0] == NULL)
			{
				/*�ڷ��ű��в��Ҵ˱�ʶ��*/
				present = FindEntry(t->name[0], &entry);
				t->table[0] = entry;

				if (present != FALSE)
				{   /*id���Ǳ���*/
					if (FindAttr(entry).kind != varKind)
					{
						ErrorPrompt(t->lineno, t->name[0], (char*)"is not variable error!\n");
						Eptr = NULL;
					}
					else
					{
						Eptr = entry->attrIR.idtype;
						if (Ekind != NULL)
							(*Ekind) = indir;  /*��ӱ���*/

					}
				}
				else /*��ʶ��������*/
				{
					ErrorPrompt(t->lineno, t->name[0], (char*)"is not declarations!\n");
				}

			}
			else/*Var = Var0[E]������*/
			{
				if (t->attr.ExpAttr.varkind == ArrayMembV)
					Eptr = arrayVar(t);
				else /*Var = Var0.id������*/
					if (t->attr.ExpAttr.varkind == FieldMembV)
						Eptr = recordVar(t);
			}
			break;
		case OpK:
			/*�ݹ���ö��ӽڵ�*/
			Eptr0 = Expr(t->child[0], NULL);
			if (Eptr0 == NULL)
				return NULL;
			Eptr1 = Expr(t->child[1], NULL);
			if (Eptr1 == NULL)
				return NULL;

			/*�����б�*/
			present = Compat(Eptr0, Eptr1);
			if (present != FALSE)
			{
				switch (t->attr.ExpAttr.op)
				{
				case LT:
				case EQ:
					Eptr = boolPtr;
					break;  /*�������ʽ*/
				case PLUS:
				case MINUS:
				case TIMES:
				case OVER:
					Eptr = intPtr;
					break;  /*�������ʽ*/
				}
				if (Ekind != NULL)
					(*Ekind) = dir; /*ֱ�ӱ���*/
			}
			else
				ErrorPrompt(t->lineno, (char*)"", (char*)"operator is not compat!\n");
			break;
		}
	return Eptr;
}


/************************************************************/
/* ������  arrayVar                                         */
/* ��  ��  �ú�����������������±����                     */
/* ˵  ��  ���var := var0[E]��var0�ǲ����������ͱ�����E�ǲ�*/
/*         �Ǻ�������±��������ƥ�䡣                     */
/************************************************************/
TypeIR* arrayVar(TreeNode* t)
{
	int present = FALSE;
	SymbTable* entry = NULL;

	TypeIR* Eptr0 = NULL;
	TypeIR* Eptr1 = NULL;
	TypeIR* Eptr = NULL;


	/*�ڷ��ű��в��Ҵ˱�ʶ��*/

	present = FindEntry(t->name[0], &entry);
	t->table[0] = entry;
	/*�ҵ�*/
	if (present != FALSE)
	{
		/*Var0���Ǳ���*/
		if (FindAttr(entry).kind != varKind)
		{
			ErrorPrompt(t->lineno, t->name[0], (char*)"is not variable error!\n");
			Eptr = NULL;
		}
		else/*Var0�����������ͱ���*/
			if (FindAttr(entry).idtype != NULL)
				if (FindAttr(entry).idtype->kind != arrayTy)
				{
					ErrorPrompt(t->lineno, t->name[0], (char*)"is not array variable error !\n");
					Eptr = NULL;
				}
				else
				{
					/*���E�������Ƿ����±��������*/
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
	else/*��ʶ��������*/
		ErrorPrompt(t->lineno, t->name[0], (char*)"is not declarations!\n");
	return Eptr;
}


/************************************************************/
/* ������  recordVar                                        */
/* ��  ��  �ú��������¼��������ķ���                     */
/* ˵  ��  ���var:=var0.id�е�var0�ǲ��Ǽ�¼���ͱ�����id�� */
/*         ���Ǹü�¼�����е����Ա��                       */
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


	/*�ڷ��ű��в��Ҵ˱�ʶ��*/
	present = FindEntry(t->name[0], &entry);
	t->table[0] = entry;
	/*�ҵ�*/
	if (present != FALSE)
	{
		/*Var0���Ǳ���*/
		if (FindAttr(entry).kind != varKind)
		{
			ErrorPrompt(t->lineno, t->name[0], (char*)"is not variable error!\n");
			Eptr = NULL;
		}
		else/*Var0���Ǽ�¼���ͱ���*/
			if (FindAttr(entry).idtype->kind != recordTy)
			{
				ErrorPrompt(t->lineno, t->name[0], (char*)"is not record variable error !\n");
				Eptr = NULL;
			}
			else/*���id�Ƿ��ǺϷ�����*/
			{
				Eptr0 = entry->attrIR.idtype;
				currentP = Eptr0->More.body;
				while ((currentP != NULL) && (result != FALSE))
				{
					result = strcmp(t->child[0]->name[0], currentP->id);
					/*������*/
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
					else/*���id���������*/
						if (t->child[0]->child[0] != NULL)
							Eptr = arrayVar(t->child[0]);
			}
	}
	else/*��ʶ��������*/
		ErrorPrompt(t->lineno, t->name[0], (char*)"is not declarations!\n");
	return Eptr;
}


/************************************************************/
/* ������  assignstatement                                  */
/* ��  ��  �ú�������ֵ������                           */
/* ˵  ��  ��ֵ��������������ص��Ǽ�鸳ֵ�����˷������� */
/*         �������ԡ�    	                                */
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
		/*�ڷ��ű��в��Ҵ˱�ʶ��*/
		present = FindEntry(child1->name[0], &entry);

		if (present != FALSE)
		{   /*id���Ǳ���*/
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
		else /*��ʶ��������*/
			ErrorPrompt(child1->lineno, child1->name[0], (char*)"is not declarations!\n");
	}
	else/*Var0[E]������*/
	{
		if (child1->attr.ExpAttr.varkind == ArrayMembV)
			Eptr = arrayVar(child1);
		else /*Var0.id������*/
			if (child1->attr.ExpAttr.varkind == FieldMembV)
				Eptr = recordVar(child1);
	}
	if (Eptr != NULL)
	{
		if ((t->nodekind == StmtK) && (t->kind.stmt == AssignK))
		{
			/*����ǲ��Ǹ�ֵ������ ���͵ȼ�*/
			ptr = Expr(child2, NULL);
			if (!Compat(ptr, Eptr))
				ErrorPrompt(t->lineno, (char*)"", (char*)"ass_expression error!\n");
		}
		/*��ֵ����в��ܳ��ֺ�������*/
	}
}

/************************************************************/
/* ������  callstatement                                    */
/* ��  ��  �ú�������������������                       */
/* ˵  ��  ����������������������ȼ����ű������������ */
/*         ��Param���֣��βη��ű����ַ��������������β�*/
/*         ��ʵ��֮��Ķ�Ӧ��ϵ�Ƿ���ȷ��                   */
/************************************************************/
void callstatement(TreeNode* t)
{
	AccessKind  Ekind;
	int present = FALSE;
	SymbTable* entry = NULL;
	TreeNode* p = NULL;

	/*��id����������ű�*/
	present = FindEntry(t->child[0]->name[0], &entry);
	t->child[0]->table[0] = entry;

	/*δ�鵽��ʾ����������*/
	if (present == FALSE)
	{
		ErrorPrompt(t->lineno, t->child[0]->name[0], (char*)"function is not declarationed!\n");
	}
	else
		/*id���Ǻ�����*/
		if (FindAttr(entry).kind != procKind)
			ErrorPrompt(t->lineno, t->name[0], (char*)"is not function name!\n");
		else/*��ʵ��ƥ��*/
		{
			p = t->child[1];
			/*paramPָ���βη��ű�ı�ͷ*/
			ParamTable* paramP = FindAttr(entry).More.ProcAttr.param;
			while ((p != NULL) && (paramP != NULL))
			{
				SymbTable* paraEntry = paramP->entry;
				TypeIR* Etp = Expr(p, &Ekind);/*ʵ��*/
				/*�������ƥ��*/
				if ((FindAttr(paraEntry).More.VarAttr.access == indir) && (Ekind == dir))
					ErrorPrompt(p->lineno, (char*)"", (char*)"param kind is not match!\n");
				else
					/*�������Ͳ�ƥ��*/
					if ((FindAttr(paraEntry).idtype) != Etp)
						ErrorPrompt(p->lineno, (char*)"", (char*)"param type is not match!\n");
				p = p->sibling;
				paramP = paramP->next;
			}
			/*����������ƥ��*/
			if ((p != NULL) || (paramP != NULL))
				ErrorPrompt(t->child[1]->lineno, (char*)"", (char*)"param num is not match!\n");
		}
}


/************************************************************/
/* ������  ifstatement                                      */
/* ��  ��  �ú�����������������                           */
/* ˵  ��  �����﷨�����������ӽڵ�                         */
/************************************************************/
void ifstatment(TreeNode* t)
{
	AccessKind* Ekind = NULL;
	TypeIR* Etp = Expr(t->child[0], Ekind);
	if (Etp != NULL)
		/*�����������ʽ*/
		if (Etp->kind != boolTy)
			ErrorPrompt(t->lineno, (char*)"", (char*)"condition expressrion error!\n");  /*�߼����ʽ����*/
		else
		{
			TreeNode* p = t->child[1];
			/*����then������в���*/
			while (p != NULL)
			{
				statement(p);
				p = p->sibling;
			}
			t = t->child[2];		/*����������*/
			/*����else��䲻��*/
			while (t != NULL)
			{
				statement(t);
				t = t->sibling;
			}
		}
}


/************************************************************/
/* ������  whilestatement                                   */
/* ��  ��  �ú�������ѭ��������                           */
/* ˵  ��  �����﷨�����������ӽڵ�                         */
/************************************************************/
void whilestatement(TreeNode* t)
{
	TypeIR* Etp = Expr(t->child[0], NULL);
	if (Etp != NULL)
		/*�����������ʽ����*/
		if (Etp->kind != boolTy)
			ErrorPrompt(t->lineno, (char*)"", (char*)"condition expression error!\n");  /*�߼����ʽ����*/
		else
		{
			t = t->child[1];
			/*����ѭ������*/
			while (t != NULL)
			{
				statement(t);
				t = t->sibling;
			}
		}
}


/************************************************************/
/* ������  readstatement                                    */
/* ��  ��  �ú�����������������                           */
/* ˵  ��  �����﷨���ڵ㣬�����������������Ƿ�Ϊ�������� */
/************************************************************/
void readstatement(TreeNode* t)
{
	SymbTable* entry = NULL;
	int present = FALSE;

	/*��id����������ű�*/
	present = FindEntry(t->name[0], &entry);
	t->table[0] = entry;

	/*δ�鵽��ʾ����������*/
	if (present == FALSE)
		ErrorPrompt(t->lineno, t->name[0], (char*)" is not declarationed!\n");
	else
		/*���Ǳ�����ʶ������*/
		if (entry->attrIR.kind != varKind)
			ErrorPrompt(t->lineno, t->name[0], (char*)"is not var name!\n ");
}

/************************************************************/
/* ������  writestatement                                   */
/* ��  ��  �ú����������������                           */
/* ˵  ��  �����������еı��ʽ�Ƿ�Ϸ�                   */
/************************************************************/
void writestatement(TreeNode* t)
{
	TypeIR* Etp = Expr(t->child[0], NULL);
	if (Etp != NULL)
		/*������ʽ����Ϊbool���ͣ�����*/
		if (Etp->kind == boolTy)
			ErrorPrompt(t->lineno, (char*)"", (char*)"exprssion type error!");
}


/************************************************************/
/* ������  returnstatement                                  */
/* ��  ��  �ú�������������������                       */
/* ˵  ��  ����������������Ƿ����������г���               */
/************************************************************/
void returnstatement(TreeNode* t)
{
	if (Level == 0)
		/*����������������������У�����*/
		ErrorPrompt(t->lineno, (char*)"", (char*)"return statement error!");
}

/************************************************************/
/* ������  analyze                                          */
/* ��  ��  �ú��������ܵ��������                           */
/* ˵  ��  ���﷨�����з���                                 */
/************************************************************/
void analyze(TreeNode* t)
{
	SymbTable* entry = NULL;
	TreeNode* p = NULL;
	TreeNode* pp = t;

	/*�������ű�*/
	CreatTable();

	/*���������ڲ���ʾ��ʼ������*/
	initialize();

	/*�﷨���������ڵ�*/
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
		p = p->sibling;/*ѭ������*/
	}

	/*������*/
	t = t->child[2];
	if (t->nodekind == StmLK)
		Body(t);

	/*�������ű�*/
	if (Level != -1)
		DestroyTable();

	/*����������*/
	if (Error == TRUE)
		fprintf(listing, "\n�������:\n");
	/*����޴����������ʾ��Ϣ*/
//	else
//		fprintf(listing, "\n........ no error!\n");
}











