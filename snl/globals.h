/************************************************************/
/* �ļ�	 globals.h											*/
/* ˵��  ��PASCAL���Ա�������ȫ�����ͺͱ���,Ӧ�����������ļ�֮ǰ	*/
/* ����  �������ṹ:ԭ���ʵ��								*/
/************************************************************/

/* ���Ѿ�����ͷ�ļ�globals.h�������� */
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

/****** globals.h������ͷ�ļ� ******/
#include "stdio.h"
#include "stdlib.h"

/* ���峣��FALSEΪ0 */
#ifndef FALSE
#define FALSE 0
#endif

/* ���峣��TRUEΪ1 */
#ifndef TRUE
#define TRUE 1
#endif


/* ���屣������������MAXRESERVEDΪ21 */
#define MAXRESERVED 21


/* MAXTOKENLENΪ������󳤶ȶ���Ϊ40 */
#define MAXTOKENLEN 40

/*��ʼ�����ű��б�����ƫ��*/
#define INITOFF 7

/* SCOPESIZEΪ���ű�scopeջ�Ĵ�С*/
#define SCOPESIZE 1000

/*����token�ĳ���*/
#define TOKENLEN sizeof (TokenType)

/*��������ڵ�ĳ���*/
#define CHAINNODELEN sizeof (ChainNodeType) 


/********************** ���ʵĴʷ����� ********************/
typedef enum
{
	/* ���ǵ��ʷ��� */
	ENDFILE, ERROR,
	/* ������ */
	PROGRAM, PROCEDURE, TYPE, VAR, IF,
	THEN, ELSE, FI, WHILE, DO,
	ENDWH, BEGIN, END, READ, WRITE,
	ARRAY, OF, RECORD, RETURN,

	INTEGER, CHAR,
	/* ���ַ����ʷ��� */
	ID, INTC, CHARC,
	/*������� */
	ASSIGN, EQ, LT, PLUS, MINUS,
	TIMES, OVER, LPAREN, RPAREN, DOT,
	COLON, SEMI, COMMA, LMIDPAREN, RMIDPAREN,
	UNDERANGE
} LexType;


/*********** ���ʵ����ͣ������ʷ���Ϣ��������Ϣ ************/
typedef struct tokenType
{
	int     lineshow;
	LexType Lex;
	char    Sem[MAXTOKENLEN + 1];
} TokenType;


/***********����Ľ�����ͣ�����Token���ֺ�ָ�벿��**********/
typedef struct  node
{
	TokenType   Token;      //����
	struct node* nextToken; //ָ����һ�����ʵ�ָ��
} ChainNodeType;




/******************************************************
 ******************   �﷨������   ********************
 ******************************************************/

 /*�﷨�����ڵ�ProK,����ͷ���PheadK���������ͽڵ�DecK,
   ��־�ӽ�㶼�����������Ľ��TypeK,��־�ӽ�㶼�Ǳ��������Ľ��VarK,
   �����������FuncDecK,������нڵ�StmLK,����������StmtK,
   ���ʽ���ExpK*/
typedef enum { ProK, PheadK, DecK, TypeK, VarK, ProcDecK, StmLK, StmtK, ExpK }
NodeKind;


/*��������Deckind ���͵�ö�ٶ��壺
  ��������ArrayK,�ַ�����CharK,
  ��������IntegerK,��¼����RecordK,
  �����ͱ�ʶ����Ϊ���͵�IdK*/
typedef enum { ArrayK, CharK, IntegerK, RecordK, IdK }  DecKind;



/* �������StmtKind���͵�ö�ٶ���:			*
 * �ж�����IfK,ѭ������WhileK				*
 * ��ֵ����AssignK,������ReadK              *
 * д����WriteK��������������CallK          */
typedef enum { IfK, WhileK, AssignK, ReadK, WriteK, CallK, ReturnK } StmtKind;


/* ���ʽ����ExpKind���͵�ö�ٶ���:         *
 * ��������OpK,��������ConstK,��������VarK */
typedef enum { OpK, ConstK, VariK } ExpKind;


/* ��������VarKind���͵�ö�ٶ���:           *
 * ��ʶ��IdV,�����ԱArrayMembV,���ԱFieldMembV*/
