
/****************************************************/
/* 文件	scanner.cpp								*/
/* 说明 TINY编译器的词法扫描器实现					*/
/* 主题 编译器结构:原理和实例						*/
/****************************************************/

/************ 该代码文件所包含的头文件 **************/

#include "globals.h"	/* 该头文件globals.h定义了全局类与变量 */

#include "string.h"

#include "ctype.h"      /* 用到了该库中的isalnum,isalpha,isdigit函数 */

#include "util.h"		/* 该头文件util.h定义了相关功能函数 */

#include "math.h"       /* 用到了这个库里的取模运算 */

/*****************词法分析器确定性有限自动机DFA的状态类型*************/
/* START 开始状态; INASSIGN 赋值状态; INRANGE 下标范围状态;          */
/* INNUM 数字状态; INID 标识符状态; DONE 完成状态;                   */
/* INCHAR 字符状态;INCOMMENT 注释状态;                               */
typedef enum
{
    START, INASSIGN, INRANGE, INCOMMENT, INNUM, INID, INCHAR, DONE
}
StateType;


/* tokenString用于保存标识符和保留字单词的词元,长度41 */
char tokenString[MAXTOKENLEN + 1];


/* BUFLEN源代码行的输入缓冲区长度为256 */
#define BUFLEN 256


/* lineBuf为当前输入代码行缓冲区 */
static char lineBuf[BUFLEN];


/* linepos为在代码缓冲区LineBuf中的当前字符位置,初始为0 */
static int linepos = 0;


/* bufsize为当前缓冲器中所存字串大小 */
static int bufsize = 0;


/* EOF_flag当为文件尾时,改变函数ungetNextChar功能 */
static int EOF_flag = FALSE;




/*******************************************************************/
/* 函数名 getNextChar											   */
/* 功  能 取得下一非空字符函数									   */
/* 说  明 该函数从输入缓冲区lineBuf中取得下一个非空字符		       */
/*        如果lineBuf中的字串已经读完,则从源代码文件中读入一新行   */
/*******************************************************************/
static int getNextChar(void)
{
    /* 当前代码输入行缓冲器lineBuf已经耗尽 */
    if (!(linepos < bufsize))

    {
        /* 源代码行号lineno加1 */
        lineno++;

        /* 从源文件source中读入BUFLEN-2(254)个字符到行缓冲区lineBuf中      *
         * fgets在的lineBuf末尾保留换行符.并在末尾加了一个NULL字符表示结束 */
        if (fgets(lineBuf, BUFLEN - 1, source))

        {
            /* 如果源文件追踪标志EchoSource为TRUE                               *
             * 将源程序行号lineno及行内容lineBuf在词法扫描时写入列表文件listing */
            if (EchoSource) fprintf(listing, "%4d: %s", lineno, lineBuf);

            /* 取得当前输入源代码行的实际长度,送给变量bufsize */
            bufsize = strlen(lineBuf);

            /* 输入行缓冲区lineBuf中当前字符位置linepos指向lineBuf开始位置 */
            linepos = 0;

            /* 取得输入行缓冲区lineBuf中下一字符 */
            return lineBuf[linepos++];

        }
        else
        {
            /* 未能成功读入新的代码行,fget函数返回值为NULL *
             * 已经到源代码文件末尾,设置EOF_flag标志为TRUE */
            EOF_flag = TRUE;

            /* 函数返回EOF */
            return EOF;
        }
    }

    /* 行输入缓冲区lineBuf中字符还未读完,直接取其中下一字符,函数返回所取字符 */
    else return lineBuf[linepos++];
}


/********************************************************/
/* 函数名 ungetNextChar									*/
/* 功  能 字符回退函数									*/
/* 说  明 该过程在行输入缓冲区lineBuf中回退一个字符		*/
/*        用于超前读字符后不匹配时候的回退				*/
/********************************************************/
static void ungetNextChar(void)

{
    /* 如果EOF_flag标志为FALSE,不是处于源文件末尾  *
     * 输入行缓冲区lineBuf中当前字符位置linepos减1 */
    if (!EOF_flag) linepos--;
}


/******************* 保留字查找表 ********************/

static struct
{
    char* str;
    LexType tok;
} reservedWords[MAXRESERVED]
= { {"program",PROGRAM},{"type",TYPE},{"var",VAR},{"procedure",PROCEDURE}
   ,{"begin",BEGIN},{"end",END},{"array",ARRAY},{"of",OF},{"record",RECORD}
   ,{"if",IF},{"then",THEN},{"else",ELSE},{"fi",FI},{"while",WHILE}
   ,{"do",DO},{"endwh",ENDWH},{"read",READ},{"write",WRITE},{"return",RETURN}
   ,{"integer",INTEGER},{"char",CHAR} };



