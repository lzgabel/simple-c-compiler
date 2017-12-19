/*************************************************************************
	> File Name: lexer.cc
	> Author: 
	> Mail: 
	> Function: 
	> Remark: 
	> Created Time: 2017年09月13日 星期三 20时56分19秒
	> Last Modified: 2017年09月28日 星期四 12时08分18秒
 ************************************************************************/

#include "common.h"
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <map>
#include <string>
#include <cstring>
#include <cctype>
#include <stdio.h>
using namespace std;
const int BUFLEN  = 100;     //buffer size
static char buffer[BUFLEN];  //buffer
static int buffer_size = 0;      //buffer's data length 
static int readPos = -1;     //读取位置
int lineNum = 1;      //行号
static char oldCh = '0';    //上一字符
const char newline = -2;     //读取新的一行
const char end_of_file = -1; //文件结束标记

string id;      //存放标识符
int num;        //存放数字
static string str;     //存放字符串
char letter=0;  //存放字符



void
Scanner::back()
{
    readPos --;
}

char
Scanner::scan()
{
    if (In.eof())      
        return end_of_file;
    if (readPos == buffer_size-1) {
        if (!In.getline(buffer, BUFLEN)) {          // 没有数据可读
            buffer_size = 1;             
            buffer[0] = end_of_file;
        } else {
            buffer_size = strlen(buffer);
            if (buffer_size == 0) {                 // 忽略空行
                lineNum ++;
                In.getline(buffer, BUFLEN);    
                buffer_size = strlen(buffer);
            }
            buffer[buffer_size++] = newline;
        }
        readPos = -1;
    }

    char ch;
    readPos ++;
    ch = buffer[readPos];

    if (oldCh == newline) {
        lineNum ++;
    }

    if (ch == end_of_file) {
        In.close();
    }

    oldCh = ch;
    return ch;
}


Boundary::Boundary()
{
    boundaries["("]  = T_Lparen;
    boundaries[")"]  = T_Rparen;
    boundaries["["]  = T_Lbrack;
    boundaries["]"]  = T_Rbrack;
    boundaries["{"]  = T_Lbrac;
    boundaries["}"]  = T_Rbrac;
    boundaries[","]  = T_Comma;
    boundaries["\'"]  = T_S_quote;
    boundaries["\""]  = T_D_quotes;
    boundaries[":"]  = T_Colon;
    boundaries[";"]  = T_Semicon;
    boundaries["&&"]  = T_AND; //逻辑与
    boundaries["&"]  = T_AND;  //按位与
    boundaries["||"]  = T_OR; //逻辑或
    boundaries["|"]  = T_OR;  //按位或
}

Tokentype
Boundary::getTokens(const string &name)
{
    return boundaries.find(name)!=boundaries.end() ? boundaries[name] : T_ERR; 
}

string
Boundary::getBoundary(const Tokentype & tokentype)
{
    for (auto it =boundaries.begin(); it != boundaries.end(); it++) {
        if (it->second == tokentype) {
            return it->first;
        }
    }
    return "ERR";
}


Operator::Operator()
{
    operators["="]  = T_ASSIGN;
    operators["+"]  = T_ADD;
    operators["++"]  = T_INC;
    operators["-"]  = T_SUB;
    operators["--"]  = T_DEC;
    operators["*"]  = T_MUL;
    operators["/"]  = T_DIV;
    operators["<"]  = T_LT;
    operators["<="] = T_LE;
    operators[">"]  = T_GT;
    operators[">="] = T_GE;
    operators["=="] = T_EQU;
    operators["!="] = T_NEQU;
}

Tokentype
Operator::getTokens(const string & name)
{
    return operators.find(name)!=operators.end() ? operators[name] : T_ERR; 
}

string
Operator::getOperator(const Tokentype & tokentype)
{
    for (auto it =operators.begin(); it != operators.end(); it++) {
        if (it->second == tokentype) {
            return it->first;
        }
    }
    return "ERR";
}



