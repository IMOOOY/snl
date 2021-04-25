//
//  symTable.cpp
//  snl
//
//  Created by IMOOOY on 2021/4/10.
//

#include "globals.h"
#include "stdio.h"
#include "string.h"
#include "util.h"



static void printTy(TypeIR* ty);
static void printVar(SymbTable* entry);
static void printProc(SymbTable* entry);
//static void printTab(int tabnum);


//MARK: - 符号表
void CreatTable(void);
void  DestroyTable();
int  Enter(char* id, AttributeIR* attribP, SymbTable** entry);
void  PrintOneLayer(int level);


SymbTable* NewTable(void);

/**
 创建空符号表
 
 当进入一个新的局部化单位时，调用本子程序。
 
 功能是建立一个空符号表 table，层数加 1，偏移初始化为 0
 */
void CreatTable(void)
{
    Level = Level + 1;                 /*层数加一*/
    scope[Level] = NULL;              /*申请了新的一层scope栈的空间*/
    Off = INITOFF;                      /*偏移初始化*/
}

/**
 撤销当前符号表
 
 退出一个局部化区时，调用本子程序。
 
 功能是层数减1并撤销当前符号表
*/
void  DestroyTable()
{
    
    Level = Level - 1;
}



/**
 登记标识符和属性到符号表
 
 将把给定id和属性attribP登记到符号表中
 @param id char* 标识符名
 @param attribP AttributeIR*  标识符属性
 @param entry SymbTable**   指向登记的标识符在符号表的位置
 @return 登记项的地址，在登记时检查在本层中是否有重复声明错误，如果已有id项则该变量返回1值，否则返回0。
 */
int  Enter(char* id, AttributeIR* attribP, SymbTable** entry)
{
    int present = FALSE;// 本层中是否有重复声明
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
        //遍历当前符号表，查询本层是否有重复定义
        {
            prentry = curentry;
            result = strcmp(id, curentry->idName);
            if (result == 0)
            {
//                fprintf(listing, "标识符 %s 重复声明!\n\n",id);
                Error = TRUE;
                present = TRUE;
//                exit(0);
            }
            curentry = (prentry->next);
        }

        if (present == FALSE)
        {
            curentry = NewTable();
            //建立新符号表项
            prentry->next = curentry;
        }
        else
            return present;
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

/// 创建当前空符号表
///
/// 遇到新的无声明的标识符时创建新的空符号表
/// @return 返回新符号表的指针，未创建成功则返回NULL
SymbTable* NewTable(void)
{

    SymbTable* table = (SymbTable*)malloc(sizeof(SymbTable));

    if (table == NULL)
    {
        fprintf(listing, "内存溢出!");
        Error = TRUE;
        return table;
    }
    table->next = NULL;
    table->attrIR.kind = typeKind;
    table->attrIR.idtype = NULL;
    table->next = NULL;
    table->attrIR.More.VarAttr.isParam = false;
    return table;
}


/// 查标识符的符号表
/// @param id 查找标识符id的表项地址
/// @param entry 返回符号表表项地址。
/// @return 如果符号表里没有所找的id项,则返回present为0,则函数中的参数entry赋值为指向该表项地址的指针;否则,present赋值为1。
int FindEntry(char* id, SymbTable** entry)
{
    int present = FALSE;    /*返回值，是否找到*/
    int same = 1;         /*标识符名字比较结果*/
    int lev = Level;        /*临时记录层数的变量*/

    SymbTable* findentry = scope[lev];  //在当前层查找

    while ((lev != -1) && (present != TRUE))
    {
        while ((findentry != NULL) && (present != TRUE))
        {
            same = strcmp(id, findentry->idName);
            if (same == 0)
                present = TRUE;
            /*如果标识符名字相同，则返回TRUE*/
            else
                findentry = findentry->next;
            /*如果没找到，则继续链表中的查找*/
        }
        
        //本层中没有查到，转到上一个局部化区域中继续查找
        if (present != TRUE)
        {
            lev = lev - 1;
            findentry = scope[lev];
        }
    }
    
    //整个程序中未找到
    if (present != TRUE)
    {
        (*entry) = NULL;
    }
    else
        (*entry) = findentry;

    return present;
}



/// 在域表中查找域名
/// @param Id 要查找的标识符
/// @param head 指向表头
/// @param Entry 返回值，此域名在记录的域表中的位置
/// @return bool类型，是否找到
bool  FindField(char* Id, fieldChain* head, fieldChain** Entry)
{
    bool  present = false;
    fieldChain* currentItem = head;//记录当前节点

    while ((currentItem != NULL) && (present == false))
    //遍历
    {
        if (strcmp(currentItem->id, Id) == 0)
        {
            present = true;
            if (Entry != NULL)
            {
                (*Entry) = currentItem;
                //FIXME: 加break
                break;
            }
        }
        else  currentItem = currentItem->Next;
    }

    return(present);
}


/// 打印符号表的一层
/// @param level 要打印的层数
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
            case  charTy:    fprintf(listing, "charTy  ");  break;
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
/// 打印生成的符号表
void   PrintSymbTable()
{ /*层数从0开始*/
    int  level = 0;
    while (scope[level] != NULL)
    {
        PrintOneLayer(level);
        level++;
    }
}








//MARK: -

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
/* 函数名 NewTy                                               */
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
/* 函数名 NewParam                                           */
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


/// 错误提示
///
///在输出文件中显示错误提示，并给全局量Error赋值为1
/// @param line 错误所在行号
/// @param name 标识符
/// @param message 错误信息
void ErrorPrompt(int line, char* name, char* message)
{
    fprintf(listing, ">>>行 %d:, %s %s", line, name, message);
    Error = TRUE;
    fprintf(listing, "===================================================================\n");
    fprintf(listing, "编译结束\n");
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
