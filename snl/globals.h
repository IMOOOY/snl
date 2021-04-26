//
//  globals.hpp
//  snl
//
//  Created by IMOOOY on 2021/4/25.
//
/**
 类PASCAL语言编译器的全局类型和变量,应在其他包含文件之前
 编译器结构:原理和实例
 */



/* 如已经联入头文件globals.h则不再联入 */
#ifndef _GLOBALS_H_
#define _GLOBALS_H_

/****** globals.h所包含头文件 ******/
#include "stdio.h"
#include "stdlib.h"

/* 定义常量FALSE为0 */
#ifndef FALSE
#define FALSE 0
#endif

/* 定义常量TRUE为1 */
#ifndef TRUE
#define TRUE 1
#endif


/* 定义保留字数量常量MAXRESERVED为21 */
#define MAXRESERVED 21


/* MAXTOKENLEN为单词最大长度定义为40 */
#define MAXTOKENLEN 40

///初始化符号表中变量的偏移
#define INITOFF 7

/* SCOPESIZE为符号表scope栈的大小*/
#define SCOPESIZE 1000

/*定义token的长度*/
#define TOKENLEN sizeof (TokenType)

/*定义链表节点的长度*/
#define CHAINNODELEN sizeof (ChainNodeType)


/********************** 单词的词法类型 ********************/
typedef enum
{
    /* 簿记单词符号 */
    ENDFILE, ERROR,
    /* 保留字 */
    PROGRAM, PROCEDURE, TYPE, VAR, IF,
    THEN, ELSE, FI, WHILE, DO,
    ENDWH, BEGIN, END, READ, WRITE,
    ARRAY, OF, RECORD, RETURN,

    INTEGER, CHAR,
    /* 多字符单词符号 */
    ID, INTC, CHARC,
    /*特殊符号 */
    ASSIGN, EQ, LT, PLUS, MINUS,
    TIMES, OVER, LPAREN, RPAREN, DOT,
    COLON, SEMI, COMMA, LMIDPAREN, RMIDPAREN,
    UNDERANGE
} LexType;


/*********** 单词的类型，包括词法信息和语义信息 ************/
typedef struct tokenType
{
    int     lineshow;
    LexType Lex;
    char    Sem[MAXTOKENLEN + 1];
} TokenType;


/***********链表的结点类型，包括Token部分和指针部分**********/
typedef struct  node
{
    TokenType   Token;      //单词
    struct node* nextToken; //指向下一个单词的指针
} ChainNodeType;




/******************************************************
 ******************   语法分析树   ********************
 ******************************************************/

 /*语法树根节点ProK,程序头结点PheadK，声明类型节点DecK,
   标志子结点都是类型声明的结点TypeK,标志子结点都是变量声明的结点VarK,
   函数声明结点FuncDecK,语句序列节点StmLK,语句声明结点StmtK,
   表达式结点ExpK*/
typedef enum { ProK, PheadK, DecK, TypeK, VarK, ProcDecK, StmLK, StmtK, ExpK }
NodeKind;


/**
 声明类型Deckind 类型的枚举定义：
 
 数组类型ArrayK,
 
 字符类型CharK,
  
 整数类型IntegerK,
 
 记录类型RecordK,
  
 以类型标识符作为类型的IdK
 */
typedef enum { ArrayK, CharK, IntegerK, RecordK, IdK }  DecKind;



/* 语句类型StmtKind类型的枚举定义:            *
 * 判断类型IfK,循环类型WhileK                *
 * 赋值类型AssignK,读类型ReadK              *
 * 写类型WriteK，函数调用类型CallK          */
typedef enum { IfK, WhileK, AssignK, ReadK, WriteK, CallK, ReturnK } StmtKind;


/* 表达式类型ExpKind类型的枚举定义:         *
 * 操作类型OpK,常数类型ConstK,变量类型VarK */
typedef enum { OpK, ConstK, VariK } ExpKind;


/* 变量类型VarKind类型的枚举定义:           *
 * 标识符IdV,数组成员ArrayMembV,域成员FieldMembV*/
