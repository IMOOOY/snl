
/****************************************************/
/* �ļ�	scanner.cpp								*/
/* ˵�� TINY�������Ĵʷ�ɨ����ʵ��					*/
/* ���� �������ṹ:ԭ���ʵ��						*/
/****************************************************/

/************ �ô����ļ���������ͷ�ļ� **************/

#include "globals.h"	/* ��ͷ�ļ�globals.h������ȫ��������� */

#include "string.h"

#include "ctype.h"      /* �õ��˸ÿ��е�isalnum,isalpha,isdigit���� */

#include "util.h"		/* ��ͷ�ļ�util.h��������ع��ܺ��� */

#include "math.h"       /* �õ�����������ȡģ���� */

/*****************�ʷ�������ȷ���������Զ���DFA��״̬����*************/
/* START ��ʼ״̬; INASSIGN ��ֵ״̬; INRANGE �±귶Χ״̬;          */
/* INNUM ����״̬; INID ��ʶ��״̬; DONE ���״̬;                   */
/* INCHAR �ַ�״̬;INCOMMENT ע��״̬;                               */
typedef enum
{
    START, INASSIGN, INRANGE, INCOMMENT, INNUM, INID, INCHAR, DONE
}
StateType;


/* tokenString���ڱ����ʶ���ͱ����ֵ��ʵĴ�Ԫ,����41 */
char tokenString[MAXTOKENLEN + 1];


/* BUFLENԴ�����е����뻺��������Ϊ256 */
#define BUFLEN 256


/* lineBufΪ��ǰ��������л����� */
static char lineBuf[BUFLEN];


/* lineposΪ�ڴ��뻺����LineBuf�еĵ�ǰ�ַ�λ��,��ʼΪ0 */
static int linepos = 0;


/* bufsizeΪ��ǰ�������������ִ���С */
static int bufsize = 0;


/* EOF_flag��Ϊ�ļ�βʱ,�ı亯��ungetNextChar���� */
static int EOF_flag = FALSE;




/*******************************************************************/
/* ������ getNextChar											   */
/* ��  �� ȡ����һ�ǿ��ַ�����									   */
/* ˵  �� �ú��������뻺����lineBuf��ȡ����һ���ǿ��ַ�		       */
/*        ���lineBuf�е��ִ��Ѿ�����,���Դ�����ļ��ж���һ����   */
/*******************************************************************/
static int getNextChar(void)
{
    /* ��ǰ���������л�����lineBuf�Ѿ��ľ� */
    if (!(linepos < bufsize))

    {
        /* Դ�����к�lineno��1 */
        lineno++;

        /* ��Դ�ļ�source�ж���BUFLEN-2(254)���ַ����л�����lineBuf��      *
         * fgets�ڵ�lineBufĩβ�������з�.����ĩβ����һ��NULL�ַ���ʾ���� */
        if (fgets(lineBuf, BUFLEN - 1, source))

        {
            /* ���Դ�ļ�׷�ٱ�־EchoSourceΪTRUE                               *
             * ��Դ�����к�lineno��������lineBuf�ڴʷ�ɨ��ʱд���б��ļ�listing */
            if (EchoSource) fprintf(listing, "%4d: %s", lineno, lineBuf);

            /* ȡ�õ�ǰ����Դ�����е�ʵ�ʳ���,�͸�����bufsize */
            bufsize = strlen(lineBuf);

            /* �����л�����lineBuf�е�ǰ�ַ�λ��lineposָ��lineBuf��ʼλ�� */
            linepos = 0;

            /* ȡ�������л�����lineBuf����һ�ַ� */
            return lineBuf[linepos++];

        }
        else
        {
            /* δ�ܳɹ������µĴ�����,fget��������ֵΪNULL *
             * �Ѿ���Դ�����ļ�ĩβ,����EOF_flag��־ΪTRUE */
            EOF_flag = TRUE;

            /* ��������EOF */
            return EOF;
        }
    }

    /* �����뻺����lineBuf���ַ���δ����,ֱ��ȡ������һ�ַ�,����������ȡ�ַ� */
    else return lineBuf[linepos++];
}