typedef enum { IdV, ArrayMembV, FieldMembV } VarKind;


/* ���ͼ��ExpType���͵�ö�ٶ���:           *
 * ��Void,��������Integer,�ַ�����Char      */
typedef enum { Void, Integer, Boolean } ExpType;

/* ��������ParamType���͵�ö�ٶ��壺        *
 * ֵ��valparamType,���varparamType        */
typedef enum { valparamType, varparamType } ParamType;

/* �����﷨���ڵ������ӽڵ���MAXCHILDRRENΪ3 */
/* �����������ֵ��ӽڵ�child[0]ָ��������֣�
   �ӽڵ�child[1]ָ�������岿�֣��ӽڵ�child[2]
   ָ��������䲿�֣�*/
#define MAXCHILDREN 3 

   /*��ǰ�������ű�ṹ*/
struct symbtable;

/********** �﷨���ڵ�treeNode���� *********/
typedef struct treeNode

{
	struct treeNode* child[MAXCHILDREN];		/* �ӽڵ�ָ��	*/
	struct treeNode* sibling;					/* �ֵܽڵ�ָ��	*/
	int lineno;								/* Դ�����к�	*/
	NodeKind nodekind;						    /* �ڵ�����		*/
	union
	{
		DecKind  dec;
		StmtKind stmt;
		ExpKind  exp;
	} kind;                       /* ��������     */

	int idnum;                    /* ��ͬ���͵ı������� */

	char name[10][10];            /* ��ʶ��������  */

	struct symbtable* table[10]; /* ���־����Ӧ�ķ��ű��ַ������������׶�����*/

	struct
	{
		struct
		{
			int low;              /* �����½�     */
			int up;               /* �����Ͻ�     */
			DecKind   childtype;  /* ����������� */
		}ArrayAttr;               /* ��������     */

		struct
		{
			ParamType  paramt;     /* ���̵Ĳ�������*/
		}ProcAttr;                 /* ��������      */

		struct
		{
			LexType op;           /* ���ʽ�Ĳ�����*/
			int val;		      /* ���ʽ��ֵ	   */
			VarKind  varkind;     /* ���������    */
			ExpType type;         /* �������ͼ��  */
		}ExpAttr;	              /* ���ʽ����    */

		char type_name[10];             /* �������Ǳ�ʶ��  */

	} attr;                          /* ����	       */
}TreeNode;

/*���ռ���������*/
#define  NTMLNUM    68

/*�ռ���������*/
#define  TMLNUM     42

/*LL1������Ĵ�С*/
#define  TABLESIZE  104

/******************���ͺͱ�������*********************/

/* 1.����LL1�﷨�����õ������ͼ���Ӧ�ı���  */

/*���з��ռ���������Ժ���ɲο�LL1�ķ�*/
typedef enum
{
	Program, ProgramHead, ProgramName, DeclarePart,
	TypeDec, TypeDeclaration, TypeDecList, TypeDecMore,
	TypeId, TypeName, BaseType, StructureType,
	ArrayType, Low, Top, RecType,
	FieldDecList, FieldDecMore, IdList, IdMore,
	VarDec, VarDeclaration, VarDecList, VarDecMore,
	VarIdList, VarIdMore, ProcDec, ProcDeclaration,
	ProcDecMore, ProcName, ParamList, ParamDecList,
	ParamMore, Param, FormList, FidMore,
	ProcDecPart, ProcBody, ProgramBody, StmList,
	StmMore, Stm, AssCall, AssignmentRest,
	ConditionalStm, StmL, LoopStm, InputStm,
	InVar, OutputStm, ReturnStm, CallStmRest,
	ActParamList, ActParamMore, RelExp, OtherRelE,
	Exp, OtherTerm, Term, OtherFactor,
	Factor, Variable, VariMore, FieldVar,
	FieldVarMore, CmpOp, AddOp, MultOp

}  NontmlType;

/*�����ռ�����ȡ�Ե��ʵĴʷ����͵�ö�ٶ��壬�ɲο�zglobals.hͷ�ļ�*/
typedef LexType  TmlType;


