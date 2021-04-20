/****************************************************/
/* 文件 main.cpp									*/
/* 说明 类PASCAL语言编译器主程序					*/
/* 主题 编译器结构:原理和实例						*/
/****************************************************/



/* 头文件globals.h定义了全局类型与变量 */
#include "globals.h"

/*头文件scan.h定义了词法分析程序的界面*/
#include "scanner.h"  

/*头文件util.h定义了语法分析程序的界面*/
#include "util.h"

#include "symbTable.h"


#include "string.h"

#define TURE 1;
/*************************************************************/
/* 条件编译处理                                              */
/* 且如果NO_DIRECT_DESTCODE为FALSE，NO_MIDCODE、NO_CONSOPTI、*/
/* NO_ECCOPTI、NO_LOOPOPTI、NO_DESTCODE均不能为FALSE         */
/*************************************************************/

/* 设置语法分析条件编译标志NO_PARSE为FALSE,         *
 * 如果为TRUE,则得到一个只有词法扫描功能的编译器    */
#define NO_PARSE      FALSE

 /* 设置LL1语法分析条件编译标志NO_LL1为FALSE,		*
  * 如果为TRUE,则进行的递归下降法语法分析；否则，    *
  * 将进行LL1法的语法分析                            */
#define NO_LL1        TRUE

  /* 设置语义分析条件编译标志NO_ANALYZE为FALSE,		*
   * 如果为TRUE,则得到一个只有语法分析功能的编译器	*/
#define NO_ANALYZE    FALSE


#if !NO_PARSE
#endif

#if !NO_LL1
		  /* 条件编译，联入头文件parseLL1.h,该头文件定义了LL1语法解析器界面 */
#include "parseLL1.h"
#else
		  /* 条件编译,联入头文件parse.h,该头文件定义了递归下降法语法解析器界面 */
#include "parse.h"
#endif

#if !NO_ANALYZE
/* 条件编译,联入头文件zanalyze.h,该头文件定义了语义解析器界面 */
#include "analyze.h"
#endif





#include <cstdio>
#include "globals.h"
#include "scanner.h"
//#include "tm.cpp"
#include "util.h"
#include "parseLL1.h"
#include "analyze.h"
#include "symbTable.h"





/*******************全局变量*******************/

FILE* source;		/* 源程序文本文件,为编译器输入文件 */

FILE* listing;		/* 中间列表文件,为编译器中间信息输出文件 */

/*指针fp,指向存放Token序列的文件"Tokenlist"*/
FILE* fp;

/*Token序列中的token数目*/
int Tokennum = 0;

/*清单的行号*/
int lineno = 0;

/*记录当前层的displayOff*/
int savedOff = 0;

/*指向各基本块的指针*/
CodeFile* baseBlock[100];

/*保存主程序的display表的偏移*/
int StoreNoff;

/*中间代码序列，以结构的形式给出*/
CodeFile* midcode = NULL;

/*目标代码文件*/
FILE* code;

/********** 设置追踪标志初始值 **********/

/* 源代码文件追踪标志,初始为FALSE.如果为TRUE,       	*
 * 将源代码信息输出到中间文件listing			        *
 * 在zscanner.cpp文件的函数getNextChar()中，实现输出    */
int EchoSource = TRUE;

/* 词法分析追踪标志,初始为FALSE.如果为TRUE,		*
 * 将词法分析信息输出到中间文件listing			*/
int TraceScan = TRUE;

/* 语法分析追踪标志,初始为FALSE.如果为TRUE,		*
 * 将语法分析信息输出到中间文件listing			*/
int TraceParse = TRUE;

/* 符号表输出标志,初始为FALSE.如果为TRUE,		*
 * 将语义分析时产生的符号表信息输出到中间文件   *
 * listing										*/
int  TraceTable = TRUE;

/* 目标代码追踪标志,初始为FALSE.如果为TRUE,		*
 * 将目标代码的注释信息输出到中间文件listing	*/
int TraceCode = TRUE;

/*错误追踪标识，防止错误的进一步传递*/
int  Error = FALSE;


/********************************************************/
/* 函数名 main											*/
/* 功  能 函数将编译程序中各个功能部分有机结合在一起,	*/
/*		  完成用户需要的编译工作,输出相关信息和结果		*/
/* 说  明 函数参数argc指明参数个数,参数argv记录参字串	*/
/*		  用户应给定要编译的源程序文件目录名为调用参数	*/
/********************************************************/

int main()