/**************************************************************/
/* 函数名 reservedLookup								      */
/* 功  能 保留字查找函数									  */
/* 说  明 使用线性查找,查看一个标识符是否是保留字			  */
/*		  标识符如果在保留字表中则返回相应单词,否则返回单词ID */
/**************************************************************/
static LexType  reservedLookup(char* s)

{
    int i;

    /* 在保留字表中查找,MAXRESERVED已经定义为8,为保留字数 */
    for (i = 0; i < MAXRESERVED; i++)

        /* 线性查保留字表,察看函数参数s指定标识符是否在表中 *
         * 当两字符串匹配的时候,函数strcmp返回值为0(FALSE)	*/
        if (!strcmp(s, reservedWords[i].str))

            /* 字符串s与保留字表中某一表项匹配,函数返回对应保留字单词 */
            return reservedWords[i].tok;

    /* 字符串s未在保留字表中找到,函数返回标识符单词ID */
    return ID;
}


/****************************************
 ********* 词法扫描器基本函数  **********
 ****************************************/

 /************************************************************/
 /* 函数名 getTokenlist						   			    */
 /* 功  能 取得单词函数										*/
 /* 说  明 函数从源文件字符串序列中获取所有Token序列 		*/
 /*        使用确定性有限自动机DFA,采用直接转向法    		*/
 /*        超前读字符,对保留字采用查表方式识别    			*/
 /*        产生词法错误时候,仅仅略过产生错误的字符,不加改正  */
 /************************************************************/

void getTokenlist(void)