/*ʵ��LL1�����õķ���ջ����ŵ����ռ����ͷ��ռ��� */
typedef struct Node
{  /*���ݱ�־*/
	int flag;    /* flagΪ1����ʾջ������Ϊ���ռ�����*/
				 /* flagΪ2����ʾջ������Ϊ�ռ���    */
	/*����*/
	union {
		NontmlType Ntmlvar;  /*���ռ�������*/
		TmlType tmlvar;       /*�ռ�������*/
	} var;

	/*ָ����һ���ڵ��ָ��*/
	struct Node* underNode;

}  StackNode;


/* 2.�����﷨����������ͼ�����**/

/*Ϊ������������䲿�ֵ��﷨�������ָ��ջ��*
 *ע��ջ�д���Ǵ��ָ��ĵ�ַ              */
typedef struct NodePA
{
	TreeNode** pointer;
	struct NodePA* underNode;
}  StackNodePA;


/*Ϊ���ɱ��ʽ���ֵ��﷨�������ָ��ջ��*
 *ջ�д���Ǵ��ָ�����ڵ��ָ��        */
typedef struct NodeP
{
	TreeNode* pointer;
	struct NodeP* underNode;
}StackNodeP;

/*����ջ��ָ��*/
extern StackNode* StackTop;

/*ջ�ձ�־*/
extern  int STACKEMPTY;

/*�﷨��ջ��ָ��*/
extern  StackNodePA* StackTopPA;
/*ջ�ձ�־*/
extern  int  paSTACKEMPTY;

/*������ջ��ջ��ָ��*/
extern  StackNodeP* OpStackTop;
/*������ջ�ձ�־*/
extern  int OpSTACKEMPTY;

/*������ջ��ջ��ָ��*/
extern  StackNodeP* NumStackTop;

/*������ջ�ձ�־*/
extern  int NumSTACKEMPTY;



/******************************************************
 **********���������Ҫ�õ������ͼ���������************
 ******************************************************/


 /*��ʶ��������*/
typedef  enum { typeKind, varKind, procKind }IdKind;

/*���������dir��ֱ�ӱ���(ֵ��)��indir��ʾ��ӱ���(���)*/
typedef  enum { dir, indir }AccessKind;

/*�βα�Ľṹ����*/
typedef struct  paramTable
{
	struct symbtable* entry;/*ָ����β����ڷ��ű��еĵ�ַ���*/
	struct paramTable* next;
}ParamTable;


struct typeIR;
/*��ʶ�������Խṹ����*/
typedef struct
{
	struct typeIR* idtype;		/*ָ���ʶ���������ڲ���ʾ*/
	IdKind    kind;					/*��ʶ��������*/
	union
	{
		struct
		{
			AccessKind   access;   /*�ж��Ǳ�λ���ֵ��*/
			int          level;
			int          off;
			bool         isParam;  /*�ж��ǲ���������ͨ����*/

		}VarAttr;/*������ʶ��������*/
		struct
		{
			int         level;     /*�ù��̵Ĳ���*/

			ParamTable* param;   /*������*/

			int         mOff;	   /*���̻��¼�Ĵ�С*/

			int         nOff;  	   /*sp��display���ƫ����*/

			int         procEntry; /*���̵���ڵ�ַ*/

			int         codeEntry;/*������ڱ��,�����м��������*/

		}ProcAttr;/*��������ʶ��������*/

	}More;/*��ʶ���Ĳ�ͬ�����в�ͬ������*/

}AttributeIR;


/*���ű�Ľṹ����*/
typedef struct  symbtable
{
	char  idName[10];
	AttributeIR  attrIR;
	struct symbtable* next;

}SymbTable;

/*ʹ��scopeջ�ľֲ����ű��������õ���scopeջ*/
extern SymbTable* scope[1000];

/*scopeջ�Ĳ���*/
extern int Level;

/*��ͬ��ı���ƫ��*/
extern int Off;

/*��¼�������displayOff*/
extern int mainOff;

/*��¼��ǰ���displayOff*/
extern int savedOff;




/******************************************************
 *****************   �����ڲ���ʾ    ******************
 ******************************************************/

 /*���͵�ö�ٶ���*/
