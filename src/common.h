/*************************************************************************
    > File Name: lexer.h
    > Author: 
    > Mail: 
    > Function: 
    > Remark: 
    > Created Time: 2017年12月02日 星期六 17时26分13秒
    > Last Modified: 2017年12月02日 星期六 17时26分13秒
 ************************************************************************/

#ifndef _lexer_H
#define _lexer_H
#include <fstream>
#include <string>
#include <map>
#include <stack>
enum Tokentype // 所有符号枚举
{
    /**
     * 关键字
     */
    T_MIN, T_INT, T_DOUBLE, T_FLOAT, T_CHAR, T_VOID, T_LONG, T_SHORT, T_MAIN,
    T_EXTERN, T_IF , T_ELSE, T_SWITCH, T_CASE, T_DEFAULT, T_RETURN,
    T_UNION, T_ENUM, T_WHILE, T_FOR, T_DO, T_BREAK, T_CONTINUE, T_STRUCT, T_STRING, T_MAX,

    /**
     * 界符
     */
    T_Lparen, T_Rparen, // ()
    T_Lbrack, T_Rbrack, // []
    T_Lbrac, T_Rbrac, // {}
    T_Comma, T_Colon, T_Semicon, T_S_quote, T_D_quotes, // , : ; ' "

    /**
     * 操作符
     */
    T_ADD, T_SUB, T_MUL, T_DIV, T_MOD, T_INC, T_DEC, T_NEGATE, // +, -, *, /, %, ++, --, ~
    T_NOT, T_AND, T_OR, // !, &&, ||
    T_GT, T_GE, T_LT, T_LE, T_EQU, T_NEQU, T_ASSIGN, // >, >=, <, <=, ==, != , =

    /**
     * 标识符
     */
    T_Null, T_IDENTIFIER, T_NUMBER, // null , identifier, number

    /**
     * 出错
     */
    T_ERR,  // error

};


/**
 * Tokens
 */

struct Tokens
{
    Tokentype _token;
    int _val;
    std::string _name;
};

extern Tokens token;

/**
 * 扫描器
 */
class Scanner
{
    public:
        char scan(); // 读取字符
        void back(); // 异常回退
};


/**
 * 界符
 */

class Boundary
{
    public :
        std::map<std::string, Tokentype> boundaries;
        char ch;
        Boundary();
        Tokentype getTokens(const std::string &);
        std::string getBoundary(const Tokentype &);
};

/**
 * 操作符
 */
 

class Operator
{
    public :
        std::map<std::string, Tokentype> operators;
        Operator();
        Tokentype getTokens(const std::string &);
        std::string getOperator(const Tokentype &);
        ~Operator(){}
};

/**
 * 关键字表
 */

class Keywords
{
    public :
        std::map<std::string, Tokentype> keywords;
        Keywords();
        Tokentype getTokens(const std::string & name);
        std::string getKeyword(const Tokentype &);
        ~Keywords(){};
};

/**
 * 封装扫描方法
 */ 


class Lexer
{
    public :
        bool scan(const char);
        bool lexer();
};

//extern Lexer *L;
extern std::ifstream In;
extern std::ofstream Out;
extern int warnNum; //记录警告
extern int lineNum; 
extern int errorNum; //记录错误数
extern int synerr;  //语法错误个数
extern int semerr;  //语义错误个数
const int TYPE_NUM = 0;
const int TYPE_INTVAR = 1;
extern int vars_count; //标识符个数
struct symbol
{
    std::string name;
    char type;    // 'L' --> local var  'G' --> global var
    int addr;
};
struct msg
{
    int type;
    std::map<std::string, std::string> name;
};
#endif