typedef enum { IdV, ArrayMembV, FieldMembV } VarKind;


/* 类型检查ExpType类型的枚举定义:           *
 * 空Void,整数类型Integer,字符类型Char      */
typedef enum { Void, Integer, Boolean } ExpType;

/* 参数类型ParamType类型的枚举定义：        *
 * 值参valparamType,变参varparamType        */
typedef enum { valparamType, varparamType } ParamType;

/* 定义语法树节点的最大子节点数MAXCHILDRREN为3 */
/* 过程声明部分的子节点child[0]指向参数部分，
   子节点child[1]指向声明体部分，子节点child[2]
   指向函数的语句部分；*/
#define MAXCHILDREN 3

   /*提前声明符号表结构*/
struct symbtable;

/**
 @brief语法树节点treeNode类型
 
 -  child[MAXCHILDREN] 子节点指针
 
 -  sibling* 兄弟节点指针
 
 -  lineno 源代码行号
 
 -  nodekind 节点类型
 
 -  kind 具体类型
 
------DecKind dec;
 
------StmtKind stmt;
 
------ ExpKind exp;
 
 - idnum       相同类型的变量个数
 
 - name[10][10]      标识符的名称
 
 - table[10]  与标志符对应的符号表地址，在语义分析阶段填入
 
 -  attr 属性
 
 ----ArrayAttr
 
   ------------low     数组下界
 
   ------------up         数组上界
 
   ------------childtype   数组的子类型
 
 ----ProcAttr     过程属性
 
   ------------ ParamType paramt    过程的参数类型
 
 ----  ExpAttr
 
   ------------ LexType op     表达式的操作符
 
   ------------val       表达式的值
 
   ------------ VarKind varkind  变量的类别
 
   ------------type  用于类型检查
 
 ---- type_name[10]   类型名是标识符
 */
typedef struct treeNode

{
    ///子节点指针
    ///
    ///子节点child[0]指向参数部分
    ///
    ///子节点child[1]指向声明体部分
    ///
    ///子节点child[2]指向函数的语句部分
    struct treeNode* child[MAXCHILDREN];
    struct treeNode* sibling;                    /* 兄弟节点指针    */
    int lineno;                                /* 源代码行号    */
    ///语法树根节点ProK,程序头结点PheadK，声明类型节点DecK,标志子结点都是类型声明的结点TypeK,标志子结点都是变量声明的结点VarK,函数声明结点FuncDecK,语句序列节点StmLK,语句声明结点StmtK,表达式结点ExpK
    NodeKind nodekind;                            /* 节点类型        */
    union
    {
        DecKind  dec;
        StmtKind stmt;
        ExpKind  exp;
    } kind;                       /* 具体类型     */

    int idnum;                    /* 相同类型的变量个数 */

    char name[10][10];            /* 标识符的名称  */

    struct symbtable* table[10]; /* 与标志符对应的符号表地址，在语义分析阶段填入*/

    struct
    {
        struct
        {
            int low;              /* 数组下界     */
            int up;               /* 数组上界     */
            DecKind   childtype;  /* 数组的子类型 */
        }ArrayAttr;               /* 数组属性     */

        struct
        {
            ParamType  paramt;     /* 过程的参数类型*/
        }ProcAttr;                 /* 过程属性      */

        struct
        {
            LexType op;           /* 表达式的操作符*/
            int val;              /* 表达式的值       */
            VarKind  varkind;     /* 变量的类别    */
            ExpType type;         /* 用于类型检查  */
        }ExpAttr;                  /* 表达式属性    */

        char type_name[10];             /* 类型名是标识符  */

    } attr;                          /* 属性           */
}TreeNode;

/*非终极符的总数*/
#define  NTMLNUM    68

/*终极符的总数*/
#define  TMLNUM     42

/*LL1分析表的大小*/
#define  TABLESIZE  104

/******************类型和变量声明*********************/

/* 1.进行LL1语法分析用到的类型及对应的变量  */