typedef  enum { intTy, charTy, arrayTy, recordTy, boolTy }TypeKind;


struct typeIR;

/*�����͵�Ԫ�ṹ����*/
typedef struct fieldchain
{
	char   id[10];              /*������*/
	int    off;                 /*���ڼ�¼�е�ƫ��*/
	struct typeIR* UnitType; /*���г�Ա������*/
	struct fieldchain* Next;
}fieldChain;


/*���͵��ڲ��ṹ����*/
typedef   struct  typeIR
{
	int				size;   /*������ռ�ռ��С*/
	TypeKind		kind;
	union
	{
		struct
		{
			struct typeIR* indexTy;
			struct typeIR* elemTy;
			int    low;     /*��¼�������͵��½�*/
			int    up;      /*��¼�������͵��Ͻ�*/
		}ArrayAttr;
		fieldChain* body;  /*��¼�����е�����*/
	} More;
}TypeIR;



/**************************************************************/
/******�������м�������ɲ�����Ҫ�õ������ͼ���������**********/
/**************************************************************/

/*ARG�ṹ��Ϊ����ţ���ֵ����ַ����*/
typedef  enum { LabelForm, ValueForm, AddrForm }  ArgForm;

/*����ARG�ṹ*/
typedef struct
{
	ArgForm   form;
	union
	{
		int  value;  /*��¼����ֵ*/
		int  label;  /*��¼��ŵ�ֵ*/
		struct
		{
			char    name[10];	/*ע�����������Ѿ�û�ã����ﱣ��ֻ��Ϊ����ʾ�������*/
			int     dataLevel;
			int     dataOff;
			AccessKind  access; /*����AccessKind��ǰ�涨��*/
		}addr;   /*������ARG�ṹ��Ҫ��¼����Ϣ*/
	}Attr;
} ArgRecord;

/*�м��������*/
typedef  enum
{
	ADD, SUB, MULT, DIV, EQC, LTC,
	READC, WRITEC, RETURNC, ASSIG, AADD, LABEL,
	JUMP0, JUMP, CALL, VARACT, VALACT,
	PENTRY, ENDPROC, MENTRY, WHILESTART, ENDWHILE
}  CodeKind;

/*�м����Ľṹ*/
typedef struct
{
	CodeKind   codekind;
	ArgRecord* arg1;  /*�Ժ�����������ָ��ã�����ֱ����
						 ArgRecord���͵ı�����*/
	ArgRecord* arg2;
	ArgRecord* arg3;
} CodeR;

/*Դ�����Ӧ���м�������б�ʾ*/
typedef  struct  codeFile
{
	CodeR  codeR;
	struct codeFile* former;
	struct codeFile* next;
} CodeFile;



/**************************************************************/
/******�����ǳ����ʽ�Ż���Ҫ�õ������ͼ���������**************/
/**************************************************************/

/*������ֵ�����ڳ����ʽ�Ż�*/
typedef  struct  constDefT
{
	ArgRecord* variable;  /*�ñ�����ARG�ṹ��ʾ����*/
	int  constValue;       /*��ֵ*/
	struct constDefT* former;
	struct constDefT* next;

} ConstDefT;


/**************************************************************/
/******�����ǹ������ʽ�Ż���Ҫ�õ������ͼ���������************/
/**************************************************************/

/*ֵ�����ValuNum*/
typedef  struct  valuNum
{
	ArgRecord* arg;
	AccessKind  access;
	union
	{
		int  valueCode;   /*ֱ�ӱ������洢ֵ����*/
		struct
		{
			int   valuecode;
			int   addrcode;
		} twoCode;         /*�����ʱ�������洢ֵ����͵�ַ��*/

	} codeInfo;
	/*ָ����һ���ڵ�ָ��*/
	struct valuNum* next;

} ValuNum;

/*�м�����Ӧ��ӳ����ṹ*/
typedef struct
{
	int  op1;
	int  op2;
	int  result;
} MirrorCode;

/*���ñ��ʽ�����UsableExpr*/
typedef  struct  usableExpr
{
	CodeFile* code;		  /*�м�����ַ*/
	MirrorCode* mirrorC;      /*ӳ����*/
	struct usableExpr* next;  /*ָ����һ���ڵ�*/
} UsableExpr;


