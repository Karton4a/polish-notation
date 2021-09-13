#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>

#define INIT_SIZE 3
typedef struct 
{
    char name[20];
    double value;
} Var;

typedef struct
{
    char **data;
    char *optimization;
    size_t size;
    size_t top;
} StackStr;
typedef struct
{
    double *data;
    size_t size;
    size_t top;
} StackDbl;


StackStr *createStringStack();
StackDbl *createDoubleStack();
void freeStringStack(StackStr *stack);
void freeDblStack(StackDbl *stack);
void pushString(StackStr *stack,char *value);
void pushDbl(StackDbl *stack,double value);
char *popString(StackStr *stack);
double popDbl(StackDbl *stack);
char *peekString(const StackStr *stack);
double peekDbl(const StackDbl *stack);
int priority(char *op);
int strLen(char *string);
bool isNum(char ch);
char *buildPolishNotation(char *equasion);
bool isLetter(char ch);
bool syntaxCheck(char equasion[]);
bool strChr(char *string,char ch);
void addDelimiter(char *string,const char delim);
bool partialStrCmp(char *begin,char *end,char *string);
double calculateResult(char *polishNotation,Var variables[],int size);


int main(int argc, char const *argv[])
{
    char *equasion ="cos(pi/2)";
    char *polishNotation = buildPolishNotation(equasion);
    Var vars[2];
    vars[0].value = M_E;
    strcpy(vars[0].name,"e");
    vars[1].value = M_PI;
    strcpy(vars[1].name,"pi");
    printf("%s\n",polishNotation);
    printf("%lf\n",calculateResult(polishNotation,vars,2));
    free(polishNotation);
    return 0;
}
bool syntaxCheck(char equasion[])
{
    //TODO написать проверку синтаксиса
}