/********************************************************/
/* ������ ungetNextChar									*/
/* ��  �� �ַ����˺���									*/
/* ˵  �� �ù����������뻺����lineBuf�л���һ���ַ�		*/
/*        ���ڳ�ǰ���ַ���ƥ��ʱ��Ļ���				*/
/********************************************************/
static void ungetNextChar(void)

{
    /* ���EOF_flag��־ΪFALSE,���Ǵ���Դ�ļ�ĩβ  *
     * �����л�����lineBuf�е�ǰ�ַ�λ��linepos��1 */
    if (!EOF_flag) linepos--;
}


/******************* �����ֲ��ұ� ********************/

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
/* ������ reservedLookup								      */
/* ��  �� �����ֲ��Һ���									  */
/* ˵  �� ʹ�����Բ���,�鿴һ����ʶ���Ƿ��Ǳ�����			  */
/*		  ��ʶ������ڱ����ֱ����򷵻���Ӧ����,���򷵻ص���ID */
/**************************************************************/
static LexType  reservedLookup(char* s)

{
    int i;

    /* �ڱ����ֱ��в���,MAXRESERVED�Ѿ�����Ϊ8,Ϊ�������� */
    for (i = 0; i < MAXRESERVED; i++)

        /* ���Բ鱣���ֱ�,�쿴��������sָ����ʶ���Ƿ��ڱ��� *
         * �����ַ���ƥ���ʱ��,����strcmp����ֵΪ0(FALSE)	*/
        if (!strcmp(s, reservedWords[i].str))

            /* �ַ���s�뱣���ֱ���ĳһ����ƥ��,�������ض�Ӧ�����ֵ��� */
            return reservedWords[i].tok;

    /* �ַ���sδ�ڱ����ֱ����ҵ�,�������ر�ʶ������ID */
    return ID;
}


/****************************************
 ********* �ʷ�ɨ������������  **********
 ****************************************/

 /************************************************************/
 /* ������ getTokenlist						   			    */
 /* ��  �� ȡ�õ��ʺ���										*/
 /* ˵  �� ������Դ�ļ��ַ��������л�ȡ����Token���� 		*/
 /*        ʹ��ȷ���������Զ���DFA,����ֱ��ת��    		*/
 /*        ��ǰ���ַ�,�Ա����ֲ��ò��ʽʶ��    			*/
 /*        �����ʷ�����ʱ��,�����Թ�����������ַ�,���Ӹ���  */
 /************************************************************/

void getTokenlist(void)