/*��ʱ�����ĵȼ۱�TempEqua*/
typedef  struct tempEqua
{
	ArgRecord* arg1; /*���滻����ʱ����*/
	ArgRecord* arg2; /*�����滻����ʱ����*/
	struct  tempEqua* next;
} TempEqua;


/**************************************************************/
/******������ѭ������ʽ�Ż���Ҫ�õ������ͼ���������************/
/**************************************************************/


/*ѭ����Ϣ��*/
typedef  struct
{
	int       state;        /*ѭ��״����Ϊ0ʱ��ʾ����ѭ����������*/
	CodeFile* whileEntry;  /*ָ��ѭ������м����*/
	int       varDef;       /*ָ�򱾲�ѭ���ı�����ַ����ʼ��*/
	CodeFile* whileEnd;    /*ָ��ѭ�������м����*/
} LoopInfo;

/*ѭ����Ϣջ*/
typedef  struct loopStack
{
	LoopInfo* loopInfo;
	struct loopStack* under;

} LoopStack;




/**************************************************************/
/******������Ŀ��������ɲ�����Ҫ�õ������ͼ���������**********/
/**************************************************************/

/*��ŵ�ַ��*/
typedef  struct  labelAddr
{
	int  label;
	int  destNum;
	struct labelAddr* next;
} LabelAddr;

/*��������ַҪ�õ������ݽṹ*/
typedef struct  backAddr
{
	int  backLoc;
	struct  backAddr* former;
} BackAddr;

extern  BackAddr* AddrTop;

extern  int AddrEMPTY;



/********************************************************************/
/* Դ�����ı��ļ�ָ��source */
extern FILE* source;

/* �б�����ļ�ָ��listing */
extern FILE* listing;

/*�ʷ��������Token���еĴ洢�ļ�ָ��fp*/
extern FILE* fp;

/*Ŀ������ļ�ָ��*/
extern FILE* code;

/*Token�����е�token��Ŀ*/
extern int Tokennum;

/*�嵥���к�*/
extern int lineno;

/*scopeջ�Ĳ���*/
extern int Level;

/*��ͬ��ı���ƫ��*/
extern int Off;

/*��¼�������displayOff*/
extern int mainOff;

/*��¼��ǰ���displayOff*/
extern int savedOff;

/******************************/
/*�����м���벿��ʱ�õ��ı���*/
/******************************/
/*��ʱ�������*/
extern  int  TempOffset;
/*���ֵ*/
extern  int  Label;

/*��һ���м����*/
extern  CodeFile* firstCode;
/*���һ���м����*/
extern  CodeFile* lastCode;



extern  CodeFile* baseBlock[100];

extern  int  blocknum;

extern  ValuNum* valuNumT;

extern  UsableExpr* usableExprT;

extern  TempEqua* tempEquaT;

/*ѭ����Ϣջ*/
extern  LoopStack* loopTop;
extern  bool  loopStackEmpty;



/*�����������display���ƫ��*/
extern  int StoreNoff;



/*************************************
 ***********   ׷�ٱ�־   ************
 *************************************/

 /* Դ����׷�ٱ�־,����ñ�־ΪTRUE			 *
  * �﷨����ʱ��Դ�����к�д���б��ļ�listing */
extern int EchoSource;

/* �ʷ�ɨ��׷�ٱ�־,����ñ�־ΪTRUE				   *
 * ��ÿ�����ʷ�ɨ����ʶ��ĵ�����Ϣд���б��ļ�listing */
extern int TraceScan;

/* �﷨����׷�ٱ�־,����ñ�־ΪTRUE				     *
 * ���﷨����������ʽ(ʹ���ӽڵ�����)д���б��ļ�listing */
extern int TraceParse;

/* �������׷�ٱ�־,����ñ�־ΪTRUE	 *
 * �����ű����Ͳ���д���б��ļ�listing */
extern int TraceTable;


extern int TraceCode;

/* ����׷�ٱ�־,����ñ�־ΪTRUE *
 * ��ֹ�������ʱ��һ�����ݴ���  */
extern int Error;

#endif

