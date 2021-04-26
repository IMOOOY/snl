#include "globals.h"
#include "string.h"
#include "ctype.h"      /* 用到了该库中的isalnum,isalpha,isdigit函数 */
#include "util.h"		/* 该头文件util.h定义了相关功能函数 */
#include "math.h"       /* 用到了这个库里的取模运算 */

typedef enum
{
    START, INASSIGN, INRANGE, INCOMMENT, INNUM, INID, INCHAR, DONE
}
StateType;



char tokenString[MAXTOKENLEN + 1];



#define BUFLEN 256



static char lineBuf[BUFLEN];



static int linepos = 0;



static int bufsize = 0;



static int EOF_flag = FALSE;



static int getNextChar(void)
{
    
    if (!(linepos < bufsize))

    {
        
        lineno++;

        
         
        if (fgets(lineBuf, BUFLEN - 1, source))

        {
            
             
            if (EchoSource) fprintf(listing, "%4d: %s", lineno, lineBuf);

            
            bufsize = strlen(lineBuf);

            
            linepos = 0;

            
            return lineBuf[linepos++];

        }
        else
        {
            
             
            EOF_flag = TRUE;

            
            return EOF;
        }
    }

    
    else return lineBuf[linepos++];
}

static void ungetNextChar(void)

{

    if (!EOF_flag) linepos--;
}



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



static LexType  reservedLookup(char* s)

{
    int i;

    
    for (i = 0; i < MAXRESERVED; i++)

        
         
        if (!strcmp(s, reservedWords[i].str))

            
            return reservedWords[i].tok;

    
    return ID;
}


void getTokenlist(void)

{
    ChainNodeType* chainHead;    /*链表的表头指针*/
    ChainNodeType* currentNode;  /*指向处理当前Token的当前结点*/
    ChainNodeType* preNode;      /*指向当前结点的前驱结点*/
    ChainNodeType* tempNode;     /*临时指针，用于释放链表部分*/
    TokenType currentToken;      /*存放当前的Token*/

    
    chainHead = preNode = currentNode = (ChainNodeType*)malloc(CHAINNODELEN);
    
    (*currentNode).nextToken = NULL;

    do
    {
        int tokenStringIndex = 0;

        
        StateType state = START;

        
         
        int save;

        
        while (state != DONE)

        {
            
            int c = getNextChar();

            
            save = TRUE;

            switch (state)
            {
                
            case START:
                if (isdigit(c))
                    state = INNUM;

                else if (isalpha(c))
                    state = INID;

                else if (c == ':')
                    state = INASSIGN;

                else if (c == '.')
                    state = INRANGE;

                else if (c == '\'')
                {
                    save = FALSE;
                    state = INCHAR;
                }

                else if ((c == ' ') || (c == '\t') || (c == '\n')|| (c == '\r'))
                    save = FALSE;

                else if (c == '{')
                {
                    save = FALSE;
                    state = INCOMMENT;
                }

                else

                {
                    state = DONE;
                    switch (c)
                    {

                    case EOF:
                        save = FALSE;
                        currentToken.Lex = ENDFILE;
                        break;

                    case '=':
                        currentToken.Lex = EQ;
                        break;

                        
                    case '<':
                        currentToken.Lex = LT;
                        break;

                        
                    case '+':
                        currentToken.Lex = PLUS;
                        break;

                        
                    case '-':
                        currentToken.Lex = MINUS;
                        break;

                        
                    case '*':
                        currentToken.Lex = TIMES;
                        break;

                        
                    case '/':
                        currentToken.Lex = OVER;
                        break;

                        
                    case '(':
                        currentToken.Lex = LPAREN;
                        break;

                        
                    case ')':
                        currentToken.Lex = RPAREN;
                        break;

                        
                    case ';':
                        currentToken.Lex = SEMI;
                        break;
                        
                    case ',':
                        currentToken.Lex = COMMA;
                        break;
                        
                    case '[':
                        currentToken.Lex = LMIDPAREN;
                        break;

                        
                    case ']':
                        currentToken.Lex = RMIDPAREN;
                        break;

                        
                    default:
                        currentToken.Lex = ERROR;
                        Error = TRUE;
                        break;
                    }
                }
                break;
                

                
            case INCOMMENT:

                
                save = FALSE;

                
                 
                if (c == EOF)
                {
                    state = DONE;
                    currentToken.Lex = ENDFILE;

                }

                
                else if (c == '}') state = START;
                break;

                
            case INASSIGN:

                
                state = DONE;

                
                if (c == '=')
                    currentToken.Lex = ASSIGN;

                
                 
                else
                {
                    ungetNextChar();
                    save = FALSE;
                    currentToken.Lex = ERROR;
                    Error = TRUE;
                }
                break;

            case INRANGE:

                
                state = DONE;

                
                if (c == '.')
                    currentToken.Lex = UNDERANGE;

                
                 
                else
                {
                    ungetNextChar();
                    save = FALSE;
                    currentToken.Lex = DOT;
                }
                break;

                
            case INNUM:

                
                 
                 
                if (!isdigit(c))
                {
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    currentToken.Lex = INTC;
                }
                break;
                
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
                
            case INID:

                if (!isalnum(c))
                {
                    ungetNextChar();
                    save = FALSE;
                    state = DONE;
                    currentToken.Lex = ID;
                }
                break;

                
            case DONE:	break;

                
            default:

                fprintf(listing, "Scanner Bug: state= %d\n", state);
                Error = TRUE;
                state = DONE;
                currentToken.Lex = ERROR;
                break;

            }
            if ((save) && (tokenStringIndex <= MAXTOKENLEN))
                tokenString[tokenStringIndex++] = (char)c;

            if (state == DONE)
            {
                tokenString[tokenStringIndex] = '\0';

                if (currentToken.Lex == ID)
                {
                    currentToken.Lex = reservedLookup(tokenString);
                    if (currentToken.Lex != ID)
                        strcpy(tokenString, tokenString);
                }
            }
        }

        currentToken.lineshow = lineno;
        strcpy(currentToken.Sem, tokenString);

        ((*currentNode).Token).lineshow = currentToken.lineshow;
        ((*currentNode).Token).Lex = currentToken.Lex;
        strcpy(((*currentNode).Token).Sem, currentToken.Sem);

        Tokennum++;   /*Token总数目加1*/

        if (preNode != currentNode)
        {
            preNode->nextToken = currentNode;
            preNode = currentNode;
        }
        currentNode = (ChainNodeType*)malloc(CHAINNODELEN);
        currentNode->nextToken = NULL;
    }
    while ((currentToken.Lex) != ENDFILE);
    ChainToFile(chainHead);

    while (chainHead != NULL)
    {
        tempNode = chainHead->nextToken;
        free(chainHead);
        chainHead = tempNode;
    }

}