{
	/*输入要编译的文件名*/
	char pgm[120];

	/*目标代码文件名*/
	char* codefile=NULL;

	printf("input program names:\n");

	scanf("%s", pgm);

	/* 若未指定文件扩展名,给源文件加上扩展名.tny */
	if (strchr(pgm, '.') == NULL)
		strcat(pgm, ".tny");

	/* 用只读方式打开源程序文件pgm,文件指针给source */
	source = fopen(pgm, "r");

	if (source == NULL)
	{
		fprintf(stderr, "File %s not found\n", pgm);
		exit(1);
	}

	/* 将中间信息列表文件listing指向屏幕标准输出stdout */
	listing = stdout;

	//testing = fopen("c://1//3.txt","w");

   /* 输出中间编译信息 */
	fprintf(listing, "\nTINY COMPILATION: %s\n", pgm);

	/* 调用词法分析函数，得到Token序列*/
	getTokenlist();

	if (EchoSource)
	{ /*按键进入下一部分*/
		getchar();
		getchar();

	}

	/* 如果词法分析追踪标志为TRUE,则将生成的Token序列输出到屏幕 */
	if (TraceScan)
	{
		fprintf(listing, "\nLexical analysizing:\n");
		//fprintf(testing,"\nLexical analysizing:\n");
		fprintf(listing, "\ntoken list:\n");
		printTokenlist();
		getchar();
	}



	/*********************************/
	/* 条件编译处理,选择语法分析部分 */
	/*********************************/
	/* 条件编译,如果!NO_PARSE为FALSE，  *
	/* 得到一个只有词法扫描功能的编译器,*
	/* 否则进行语法分析                 */
#if !NO_PARSE


	/* 条件编译，只做LL1语法分析处理    *
	/* 调用语法分析函数，生成语法分析树 */
#if !NO_LL1

	TreeNode* syntaxTree;

	fprintf(listing, "\nLL1 Syntax analysizing:\n");
	syntaxTree = parseLL1();

	/* 如果语法分析追踪标志为TRUE且没有语法错误,
	   则将生成的语法树输出到屏幕 */
	if ((TraceParse) && (!Error))
	{
		fprintf(listing, "\nSyntax tree:\n\n");
		printTree(syntaxTree);
		getchar();
		getchar();
	}


	/* 条件编译,只做递归下降语法解析处理*
	 * 调用语法分析函数,生成语法分析树	*/
#else
	TreeNode* syntaxTree;

	fprintf(listing, "\nRecursion Syntax analysizing:\n");
	syntaxTree = parse();

	/* 如果语法分析追踪标志为TRUE且没有语法错误,
	   则将生成的语法树输出到屏幕 */
	if ((TraceParse) && (!Error))
	{
		fprintf(listing, "\nSyntax tree:\n\n");
		printTree(syntaxTree);
		getchar();
	}
#endif//语法分析方法选择结束NO_LL1


	/*********************************/
	/* 条件编译处理,选择语义分析部分 */
	/*********************************/
	/*条件编译，当!NO_ANALYZE为真，且前面的分析没有错误，
	  则进行下面的语义分析*/
#if !NO_ANALYZE
	{
		fprintf(listing, "\nSematic analysizing:\n");
		fprintf(listing, "Checking  sematic  error......\n");
		/*语义分析*/
		analyze(syntaxTree);
		if (!Error)
			fprintf(listing, " \nNo  error !\n");

		/*输出符号表*/
		if ((TraceTable) && (!Error))
		{
			fprintf(listing, "\nNow printing  symb table.....\n");
			PrintSymbTable();
			getchar();
			getchar();
		}
		/*输出含符号表信息的语法树*/
		if ((TraceParse) && (!Error))
		{
			fprintf(listing, "after  analysis ..");
			fprintf(listing, "\nSyntax tree:\n\n");
			printTree(syntaxTree);
			getchar();
		}
	}

#endif//结束NO_ANALYZE的条件编译
#endif//结束NO_PARSE的条件编译


	getchar();
	fprintf(listing, "\nNow is free space ........\n");
	getchar();

	/*释放语法树空间*/
	if (syntaxTree != NULL)
	{
		freeTree(syntaxTree);
		fprintf(listing, ">>free  tree\n");
	}
	/*释放符号表空间*/
	freeTable();
	fprintf(listing, ">>free  symb table\n ");

	
	/* 编译结束,关闭源程序文件source */
	fclose(source);

	/* 编译成功,程序正常返回 */
	return 0;
}