/*所有非终极符，其各自含义可参考LL1文法*/
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

/*所有终极符，取自单词的词法类型的枚举定义，可参考zglobals.h头文件*/
typedef LexType  TmlType;


/*实现LL1分析用的分析栈，存放的是终极符和非终极符 */
typedef struct Node
{  /*内容标志*/
    int flag;    /* flag为1，表示栈中内容为非终极符；*/
                 /* flag为2，表示栈中内容为终极符    */
    /*内容*/
    union {
        NontmlType Ntmlvar;  /*非终极符部分*/
        TmlType tmlvar;       /*终极符部分*/
    } var;

    /*指向下一个节点的指针*/
    struct Node* underNode;

}  StackNode;


/* 2.创建语法树所需的类型及变量**/

/*为建立声明和语句部分的语法树所设的指针栈，*
 *注意栈中存的是存放指针的地址              */
typedef struct NodePA
{
    TreeNode** pointer;
    struct NodePA* underNode;
}  StackNodePA;


/*为生成表达式部分的语法树所设的指针栈，*
 *栈中存的是存放指向树节点的指针        */
typedef struct NodeP
{
    TreeNode* pointer;
    struct NodeP* underNode;
}StackNodeP;

/*符号栈顶指针*/
extern StackNode* StackTop;

/*栈空标志*/
extern  int STACKEMPTY;

/*语法树栈顶指针*/
extern  StackNodePA* StackTopPA;
/*栈空标志*/
extern  int  paSTACKEMPTY;

/*操作符栈的栈顶指针*/
extern  StackNodeP* OpStackTop;
/*操作符栈空标志*/
extern  int OpSTACKEMPTY;

/*操作数栈的栈顶指针*/
extern  StackNodeP* NumStackTop;

/*操作数栈空标志*/
extern  int NumSTACKEMPTY;




//MARK:- 语义分析的类型及变量定义

struct typeIR;

/// 标识符的类型
///
/// 一个标识符的类别只可能 为 typeKind, varKind, procKind 其中之一
typedef  enum {typeKind, varKind, procKind }IdKind;

///变量的类别。dir表直接变量(值参)，indir表示间接变量(变参)
typedef  enum { dir, indir }AccessKind;

//MARK:形参表ParamTable的结构定义
///形参表的结构定义
///
///entry：symbtable*，指向该形参所在符号表中的地址入口
///
///next：paramTable* 形参表的下一项
typedef struct  paramTable
{
    ///指向该形参所在符号表中的地址入口
    struct symbtable* entry;
    struct paramTable* next;
}ParamTable;

//MARK: 标识符信息项AttributeIR
///标识符信息项
///
///标识符的内部表示中涉及到层数、偏移量、过程的存储大小和目标代码入口地址等内容
typedef struct
{
    ///指向标识符的类型内部表示
    struct typeIR* idtype;
    ///标识符的类型
    IdKind    kind;
    
    ///标识符的不同类型有不同的属性
    union
    {
        ///变量标识符的属性
        struct
        {
            ///变参或值参
            AccessKind   access;
            int          level;
            int          off;
            
            //TODO: added
            ///参数或普通变量
            bool         isParam;
        }VarAttr;
        
        ///过程名标识符的属性
        struct
        {
            ///该过程的层数
            int         level;
            ///参数表
            ParamTable* param;
            
            //FIXME: deleted
//            int code;
//            int size;
            
            //TODO: added
            ///过程活动记录的大小
            int         mOff;
            //sp到display表的偏移量
            int         nOff;
            ///过程的入口地址
            int         procEntry;
            ///过程入口标号,用于中间代码生成
            int         codeEntry;
        }ProcAttr;

    }More;

}AttributeIR;


// MARK: 符号表SymbTable数据结构定义
/**
 符号表的数据结构定义
 
 idName: 标识符名
 
 attrIR: 标识符信息项
 
 next: 符号表的下一个元素
 */
