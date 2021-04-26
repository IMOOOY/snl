#include "globals.h"
#include "scanner.h"
#include "util.h"
#include "symbTable.h"
#include "string.h"
#include "parseLL1.h"
#include "parse.h"
#include "analyze.h"
#include <cstdio>
#include "globals.h"
#include "scanner.h"
#include "util.h"
#include "parseLL1.h"
#include "analyze.h"
#include "symbTable.h"
#define TURE 1;

FILE* source;        /* 源程序文本文件,为编译器输入文件 */

FILE* listing;        /* 中间列表文件,为编译器中间信息输出文件 */

/*指针fp,指向存放Token序列的文件"Tokenlist"*/
FILE* fp;

/*Token序列中的token数目*/
int Tokennum = 0;

/*清单的行号*/
int lineno = 0;

/*记录当前层的displayOff*/
int savedOff = 0;

/*保存主程序的display表的偏移*/
int StoreNoff;

/*目标代码文件*/
FILE* code;

int EchoSource = TRUE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int  TraceTable = TRUE;
int TraceCode = TRUE;
int  Error = FALSE;

int main()

{
    /*输入要编译的文件名*/
    char pgm[120];

    /*目标代码文件名*/
    char* codefile=NULL;

    printf( "===================================================================\n");
    printf("输入待编译文件名");
    printf( "\n===================================================================\n");
    scanf("%s", pgm);

    /* 若未指定文件扩展名,给源文件加上扩展名.txt */
    if (strchr(pgm, '.') == NULL)
        strcat(pgm, ".txt");

    /* 用只读方式打开源程序文件pgm,文件指针给source */
    source = fopen(pgm, "r");

    if (source == NULL)
    {
        fprintf(stderr, "File %s not found\n", pgm);
        exit(1);
    }

    /* 将中间信息列表文件listing指向屏幕标准输出stdout */
    listing = stdout;

   /* 输出中间编译信息 */
    fprintf(listing, "\n\n===================================================================\n");
    fprintf(listing, "对 ｜%s｜ 进行编译", pgm);
    fprintf(listing, "\n===================================================================\n");

    /* 调用词法分析函数，得到Token序列*/
    getTokenlist();

    fprintf(listing, "\n===================================================================\n");
    fprintf(listing, "回车进行下一步");
    fprintf(listing, "\n===================================================================\n");
    if (EchoSource)
    { /*按键进入下一部分*/
        getchar();

    }

    /* 如果词法分析追踪标志为TRUE,则将生成的Token序列输出到屏幕 */
    if (TraceScan)
    {
        getchar();
        fprintf(listing, "\n\n\n===================================================================\n");
        fprintf(listing, "词法分析:TOKEN序列如下");
        fprintf(listing, "\n===================================================================\n");
        printTokenlist();
    }
    
    fprintf(listing, "\n===================================================================\n");
    fprintf(listing, "回车进行下一步");
    fprintf(listing, "\n===================================================================\n");

    
    



    printf( "\n\n\n===================================================================\n");
    printf("选择语法分析方式\n");
    printf("* 1-LL1\n");
    printf("* 2-递归下降语法");
    printf( "\n===================================================================\n");
    int ll1;
    scanf("%d", &ll1);
    while(ll1!=1&&ll1!=2)
    {
        printf( "\n\n\n!!!!!!!!!!!!!!!!!!!!!!!!!==错误输入==!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
        printf("\n\n");
        
        printf( "===================================================================\n");
        printf("选择语法分析方式\n");
        printf("* 1-LL1\n");
        printf("* 2-递归下降语法");
        printf( "\n===================================================================\n");
        scanf("%d", &ll1);
    }
    TreeNode* syntaxTree;
    if(ll1==1)
    {
        /* LL1语法分析                  *
        /* 调用语法分析函数，生成语法分析树 */
        
        fprintf(listing, "\n\n\n===================================================================\n");
        syntaxTree = parseLL1();

        /* 如果语法分析追踪标志为TRUE且没有语法错误,
           则将生成的语法树输出到屏幕 */
        if ((TraceParse) && (!Error))
        {
            fprintf(listing, "LL1语法分析：语法树如下");
            fprintf(listing, "\n===================================================================\n");
            printTree(syntaxTree);
            getchar();
//            getchar();
        }
        else
        {
            fprintf(listing, "\n\n>>>LL1语法分析：存在上述语法错误");
            fprintf(listing, "\n===================================================================\n");
        }

    }
    else
    {
        /* 递归下降语法解析                *
         * 调用语法分析函数,生成语法分析树    */
        
        fprintf(listing, "\n\n\n===================================================================\n");
        syntaxTree = parse();

        /* 如果语法分析追踪标志为TRUE且没有语法错误,
           则将生成的语法树输出到屏幕 */
        if ((TraceParse) && (!Error))
        {
            fprintf(listing, "递归下降语法分析：语法树如下");
            fprintf(listing, "\n===================================================================\n");
            printTree(syntaxTree);
            getchar();
//            getchar();
        }
        else
        {
            fprintf(listing, "\n\n>>>LL1语法分析：存在上述语法错误");
            fprintf(listing, "\n===================================================================\n");
        }
    }
    if(!ERROR)
    {
        fprintf(listing, "\n===================================================================\n");
        fprintf(listing, "回车进行下一步");
        fprintf(listing, "\n===================================================================\n");
        getchar();
        {
            fprintf(listing, "\n\n\n===================================================================\n");
            fprintf(listing, "语义分析：\n");
            fprintf(listing, "===================================================================\n");
            fprintf(listing, "语义错误信息：\n");
            /*语义分析*/
            analyze(syntaxTree);
            if (!Error)
            {
                fprintf(listing, ">>>...\n");
                fprintf(listing, ">>>无语义错误!\n");
                fprintf(listing, "===================================================================\n");
            }
     

            /*输出符号表*/
            if ((TraceTable) && (!Error))
            {
                fprintf(listing, "\n\n\n===================================================================\n");
                fprintf(listing, "符号表如下：");
                fprintf(listing, "\n===================================================================\n");
                PrintSymbTable();
    //            getchar();
            }
        }
    }


    fprintf(listing, "\n===================================================================\n");
    fprintf(listing, "编译结束，回车退出程序");
    fprintf(listing, "\n===================================================================\n");

    getchar();
    fprintf(listing, ">>语法树空间释放.........\n");
//    getchar();

    /*释放语法树空间*/
    if (syntaxTree != NULL)
    {
        freeTree(syntaxTree);
//        fprintf(listing, ">>........\n");
    }
    /*释放符号表空间*/
    freeTable();
    fprintf(listing, ">>符号表空间释放.........\n");
    fprintf(listing, "===================================================================\n");

    
    /* 编译结束,关闭源程序文件source */
    fclose(source);

    /* 编译成功,程序正常返回 */
    return 0;
}