Keywords::Keywords()
{
    keywords["main"]     =     T_MAIN;
    keywords["if"]       =     T_IF;
    keywords["else"]     =     T_ELSE;
    keywords["switch"]   =     T_SWITCH;
    keywords["case"]     =     T_CASE;
    keywords["default"]  =     T_DEFAULT;
    
    keywords["for"]      =     T_FOR;
    keywords["do"]       =     T_DO;
    keywords["while"]    =     T_WHILE;

    keywords["int"]      =     T_INT;
    keywords["double"]   =     T_DOUBLE;
    keywords["float"]    =     T_FLOAT;
    keywords["char"]     =     T_CHAR;

    keywords["break"]    =     T_BREAK;
    keywords["return"]   =     T_RETURN;
    keywords["continue"] =     T_CONTINUE;

    keywords["extern"]   =     T_EXTERN;
    
    keywords["void"]     =     T_VOID;
    keywords["long"]     =     T_LONG;
    keywords["short"]    =     T_SHORT;

    keywords["struct"]   =     T_STRUCT;
    keywords["enum"]     =     T_ENUM;
    keywords["union"]    =     T_UNION;
}

Tokentype
Keywords::getTokens(const string & name)
{
    return keywords.find(name) != keywords.end() ? keywords[name] : T_IDENTIFIER;
}

string
Keywords::getKeyword(const Tokentype & tokentype)
{
    for (auto it = keywords.begin(); it != keywords.end(); it++) {
        if (it->second == tokentype) {
            return it->first;
        }
    }
    return "ERR";
}


bool
Lexer::scan(const char need=0)
{
    Scanner *S = new Scanner();
    char ch = S->scan();
    if (need) {
        if (ch != need) {
            S -> back();
            return false;
        }
    return true;
    }
    return true;
}