{
    ChainNodeType* chainHead;    /*����ı�ͷָ��*/
    ChainNodeType* currentNode;  /*ָ����ǰToken�ĵ�ǰ���*/
    ChainNodeType* preNode;      /*ָ��ǰ����ǰ�����*/
    ChainNodeType* tempNode;     /*��ʱָ�룬�����ͷ�������*/
    TokenType currentToken;      /*��ŵ�ǰ��Token*/

    /*��������ĵ�һ�����*/
    chainHead = preNode = currentNode = (ChainNodeType*)malloc(CHAINNODELEN);
    /*��ʼ����ǰ����У�ָ����һ������ָ��Ϊ��*/
    (*currentNode).nextToken = NULL;

    do
    {  /* tokenStringIndex���ڼ�¼��ǰ����ʶ�𵥴ʵĴ�Ԫ�洢�� *
        * tokenString�еĵ�ǰ����ʶ���ַ�λ��,��ʼΪ0          */
        int tokenStringIndex = 0;

        /* ��ǰ״̬��־state,ʼ�ն�����START��Ϊ��ʼ */
        StateType state = START;

        /* tokenString�Ĵ洢��־save,��������						*
         * ������ǰʶ���ַ��Ƿ���뵱ǰʶ�𵥴ʴ�Ԫ�洢��tokenString */
        int save;

        /* ��ǰȷ���������Զ���DFA״̬state�������״̬DONE */
        while (state != DONE)

        {
            /* ��Դ�����ļ��л�ȡ��һ���ַ�,�������c��Ϊ��ǰ�ַ� */
            int c = getNextChar();

            /* ��ǰ��ʶ���ַ��Ĵ洢��־save��ʼΪTRUE */
            save = TRUE;

            switch (state)
            {
                /* ��ǰDFA״̬stateΪ��ʼ״̬START,DFA���ڵ�ǰ���ʿ�ʼλ�� */
            case START:

                /* ��ǰ�ַ�cΪ����,��ǰDFA״̬state����Ϊ����״̬INNUM *
                 * ȷ���������Զ���DFA�����������͵�����               */
                if (isdigit(c))
                    state = INNUM;

                /* ��ǰ�ַ�cΪ��ĸ,��ǰDFA״̬state����Ϊ��ʶ��״̬INID *
                 * ȷ���������Զ���DFA���ڱ�ʶ�����͵�����              */
                else if (isalpha(c))
                    state = INID;

                /* ��ǰ�ַ�cΪð��,��ǰDFA״̬state����Ϊ��ֵ״̬INASSIGN *
                 * ȷ���������Զ���DFA���ڸ�ֵ���͵�����				   */
                else if (c == ':')
                    state = INASSIGN;

                /* ��ǰ�ַ�cΪ.,��ǰDFA״̬state����Ϊ�����±����״̬*/
                /* INRANGE��ȷ���������Զ���DFA���������±�������͵�����*/
                else if (c == '.')
                    state = INRANGE;


                /* ��ǰ�ַ�cΪ',��ǰDFA״̬state����Ϊ�ַ���־״̬*/
                /* INCHAR��ȷ���������Զ���DFA�����ַ���־���͵�����*/
                else if (c == '\'')
                {
                    save = FALSE;
                    state = INCHAR;
                }

                /* ��ǰ�ַ�cΪ�հ�(�ո�,�Ʊ��,���з�),�ַ��洢��־save����ΪFALSE *
                 * ��ǰ�ַ�Ϊ�ָ���,����Ҫ��������,����洢                        */
                else if ((c == ' ') || (c == '\t') || (c == '\n')|| (c == '\r'))
                    save = FALSE;

                /* ��ǰ�ַ�cΪ������,�ַ��洢��־save����ΪFALSE     *
                 * ��ǰDFA״̬state����Ϊע��״̬INCOMMENT			  *
                 * ȷ���������Զ���DFA����ע����,�����ɵ���,����洢 */
                else if (c == '{')
                {
                    save = FALSE;
                    state = INCOMMENT;
                }

                /* ��ǰ�ַ�cΪ�����ַ�,��ǰDFA״̬state����Ϊ���״̬DONE *
                 * ȷ���������Զ���DFA���ڵ��ʵĽ���λ��,���һ�����ദ�� */
                else

                {
                    state = DONE;
                    switch (c)
                    {
                        /* ��ǰ�ַ�cΪEOF,�ַ��洢��־save����ΪFALSE,����洢     *
                         * ��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ�ļ���������ENDFILE */
                    case EOF:
                        save = FALSE;
                        currentToken.Lex = ENDFILE;
                        break;

                        /* ��ǰ�ַ�cΪ"=",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ�Ⱥŵ���EQ */
                    case '=':
                        currentToken.Lex = EQ;
                        break;

                        /* ��ǰ�ַ�cΪ"<",��ǰʶ�𵥴ʷ���ֵcurrentToken����ΪС�ڵ���LT */
                    case '<':
                        currentToken.Lex = LT;
                        break;

                        /* ��ǰ�ַ�cΪ"+",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ�Ӻŵ���PLUS */
                    case '+':
                        currentToken.Lex = PLUS;
                        break;

                        /* ��ǰ�ַ�cΪ"-",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ���ŵ���MINUS */
                    case '-':
                        currentToken.Lex = MINUS;
                        break;

                        /* ��ǰ�ַ�cΪ"*",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ�˺ŵ���TIMES */
                    case '*':
                        currentToken.Lex = TIMES;
                        break;

                        /* ��ǰ�ַ�cΪ"/",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ���ŵ���OVER */
                    case '/':
                        currentToken.Lex = OVER;
                        break;

                        /* ��ǰ�ַ�cΪ"(",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ�����ŵ���LPAREN */
                    case '(':
                        currentToken.Lex = LPAREN;
                        break;

                        /* ��ǰ�ַ�cΪ")",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ�����ŵ���RPAREN */
                    case ')':
                        currentToken.Lex = RPAREN;
                        break;

                        /* ��ǰ�ַ�cΪ";",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ�ֺŵ���SEMI */
                    case ';':
                        currentToken.Lex = SEMI;
                        break;
                        /* ��ǰ�ַ�cΪ",",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ���ŵ���COMMA */
                    case ',':
                        currentToken.Lex = COMMA;
                        break;
                        /* ��ǰ�ַ�cΪ"[",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ�������ŵ���LMIDPAREN */
                    case '[':
                        currentToken.Lex = LMIDPAREN;
                        break;

                        /* ��ǰ�ַ�cΪ"]",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ�������ŵ���RMIDPAREN */
                    case ']':
                        currentToken.Lex = RMIDPAREN;
                        break;

                        /* ��ǰ�ַ�cΪ�����ַ�,��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ���󵥴�ERROR */
                    default:
                        currentToken.Lex = ERROR;
                        Error = TRUE;
                        break;
                    }
                }
                break;
                /********** ��ǰ״̬Ϊ��ʼ״̬START�Ĵ������ **********/

                /* ��ǰDFA״̬stateΪע��״̬INCOMMENT,ȷ���������Զ���DFA����ע��λ�� */
            case INCOMMENT:

                /* ��ǰ�ַ��洢״̬save����ΪFALSE,ע�������ݲ����ɵ���,����洢 */
                save = FALSE;

                /* ��ǰ�ַ�cΪEOF,��ǰDFA״̬state����Ϊ���״̬DONE,��ǰ����ʶ����� *
                 * ��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ�ļ���������ENDFILE            */
                if (c == EOF)
                {
                    state = DONE;
                    currentToken.Lex = ENDFILE;

                }

                /* ��ǰ�ַ�cΪ"}",ע�ͽ���.��ǰDFA״̬state����Ϊ��ʼ״̬START */
                else if (c == '}') state = START;
                break;

                /* ��ǰDFA״̬stateΪ��ֵ״̬INASSIGN,ȷ���������Զ���DFA���ڸ�ֵ����λ�� */
            case INASSIGN:

                /* ��ǰDFA״̬state����Ϊ���״̬DONE,��ֵ���ʽ��� */
                state = DONE;

                /* ��ǰ�ַ�cΪ"=",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ��ֵ����ASSIGN */
                if (c == '=')
                    currentToken.Lex = ASSIGN;

                /* ��ǰ�ַ�cΪ�����ַ�,��":"����"=",�������л������л���һ���ַ�       *
                 * �ַ��洢״̬save����ΪFALSE,��ǰʶ�𵥴ʷ���ֵcurrentToken����ΪERROR */
                else
                {
                    ungetNextChar();
                    save = FALSE;
                    currentToken.Lex = ERROR;
                    Error = TRUE;
                }
                break;

            case INRANGE:

                /* ��ǰDFA״̬state����Ϊ���״̬DONE,��ֵ���ʽ��� */
                state = DONE;

                /* ��ǰ�ַ�cΪ".",��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ�±��UNDERANGE */
                if (c == '.')
                    currentToken.Lex = UNDERANGE;

                /* ��ǰ�ַ�cΪ�����ַ�,��"."����".",�������л������л���һ���ַ�       *
                 * �ַ��洢״̬save����ΪFALSE,��ǰʶ�𵥴ʷ���ֵcurrentToken����ΪERROR */
                else
                {
                    ungetNextChar();
                    save = FALSE;
                    currentToken.Lex = DOT;
                }
                break;

                /* ��ǰDFA״̬stateΪ����״̬INNUM,ȷ���������Զ����������ֵ���λ�� */
            case INNUM:

                /* ��ǰ�ַ�c��������,���������л�����Դ�л���һ���ַ�					*
                 * �ַ��洢��־����ΪFALSE,��ǰDFA״̬state����ΪDONE,���ֵ���ʶ����� *
                 * ��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ���ֵ���NUM                     */
                if (!isdigit(c))
                {
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    currentToken.Lex = INTC;
                }
                break;
                /*��ǰDFA״̬stateΪ�ַ���־״̬INCHAR,ȷ�������Զ��������ַ���־״̬*/
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
                /* ��ǰDFA״̬stateΪ��ʶ��״̬INID,ȷ���������Զ���DFA���ڱ�ʶ������λ�� */
            case INID:

                /* ��ǰ�ַ�c������ĸ,���������л�����Դ�л���һ���ַ�		 			  *
                 * �ַ��洢��־����ΪFALSE,��ǰDFA״̬state����ΪDONE,��ʶ������ʶ����� *
                 * ��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ��ʶ������ID                      */
                if (!isalnum(c))
                {
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    currentToken.Lex = ID;
                }
                break;

                /* ��ǰDFA״̬stateΪ���״̬DONE,ȷ���������Զ���DFA���ڵ��ʽ���λ�� */
            case DONE:	break;

                /* ��ǰDFA״̬stateΪ����״̬,���������Ӧ���� */
            default:

                /* ���ʷ�ɨ�������������״̬stateд���б��ļ�listing	*
                 * ��ǰDFA״̬state����Ϊ���״̬DONE					*
                 * ��ǰʶ�𵥴ʷ���ֵcurrentToken����Ϊ���󵥴�ERROR	*/
                fprintf(listing, "Scanner Bug: state= %d\n", state);
                Error = TRUE;
                state = DONE;
                currentToken.Lex = ERROR;
                break;

            }
            /*************** �����жϴ������ *******************/

            /* ��ǰ�ַ��洢״̬saveΪTRUE,�ҵ�ǰ��ʶ�𵥴��Ѿ�ʶ�𲿷�δ����������󳤶� *
             * ����ǰ�ַ�cд�뵱ǰ��ʶ�𵥴ʴ�Ԫ�洢��tokenString						  */
            if ((save) && (tokenStringIndex <= MAXTOKENLEN))
                tokenString[tokenStringIndex++] = (char)c;

            if (state == DONE)
            {
                /* ��ǰDFA״̬stateΪ���״̬DONE,����ʶ�����   *
                 * ��ǰʶ�𵥴ʴ�Ԫ�洢��tokenString���Ͻ�����־ */
                tokenString[tokenStringIndex] = '\0';

                /* ��ǰ����currentTokenΪ��ʶ����������,�鿴���Ƿ�Ϊ�����ֵ��� */
                if (currentToken.Lex == ID)
                {
                    currentToken.Lex = reservedLookup(tokenString);
                    if (currentToken.Lex != ID)
                        strcpy(tokenString, tokenString);
                }
            }
        }
        /**************** ѭ��������� ********************/
        /*���к���Ϣ����Token*/
        currentToken.lineshow = lineno;
        /*�����ʵ�������Ϣ����Token*/
        strcpy(currentToken.Sem, tokenString);

        /*���Ѵ�����ĵ�ǰToken���������Token����*/
        ((*currentNode).Token).lineshow = currentToken.lineshow;
        ((*currentNode).Token).Lex = currentToken.Lex;
        strcpy(((*currentNode).Token).Sem, currentToken.Sem);

        Tokennum++;   /*Token����Ŀ��1*/

        /*�����ǵ�һ����㣬�򽫵�ǰ�����������*/
        if (preNode != currentNode)
        {
            preNode->nextToken = currentNode;
            preNode = currentNode;
        }
        /*����һ���µĽ�㣬�Լ�¼��һ��Token����Ϣ*/
        currentNode = (ChainNodeType*)malloc(CHAINNODELEN);
        /*��ʼ���������У�ָ����һ������ָ��Ϊ��*/
        currentNode->nextToken = NULL;
    }
    /* ֱ���������ʾ�ļ�������Token:ENDFILE��˵�����������е�Token*/
    /* �������������У�ѭ������*/
    while ((currentToken.Lex) != ENDFILE);
    /*����chainHeadָ���Token��������ļ�"Tokenlist"��*/
    ChainToFile(chainHead);

    /*�ͷ�����*/
    while (chainHead != NULL)
    {
        tempNode = chainHead->nextToken;
        free(chainHead);
        chainHead = tempNode;
    }

}
/* �ʷ������������� */