char *buildPolishNotation(char *equasion)
{
    StackStr *stack = createStringStack(); 
    char *polishNotation = malloc(sizeof(char) * 4 * strLen(equasion) + 1);
    size_t i = 0;
    const char delim ='|';
    const char delimOp = '%';
    //polishNotation[0] = delim;
    polishNotation[0] = '\0';
    int len = 0;
    while(equasion[i] != '\0')
    {
        if(isNum(equasion[i]))
        {
            char *begin = equasion + i;
            while( (isNum(equasion[i]) || equasion[i]=='.')&&equasion[i] != '\0')
            {
                i++;
            }
            len = equasion + i - begin;
            begin = malloc(len * sizeof(char) + 1);
            memcpy(begin,equasion + i-len,len);
            begin[len] = '\0';
            addDelimiter(polishNotation,delim);
            strcat(polishNotation,begin);
            addDelimiter(polishNotation,delim);
            free(begin);
            continue;
        }
        else if(isLetter(equasion[i]))
        {
            char *begin = equasion + i;
            while(isLetter(equasion[i]) && equasion[i] != '\0')
            {
                i++;
            }
            len = equasion + i - begin;
            begin = malloc(len * sizeof(char) + 1);
            memcpy(begin,equasion + i-len,len);
            begin[len] = '\0';
            if(equasion[i]=='(')
            {
                pushString(stack,begin);
            }
            else
            {
                addDelimiter(polishNotation,delim);
                strcat(polishNotation,begin);
                addDelimiter(polishNotation,delim);
            }
            free(begin);
            continue;  
        }
        else
        {
            char opString[2];
            opString[0] = equasion[i];
            opString[1] = '\0';
            if(peekString(stack)== NULL)
            {
                pushString(stack,opString);
            }
            else
            {
                if(equasion[i] == ')')
                {
                    char *op = popString(stack);
                    while(!strChr( op , '(' ))
                    {
                        addDelimiter(polishNotation,delimOp);
                        strcat(polishNotation,op);
                        addDelimiter(polishNotation,delimOp);
                        op = popString(stack);
                    }
                }
            
                else if(priority(peekString(stack)) == priority(opString))
                {
                    addDelimiter(polishNotation,delimOp);
                    strcat(polishNotation,popString(stack));
                    addDelimiter(polishNotation,delimOp);
                    pushString(stack,opString);
                }
                else if (priority(peekString(stack)) > priority(opString) || equasion[i] == '(')
                {
                    pushString(stack,opString);
                }
                else if (priority(peekString(stack)) < priority(opString))
                {
                    addDelimiter(polishNotation,delimOp);
                    strcat(polishNotation,popString(stack));
                    addDelimiter(polishNotation,delimOp);
                    pushString(stack,opString);
                }
                
            }
        }
        i++;
    }
    while(peekString(stack) != NULL)
    {
        addDelimiter(polishNotation,delimOp);
        strcat(polishNotation,popString(stack));
        addDelimiter(polishNotation,delimOp);
  
    }
    freeStringStack(stack);
    return polishNotation;
}
void addDelimiter(char *string,const char delim)
{
    unsigned int len = strlen(string);
    string[len] = delim;
    string[len + 1] = '\0'; 
}
bool isNum(char ch)
{
    return ch >= '0' && ch<='9';
}
bool isLetter(char ch)
{
    return (ch >= 'a' && ch <= 'z')||(ch >= 'A' && ch <= 'Z');
}
int priority(char *op)
{
    if(strChr(op,'^') || strChr(op,'!'))
    {
       return 1; 
    }
    else if(strChr(op,'+'))
    {
        return 3;
    }
    else if(strChr(op,'-'))
    {
        return 3;
    }
    else if(strChr(op,'*'))
    {
        return 2;
    }
    else if(strChr(op,'/'))
    {
        return 2;
    }
    else if(strChr(op,'('))
    {
       return 4; 
    }
    else if(strChr(op,')'))
    {
        return 4; 
    }
    else
    {
        return 5;
    }            
}
double calculateResult(char *polishNotation,Var variables[],int size)
{
    StackDbl *stack = createDoubleStack();
    for(int i = 0; polishNotation[i]!='\0'; i++)
    {
        if(polishNotation[i]=='|')
        {
            char *begin = polishNotation + i + 1;
            do 
            {
                i++;
            } while(polishNotation[i] != '|');
            int len = polishNotation + i - begin;
            begin = (char *)malloc(sizeof(char) * len + 1);
            memcpy(begin,polishNotation + i - len,len);
            begin[len] = '\0';
            if(!isNum(*begin))
            {
                int j=0;
                while((j<size) && (strcmp(begin,variables[j].name)!=0))
                {
                    j++;
                }
                pushDbl(stack,variables[j].value);
            }
            else
            {
                pushDbl(stack,atof(begin));
            }
            free(begin);          
        }
        else if(polishNotation[i]=='%')
        {
            char *begin = polishNotation + i + 1;
            do 
            {
                i++;
            } while(polishNotation[i] != '%');
            char *end = polishNotation + i-1;
            if(partialStrCmp(begin,end,"+"))
            {
                double second = popDbl(stack);
                double first = popDbl(stack);
                pushDbl(stack,first + second);
            }
            else if(partialStrCmp(begin,end,"-"))
            {
                double second = popDbl(stack);
                double first = popDbl(stack);
                pushDbl(stack,first - second);
            }
            else if(partialStrCmp(begin,end,"*"))
            {
                double second = popDbl(stack);
                double first = popDbl(stack);
                pushDbl(stack,first * second);
            }
            else if(partialStrCmp(begin,end,"/"))
            {
                double second = popDbl(stack);
                double first = popDbl(stack);
                pushDbl(stack,first/second);
            }
            else if(partialStrCmp(begin,end,"^"))
            {
                double second = popDbl(stack);
                double first = popDbl(stack);
                pushDbl(stack,pow(first,second));
            }
            else if(partialStrCmp(begin,end,"sqrt"))
            {
                double num = popDbl(stack);
                pushDbl(stack,sqrtl(num));
            }
            else if(partialStrCmp(begin,end,"!"))
            {
                long int fact = 1;
                int num = popDbl(stack);
                for (int i = 1; i <= num; i++)
                {
                    fact = fact * i;
                }
                pushDbl(stack,fact);
            }
            else if(partialStrCmp(begin,end,"cos"))
            {
               double num = popDbl(stack);
               pushDbl(stack,cos(num));
            }
        }
    }
    double result = popDbl(stack);
    freeDblStack(stack); 
    return result;
}
bool partialStrCmp(char *begin,char *end,char *string)
{
    for(int i = 0; (begin + i)!= end+1; i++)
    {
        if(string[i] != begin[i])
        {
            return false;
        }
    }
    if(string[end-begin+1]=='\0')
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool strChr(char *string,char ch)
{
    
    for(char *i = string; *i!='\0'; i++)
    {
        if(*i==ch)
        {
            return true;
        }
    }
    return false;
}

int strLen(char *string)
{
    char *action_string = string;
    int count = 0;
    
    while(*action_string !='\0')
    {
        if(*action_string != 32)
        {
           count++; 
        }
        action_string++;
    }

    return count;   
}

StackStr *createStringStack()
{
    StackStr *stack = malloc(sizeof(StackStr));
    stack->data = (char**)malloc(sizeof(char*) * INIT_SIZE);
    stack->optimization = (char*)malloc(sizeof(char) * 10 * INIT_SIZE);
    for(char i = 0; i < INIT_SIZE; i++)
    {
       stack->data[i] = malloc(sizeof(char) * 10);//stack->optimization + i;
    }
    stack->top = 0;
    stack->size = INIT_SIZE;
    return stack; 
}
void freeStringStack(StackStr *stack)
{
    
    for(int i = 0; i < stack->size; i++)
    {
        free(stack->data[i]);
    }
    
    //free(stack->optimization);
    free(stack->data);
    free(stack);
}
char *peekString(const StackStr *stack) 
{
    if (stack->top <= 0) 
    {
        return NULL;
    }
    else
    {
        return stack->data[stack->top - 1];
    }
}

void pushString(StackStr *stack,char *value)
{
    if(stack->top >= stack->size)
    {
        stack->size++;
        stack->data = realloc(stack->data,stack->size * sizeof(char*));
        //stack->optimization = realloc(stack->optimization,stack->size * sizeof(char) * 10);
        stack->data[stack->size - 1] = malloc(sizeof(char) * 10);
        if (stack->data == NULL) 
        {
            exit(1);
        }
    }
    strcpy(stack->data[stack->top],value);
    stack->top++; 
}
char *popString(StackStr *stack)
{
    if (stack->top == 0) 
    {
        exit(1);
    }
    stack->top--;
    return stack->data[stack->top];
}

//////////////////////////

StackDbl *createDoubleStack()
{
    StackDbl *stack = malloc(sizeof(StackDbl));
    stack->data = (double *)malloc(sizeof(double) * INIT_SIZE);
    stack->top = 0;
    stack->size = INIT_SIZE;
    return stack; 
}
void freeDblStack(StackDbl *stack)
{
    free(stack->data);
    free(stack);
}
double peekDbl(const StackDbl *stack) 
{
    if (stack->top <= 0) 
    {
        return -1;
    }
    else
    {
        return stack->data[stack->top - 1];
    }
}

void pushDbl(StackDbl *stack,double value)
{
    if(stack->top >= stack->size)
    {
        stack->size++;
        stack->data = realloc(stack->data,stack->size * sizeof(double));
        if (stack->data == NULL) 
        {
            exit(1);
        }
    }
    stack->data[stack->top] = value;
    stack->top++; 
}
double popDbl(StackDbl *stack)
{
    if (stack->top == 0) 
    {
        exit(1);
    }
    stack->top--;
    return stack->data[stack->top];
}