typedef struct  symbtable
{
    ///标识符名
    char  idName[10];
    ///标识符信息项
    AttributeIR  attrIR;
    ///符号表的下一个元素
    struct symbtable* next;
}SymbTable;

///使用scope栈的局部符号表方法中所用到的scope栈
extern SymbTable* scope[1000];

///scope栈的层数
extern int Level;

///在同层的变量偏移
extern int Off;

///记录主程序的displayOff
extern int mainOff;

///记录当前层的displayOff
extern int savedOff;



//MARK: - 类型内部表示

///类型的种类的枚举定义
///
///intTy, charTy, arrayTy, recordTy, boolTy
///
///SNL 的类型包括：整数类型，字符类型，数组类型，记录类型，布尔类型（其 中布尔类型只在判断条件表达式的值时使用）。其中整型和字符类型是标准类型，其 内部表示可以事先构造，数组和记录类型等构造类型则要在变量声明或类型声明时 构造。
typedef  enum { intTy, charTy, arrayTy, recordTy, boolTy }TypeKind;


struct typeIR;

/**
 域类型单元结构定义
 
 id[10]：char   变量名

 off：int 所在记录中的偏移
 
 UnitType：typeIR*  域中成员的类型
 
 Next：fieldchain*   下一项
 */
typedef struct fieldchain
{
    ///变量名
    char   id[10];
    ///所在记录中的偏移
    int    off;
    ///域中成员的类型
    struct typeIR* UnitType;
    ///下一项
    struct fieldchain* Next;
}fieldChain;


/**
 @brief类型的内部结构定义

 size  类型所占空间大小
 
 kind 节点类型
 
 more
 
 --ArrayAttr

 -----indexTy       指向数组下标类型的内部表示

 -----elemTy        指向数组元素类型的内部表示，即指向证书或字符

 -----iow       记录数组类型的下界

 -----up        记录数组类型的上界
 
 --body     记录类型中的域链
 */
typedef   struct  typeIR
{
    ///类型所占空间大小
    int             size;
    ///intTy, charTy, arrayTy, recordTy, boolTy
    TypeKind        kind;
    union
    {
        ///数组类型的内部表示的额外内容
        struct
        {
            ///指向数组下标类型的内部表示
            struct typeIR* indexTy;
            ///指向数组元素类型的内部表示，即指向证书或字符
            struct typeIR* elemTy;
            ///记录数组类型的下界
            int    low;
            ///记录数组类型的上界
            int    up;
        }ArrayAttr;
        ///记录类型中的域链
        fieldChain* body;  
    } More;
}TypeIR;






//MARK: - 文件指针
/* 源代码文本文件指针source */
extern FILE* source;

/* 列表输出文件指针listing */
extern FILE* listing;

/*词法分析结果Token序列的存储文件指针fp*/
extern FILE* fp;

/*目标代码文件指针*/
//extern FILE* code;

/*Token序列中的token数目*/
extern int Tokennum;

/*清单的行号*/
extern int lineno;

/*scope栈的层数*/
extern int Level;

/*在同层的变量偏移*/
extern int Off;

/*记录主程序的displayOff*/
//extern int mainOff;

/*记录当前层的displayOff*/
//extern int savedOff;



// MARK: - 追踪标志
 /* 源程序追踪标志,如果该标志为TRUE             *
  * 语法分析时将源程序行号写入列表文件listing */
extern int EchoSource;

/* 词法扫描追踪标志,如果该标志为TRUE                   *
 * 将每个被词法扫描器识别的单词信息写入列表文件listing */
extern int TraceScan;

/* 语法分析追踪标志,如果该标志为TRUE                     *
 * 将语法树以线性形式(使用子节点缩进)写入列表文件listing */
extern int TraceParse;

/* 语义分析追踪标志,如果该标志为TRUE     *
 * 将符号表插入和查找写入列表文件listing */
extern int TraceTable;


extern int TraceCode;

/* 错误追踪标志,如果该标志为TRUE *
 * 防止错误产生时进一步传递错误  */
extern int Error;

#endif