{
    ChainNodeType* chainHead;    /*链表的表头指针*/
    ChainNodeType* currentNode;  /*指向处理当前Token的当前结点*/
    ChainNodeType* preNode;      /*指向当前结点的前驱结点*/
    ChainNodeType* tempNode;     /*临时指针，用于释放链表部分*/
    TokenType currentToken;      /*存放当前的Token*/

    /*产生链表的第一个结点*/
    chainHead = preNode = currentNode = (ChainNodeType*)malloc(CHAINNODELEN);
    /*初始化当前结点中，指向下一个结点的指针为空*/
    (*currentNode).nextToken = NULL;

    do
    {  /* tokenStringIndex用于记录当前正在识别单词的词元存储区 *
        * tokenString中的当前正在识别字符位置,初始为0          */
        int tokenStringIndex = 0;

        /* 当前状态标志state,始终都是以START作为开始 */
        StateType state = START;

        /* tokenString的存储标志save,整数类型						*
         * 决定当前识别字符是否存入当前识别单词词元存储区tokenString */
        int save;

        /* 当前确定性有限自动机DFA状态state不是完成状态DONE */
        while (state != DONE)

        {
            /* 从源代码文件中获取下一个字符,送入变量c作为当前字符 */
            int c = getNextChar();

            /* 当前正识别字符的存储标志save初始为TRUE */
            save = TRUE;

            switch (state)
            {
                /* 当前DFA状态state为开始状态START,DFA处于当前单词开始位置 */
            case START:

                /* 当前字符c为数字,当前DFA状态state设置为数字状态INNUM *
                 * 确定性有限自动机DFA处于数字类型单词中               */
                if (isdigit(c))
                    state = INNUM;

                /* 当前字符c为字母,当前DFA状态state设置为标识符状态INID *
                 * 确定性有限自动机DFA处于标识符类型单词中              */
                else if (isalpha(c))
                    state = INID;

                /* 当前字符c为冒号,当前DFA状态state设置为赋值状态INASSIGN *
                 * 确定性有限自动机DFA处于赋值类型单词中				   */
                else if (c == ':')
                    state = INASSIGN;

                /* 当前字符c为.,当前DFA状态state设置为数组下标界限状态*/
                /* INRANGE，确定性有限自动机DFA处于数组下标界限类型单词中*/
                else if (c == '.')
                    state = INRANGE;


                /* 当前字符c为',当前DFA状态state设置为字符标志状态*/
                /* INCHAR，确定性有限自动机DFA处于字符标志类型单词中*/
                else if (c == '\'')
                {
                    save = FALSE;
                    state = INCHAR;
                }

                /* 当前字符c为空白(空格,制表符,换行符),字符存储标志save设置为FALSE *
                 * 当前字符为分隔符,不需要产生单词,无须存储                        */
                else if ((c == ' ') || (c == '\t') || (c == '\n')|| (c == '\r'))
                    save = FALSE;

                /* 当前字符c为左括号,字符存储标志save设置为FALSE     *
                 * 当前DFA状态state设置为注释状态INCOMMENT			  *
                 * 确定性有限自动机DFA处于注释中,不生成单词,无需存储 */
                else if (c == '{')
                {
                    save = FALSE;
                    state = INCOMMENT;
                }

                /* 当前字符c为其它字符,当前DFA状态state设置为完成状态DONE *
                 * 确定性有限自动机DFA处于单词的结束位置,需进一步分类处理 */
                else

                {
                    state = DONE;
                    switch (c)
                    {
                        /* 当前字符c为EOF,字符存储标志save设置为FALSE,无需存储     *
                         * 当前识别单词返回值currentToken设置为文件结束单词ENDFILE */
                    case EOF:
                        save = FALSE;
                        currentToken.Lex = ENDFILE;
                        break;

                        /* 当前字符c为"=",当前识别单词返回值currentToken设置为等号单词EQ */
                    case '=':
                        currentToken.Lex = EQ;
                        break;

                        /* 当前字符c为"<",当前识别单词返回值currentToken设置为小于单词LT */
                    case '<':
                        currentToken.Lex = LT;
                        break;

                        /* 当前字符c为"+",当前识别单词返回值currentToken设置为加号单词PLUS */
                    case '+':
                        currentToken.Lex = PLUS;
                        break;

                        /* 当前字符c为"-",当前识别单词返回值currentToken设置为减号单词MINUS */
                    case '-':
                        currentToken.Lex = MINUS;
                        break;

                        /* 当前字符c为"*",当前识别单词返回值currentToken设置为乘号单词TIMES */
                    case '*':
                        currentToken.Lex = TIMES;
                        break;

                        /* 当前字符c为"/",当前识别单词返回值currentToken设置为除号单词OVER */
                    case '/':
                        currentToken.Lex = OVER;
                        break;

                        /* 当前字符c为"(",当前识别单词返回值currentToken设置为左括号单词LPAREN */
                    case '(':
                        currentToken.Lex = LPAREN;
                        break;

                        /* 当前字符c为")",当前识别单词返回值currentToken设置为右括号单词RPAREN */
                    case ')':
                        currentToken.Lex = RPAREN;
                        break;

                        /* 当前字符c为";",当前识别单词返回值currentToken设置为分号单词SEMI */
                    case ';':
                        currentToken.Lex = SEMI;
                        break;
                        /* 当前字符c为",",当前识别单词返回值currentToken设置为逗号单词COMMA */
                    case ',':
                        currentToken.Lex = COMMA;
                        break;
                        /* 当前字符c为"[",当前识别单词返回值currentToken设置为左中括号单词LMIDPAREN */
                    case '[':
                        currentToken.Lex = LMIDPAREN;
                        break;

                        /* 当前字符c为"]",当前识别单词返回值currentToken设置为右中括号单词RMIDPAREN */
                    case ']':
                        currentToken.Lex = RMIDPAREN;
                        break;

                        /* 当前字符c为其它字符,当前识别单词返回值currentToken设置为错误单词ERROR */
                    default:
                        currentToken.Lex = ERROR;
                        Error = TRUE;
                        break;
                    }
                }
                break;
                /********** 当前状态为开始状态START的处理结束 **********/

                /* 当前DFA状态state为注释状态INCOMMENT,确定性有限自动机DFA处于注释位置 */
            case INCOMMENT:

                /* 当前字符存储状态save设置为FALSE,注释中内容不生成单词,无需存储 */
                save = FALSE;

                /* 当前字符c为EOF,当前DFA状态state设置为完成状态DONE,当前单词识别结束 *
                 * 当前识别单词返回值currentToken设置为文件结束单词ENDFILE            */
                if (c == EOF)
                {
                    state = DONE;
                    currentToken.Lex = ENDFILE;

                }

                /* 当前字符c为"}",注释结束.当前DFA状态state设置为开始状态START */
                else if (c == '}') state = START;
                break;

                /* 当前DFA状态state为赋值状态INASSIGN,确定性有限自动机DFA处于赋值单词位置 */
            case INASSIGN:

                /* 当前DFA状态state设置为完成状态DONE,赋值单词结束 */
                state = DONE;

                /* 当前字符c为"=",当前识别单词返回值currentToken设置为赋值单词ASSIGN */
                if (c == '=')
                    currentToken.Lex = ASSIGN;

                /* 当前字符c为其它字符,即":"后不是"=",在输入行缓冲区中回退一个字符       *
                 * 字符存储状态save设置为FALSE,当前识别单词返回值currentToken设置为ERROR */
                else
                {
                    ungetNextChar();
                    save = FALSE;
                    currentToken.Lex = ERROR;
                    Error = TRUE;
                }
                break;

            case INRANGE:

                /* 当前DFA状态state设置为完成状态DONE,赋值单词结束 */
                state = DONE;

                /* 当前字符c为".",当前识别单词返回值currentToken设置为下标界UNDERANGE */
                if (c == '.')
                    currentToken.Lex = UNDERANGE;

                /* 当前字符c为其它字符,即"."后不是".",在输入行缓冲区中回退一个字符       *
                 * 字符存储状态save设置为FALSE,当前识别单词返回值currentToken设置为ERROR */
                else
                {
                    ungetNextChar();
                    save = FALSE;
                    currentToken.Lex = DOT;
                }
                break;

                /* 当前DFA状态state为数字状态INNUM,确定性有限自动机处于数字单词位置 */
            case INNUM:

                /* 当前字符c不是数字,则在输入行缓冲区源中回退一个字符					*
                 * 字符存储标志设置为FALSE,当前DFA状态state设置为DONE,数字单词识别完成 *
                 * 当前识别单词返回值currentToken设置为数字单词NUM                     */
                if (!isdigit(c))
                {
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    currentToken.Lex = INTC;
                }
                break;
                /*当前DFA状态state为字符标志状态INCHAR,确定有限自动机处于字符标志状态*/
            case INCHAR:

                if (isalnum(c))

                {
                    int c1 = getNextChar();
                    if (c1 == '\'')
                    {
                        save = TRUE;
                        state = DONE;
                        currentToken.Lex = CHARC;
                    }
                    else
                    {
                        ungetNextChar();
                        ungetNextChar();
                        state = DONE;
                        currentToken.Lex = ERROR;
                        Error = TRUE;
                    }
                }
                else
                {
                    ungetNextChar();
                    state = DONE;
                    currentToken.Lex = ERROR;
                    Error = TRUE;
                }
                break;
                /* 当前DFA状态state为标识符状态INID,确定性有限自动机DFA处于标识符单词位置 */
            case INID:

                /* 当前字符c不是字母,则在输入行缓冲区源中回退一个字符		 			  *
                 * 字符存储标志设置为FALSE,当前DFA状态state设置为DONE,标识符单词识别完成 *
                 * 当前识别单词返回值currentToken设置为标识符单词ID                      */
                if (!isalnum(c))
                {
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    currentToken.Lex = ID;
                }
                break;

                /* 当前DFA状态state为完成状态DONE,确定性有限自动机DFA处于单词结束位置 */
            case DONE:	break;

                /* 当前DFA状态state为其它状态,此种情况不应发生 */
            default:

                /* 将词法扫描器产生错误的状态state写入列表文件listing	*
                 * 当前DFA状态state设置为完成状态DONE					*
                 * 当前识别单词返回值currentToken设置为错误单词ERROR	*/
                fprintf(listing, "Scanner Bug: state= %d\n", state);
                Error = TRUE;
                state = DONE;
                currentToken.Lex = ERROR;
                break;

            }
            /*************** 分类判断处理结束 *******************/

            /* 当前字符存储状态save为TRUE,且当前正识别单词已经识别部分未超过单词最大长度 *
             * 将当前字符c写入当前正识别单词词元存储区tokenString						  */
            if ((save) && (tokenStringIndex <= MAXTOKENLEN))
                tokenString[tokenStringIndex++] = (char)c;

            if (state == DONE)
            {
                /* 当前DFA状态state为完成状态DONE,单词识别完成   *
                 * 当前识别单词词元存储区tokenString加上结束标志 */
                tokenString[tokenStringIndex] = '\0';

                /* 当前单词currentToken为标识符单词类型,查看其是否为保留字单词 */
                if (currentToken.Lex == ID)
                {
                    currentToken.Lex = reservedLookup(tokenString);
                    if (currentToken.Lex != ID)
                        strcpy(tokenString, tokenString);
                }
            }
        }
        /**************** 循环处理结束 ********************/
        /*将行号信息存入Token*/
        currentToken.lineshow = lineno;
        /*将单词的语义信息存入Token*/
        strcpy(currentToken.Sem, tokenString);

        /*将已处理完的当前Token存入链表的Token部分*/
        ((*currentNode).Token).lineshow = currentToken.lineshow;
        ((*currentNode).Token).Lex = currentToken.Lex;
        strcpy(((*currentNode).Token).Sem, currentToken.Sem);

        Tokennum++;   /*Token总数目加1*/

        /*若不是第一个结点，则将当前结点连入链表*/
        if (preNode != currentNode)
        {
            preNode->nextToken = currentNode;
            preNode = currentNode;
        }
        /*申请一个新的结点，以记录下一个Token的信息*/
        currentNode = (ChainNodeType*)malloc(CHAINNODELEN);
        /*初始化这个结点中，指向下一个结点的指针为空*/
        currentNode->nextToken = NULL;
    }
    /* 直到处理完表示文件结束的Token:ENDFILE，说明处理完所有的Token*/
    /* 并存入了链表中，循环结束*/
    while ((currentToken.Lex) != ENDFILE);
    /*将由chainHead指向的Token链表存入文件"Tokenlist"中*/
    ChainToFile(chainHead);

    /*释放链表*/
    while (chainHead != NULL)
    {
        tempNode = chainHead->nextToken;
        free(chainHead);
        chainHead = tempNode;
    }

}
/* 词法分析函数结束 */