bool
Lexer::lexer()
{
    Scanner *S = new Scanner();
    token._val = -1;
    token._token = T_Null;
    token._name = "";
    char ch;
    ch = S -> scan();
    /**
     * 忽略空白字符 <space> <\n> <\t>
     */
    while( ch == ' ' || ch == 10 || ch == 9 )
    {
        ch = S -> scan();
    }
    if (ch == -1) 
        return false;
    /**
     * 标识符/关键字   _,字母,数字串
     */
    if ((isalpha(ch)) || ch == '_') {  //字母,_ 开头
        string name = "";
        do
        {
            name.push_back(ch);
            ch = S -> scan();
        }while(isalnum(ch)|| ch == '_');
        S -> back();
        token._name = name;
        Keywords *K = new Keywords();
        Tokentype t = K -> getTokens(id);
        if (t == T_IDENTIFIER) {
            token._token = T_IDENTIFIER;
            token._name = name;
        } else {
            token._token = t;
            token._name = K -> getKeyword(token._token);
        }
        delete K;
        return true;
    }
    /**
     *  Number
     */
    else if (isdigit(ch)) {
        int val = 0;
        //十进制
        if (ch != '0') {
            do 
            {
                val = val*10+ch-'0';
                ch = S -> scan();
            }while(isdigit(ch));
            S -> back();
        }
        token._token = T_NUMBER;
        token._val = val;
        stringstream ss;
        ss<<val;
        ss>>token._name;
        return true;
    }

    /**
     * 界符 || 操作符
     */
    else {
        Lexer *l = new Lexer();
        string name;
        switch(ch) {
            case '(' :
                name.append("("); break; 
            case ')' :
                name.append(")"); break; 
            case '[' :
                name.append("["); break; 
            case ']' :
                name.append("]"); break; 
            case '{' :
                name.append("{"); break; 
            case '}' :
                name.append("}"); break; 
            case ',' :
                name.append(","); break; 
            case ';' :
                name.append(";"); break; 
            case ':' :
                name.append(":"); break; 
            case '+' : 
                l->scan('+') ? name.append("++") : name.append("+"); break;
            case '-' : 
                l->scan('-') ? name.append("--") : name.append("-"); break;
            case '*' :  
                name.append("*"); break; 
            case '&' :  
                name.append("&"); break; 
            case '|' :  
                name.append("|"); break; 
            case '/' : //div
                {
                    name.append("/");
                    if (l->scan('/')) {
                        name = "";
                        readPos = buffer_size-1; //单行注释舍弃
                        l->lexer();
                        return true;
                    } else if (l->scan('*')) {
                        name = "";
                        do
                        {
                            ch = S -> scan();
                            if (ch == '*') {
                                ch = S->scan();
                                if (ch == '/') {
                                    break;
                                }
                            }
                        }while(true);
                        l->lexer();
                        return true;
                    }
                }
                break;
            case '#' :  
                {
                    name = "";
                    readPos = buffer_size-1;
                    l->lexer();
                }
                break;
            case '>' :  l->scan('=') ? name.append(">=") : name.append(">"); break;
            case '<' :  l->scan('=') ? name.append("<=") : name.append("<"); break;
            case '=' :  l->scan('=') ? name.append("==") : name.append("="); break;
            case '!' :  l->scan('=') ? name.append("!=") : name.append("!"); break;
            case '\'':  name.append("\'"); break;
            case '\"':  name.append("\""); break;
            default  :  l->lexer(); break;
        }
        /*switch(ch) {
            case '+' :  token._token = T_ADD; token._name = "+"; break;
            case '-' :  token._token = T_SUB; token._name = "+"; break;
            case '*' :  token._token = T_MUL; token._name = "+"; break;
            case '/' :  
                token._token = T_DIV;
                token._name = "/";
                if (l->scan('/')) {
                    token._token = T_Null;
                    do 
                    {
                        ch  = S->scan();
                    }while(ch != '\n');
                } else if (l->scan('*')) {
                    token._token = T_Null;
                    do
                    {
                        ch = S->scan();
                        if (ch == '*') {
                            ch = S->scan();
                            if (ch == '/')
                                break;
                        }
                    }while(true);
                }
                break;
            case '#' :
                do 
                {
                    ch = S -> scan();
                }while(ch != '\n');
                break;
            case '>' :  if (l->scan('=')) {
                            token._token = T_GE;
                            token._name = ">=";
                        } else {
                            token._token = T_GT;
                            token._name = ">";
                        }
                        break;
            case '<' :  if (l->scan('=')) {
                            token._token = T_LE;
                            token._name = "<=";
                        } else {
                            token._token = T_LT;
                            token._name = "<";
                        }
                        break;
            case '=' :  if (l->scan('=')) {
                            token._token = T_EQU;
                            token._name = "==";
                        } else {
                            token._token = T_ASSIGN;
                            token._name = "=";
                        }
                        break;
            case '!' :  if (l->scan('=')) {
                            token._token = T_NEQU;
                            token._name = "!=";
                        } else {
                            token._token = T_NOT;
                            token._name = "!";
                        }
                        break;
            case '(' :  token._token = T_Lparen; token._name="("; break;
            case ')' :  token._token = T_Rparen; token._name=")";  break;
            case '[' :  token._token = T_Lbrack; token._name="[";  break;
            case ']' :  token._token = T_Rbrack; token._name="]";  break;
            case '{' :  token._token = T_Lbrac;  token._name="{"; break;
            case '}' :  token._token = T_Rbrac;  token._name="}"; break;
            case ',' :  token._token = T_Comma;  token._name=","; break;
            case ';' :  token._token = T_Semicon; token._name=";";  break;
            case ':' :  token._token = T_Colon;  token._name=":"; break;
            //case '\'':  token = S_quote; break; //暂时不考虑
            //case '\"':  token = D_quotes; break;
            default  :  break;
        }*/
        if (name != "") {
            Boundary *B = new Boundary();
            Operator *O = new Operator();
            if (B -> getTokens(name) != T_ERR) {
                if (B -> boundaries[name] == T_D_quotes) {
                    name.clear();
                    do 
                    {
                        ch = S -> scan(); 
                        name += ch;
                    } while(ch != '"');
                    name.pop_back(); // 字符串最后会多出'"', 将其出栈 
                    token._token = T_STRING;
                    token._val = -1;
                    token._name = name;
                } else if (B -> boundaries[name] == T_S_quote) {
                    name.clear();
                    name += S->scan();
                    S->scan();    // 读取剩下单引号
                    //cout << "(char," << name << ')' << endl;
                } else {
                    token._token = B -> getTokens(name);
                    token._val = -1;
                    token._name = B -> getBoundary(token._token);
                }
            } else if(O -> getTokens(name) != T_ERR) {
                token._token = O -> getTokens(name);
                token._val = -1;
                token._name = O -> getOperator(token._token);
            } else {
                cout << "既不是界符， 也不是操作符" << endl;
                token._token = T_ERR;
                token._val = -1;
                token._name = name;
                return false;
            }
            delete B;
            delete O;
        }
        delete l;
        return true;
    }
    return false;
}


/*
ifstream In;
Tokens token = {
    T_Null,
    -1,
    ""
};
int main(int argc, char *argv[])
{
    if (argc < 2) {
        perror("Usage : lexer filename");
        exit(1);
    }
    In.open(argv[1]);
    Lexer *L = new Lexer();
    while(!In.eof()) {
        L -> lexer();
        if (token._name == "") {
            L -> lexer();
        }
        cout << "token_type : " << token._token << '\t' << "token_val : " << token._val << '\t' << "token_name : " << token._name << endl;
    }
}*/
