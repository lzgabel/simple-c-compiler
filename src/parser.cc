/*************************************************************************
    > File Name: parser.cc
    > Author: 
    > Mail: 
    > Function: 
    > Remark: 
    > Created Time: 2017年12月04日 星期一 16时53分57秒
    > Last Modified: 2017年12月04日 星期一 16时53分57秒
 ************************************************************************/

/**
 * 程序     <program>     ::=  <var-decl>|<func-decl>|<func-def>
 * 变量声明 <var-decl>    ::=  <type> <identifier> ";"
 * 函数声明 <func-decl>   ::=  <type> <identifier> "(" <func-args> ")" ";" 
 * 函数定义 <func-def>    ::=  <type> <identifier> "(" <func-args> ")" <func-body>
 * 函数参数 <func-args>   ::=  <type><identifier> ","
 * 类型     <type>        ::=  "int"
 * 函数体   <func-body>   ::=  <statement>
 * 语句     <statement>   ::=  "{" { <statement> } "}"                  //block
 *                        |   [<type>] <identifier> [ "=" <expr> ] ";"  //assign || local-var-decl
 *                        |   "if" "(" <expr> ")" <statement> [ "else" <statement>]   //if
 *                        |   "while" "(" <expr> ")" <statement>          //while
 *                        |   "print" "(" <func-args> ")" ";" //print
 *                        |   <expr> ";"
 *                        |   "return" <expr> ";"
 * 表达式:
 * 
 * 赋值     <ass-expr>    ::=  <logical-expr> <ass-tail>
 *                        <logical-tail> ::= "=" <logical-expr> <ass-tail> | ^
 * 逻辑     <logical-expr>::= <cmp-expr> <logical-tail>
 *                        <logical-tail> ::= <logicals> <logical-expr> <logical-tail> | ^
 *                        <logicals> ::= "&" | "|"
 * 比较     <cmp-expr>    ::= <alo-expr> <cmp-tail>
 *                        <cmp-tail> ::= <cmps> <alo-expr><cmp-tail> | ^
 *                        <cmps> ::= "==" | "!=" | ">" | ">=" | "<" | "<="
 * 算数     <alo-expr>    ::= <postfix> <alo-tail>
 *                        <alo-tail> ::= <alos-expr> <postfix><alo-tail> | ^
 *                        <alos-expr> ::= "+" | "-" | "*" | "/"
 * 后缀     <postfix-expr>::= <factor-expr> <postfix-tail>
 *                        <postfix-tail> ::= <postfixs-expr><factor><postfix-tail> | ^
 *                        <postfixs-expr> ::= ( <expr> { "," <expr>  }  ) | [ <expr>  ]
 * 因子     <factor-expr> ::= <number> | <identifer> | <string> | "(" <expr> ")" | ^
 * 
 */


#include "common.h"
#include "gen.h"
#include <stdarg.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <stack>
#include <algorithm>
using namespace std;
ifstream In;         //文件输入流
ofstream Out;        //文件输出流
int vars_count = 0;  //变量个数
int msgs_count = 0;  //print 字符串个数
int while_count = 0; //while表达式个数
int if_count = 0;    //if表达式个数
Tokentype while_flag;//while循环里边判断类型
string cmp_flag;     //判断条件前if/while调用
vector<symbol> symbols; //单词符号
stack<msg> msgs;     //输入输出字符串
stack<char>Braces;
map<string, string> msgslist; // .data 段列表
Tokens token = {
    T_Null,
    0,
    ""
};





static void error(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    exit(EXIT_FAILURE);
}

static void debug(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}

symbol * findSymbol(string name)
{
    for (vector<symbol>::iterator it = symbols.begin(); it != symbols.end(); it++) {
        if (it->name == name) {
            return &*it;
        }
    }
    return nullptr;
}

void 
addSymbol(char type, string name, int pos) 
{
    symbol *sym = findSymbol(name);
    if (sym != nullptr) {
        error("\033[41;37m(error)\033[0m\nLine : %d symbol \"%s\" is already defined\n", lineNum, token._name.c_str());
    }
    symbol s;
    s.type = type;
    s.name = name;
    s.addr = pos;
    symbols.push_back(s);
}

class Parser
{
    public :
        Parser();
        bool match(const string &);
        bool next(const string &);
        void check(const string &);
        void parser();
        bool Typename();
        void statement();
        int expr();         //赋值表达式
        int logical_expr(); //逻辑...
        int cmp_expr();     //比较...
        int binary(int type, int (Parser::*func)());
        int alo_expr();     //算术...
        int postfix_expr(); //后缀...
        int factor_expr();  //因子...
        Lexer *L;
};

Parser::Parser()
{
    L = new Lexer();
}
/**
 * get type name 
 */

bool 
Parser::Typename() {
    if (match("int")) {
        L -> lexer();
        return true;
    }
    return false;
}


/**
 * current token = given string ? true : false
 */

bool
Parser::match(const string &str)
{
    return token._name == str;
}

/**
 * next token = given string ? true : false
 */ 
bool 
Parser::next(const string &str) 
{
    if (match(str)) {
        L -> lexer();
        return true;
    } else {
        return false;
    }
}

/**
 * check language syntax
 */
void
Parser::check(const string &str)
{
    if (!next(str)) {
        error("\033[41;37m(error)\033[0m Line : %d checked '%s', but found : \"%s\"\n", lineNum, str.c_str(), token._name.c_str());
    }
}


void
Parser::statement()
{
    if (next("{")) {
        Braces.push('{');
        while(!next("}")) {
            statement();
            if (match("}") && Braces.top() == '{') {
                Braces.pop();
            }
            if (Braces.empty()) break;
        }
    } else if (Typename()) {
        addSymbol('L', token._name, vars_count);
        vars_count ++;
        gen_var_push(token._name);
        L -> lexer();
        if (next("=")) {
            //debug("局部变量赋值\n");
            expr();
            gen_assign();
            //debug("局部变量赋值 end\n");
            //debug(" := \n");
        }
        /*for (auto it = symbols.begin(); it!=symbols.end(); it++) {
            cout << "var : " << it->name << ' ' << "var->addr : " << it->addr << endl;
        }*/
        if (next(",")) {
            for ( ;; ) {
            addSymbol('L', token._name, vars_count);
            vars_count ++;
            gen_var_push(token._name);
                L -> lexer();
                if (next("=")) {
                    expr();
                    //debug(" := \n");
                }
                if (match(";")) {
                    break;
                }
                check(",");
            }
        }
        check(";");
    } else if (next("if")) {
        //  "if" "(" <expr> ")" <statement> //if
        cmp_flag = "if";
        check("(");
        for ( ;; ) {
            expr();
            if (match(")")) {
                break;
            }
        }
        check(")");
        statement();
    } else if (next("while")) {
        cmp_flag = "while";
        //"while" "(" <expr> ")" <statement>
        check("(");
        for ( ;; ) {
            expr();
            if (match(")")) {
                break;
            }
        }
        check(")");
        statement();
        gen_while_end(while_flag);
    } else if (next("read")) {
        check("(");
        // '(' <args> ')'
        for ( ;; ) {
            struct msg m;
            m.type = token._token;
            if (m.type == T_STRING) {
                string s;
                s.append("msg"+(to_string(msgs_count)));
                msgslist[s] = token._name;
                m.name[s] = token._name;
                msgs_count ++;
            } else {
                m.name[token._name] = token._name;
            }
            msgs.push(m);
            if (match(")")) {
                break;
            } else {
                L -> lexer();
                if (match(")")) {
                    break;
                }
                check(",");
            }
        }
        check(")");
        check(";");
        gen_read();
    } else if (next("print")) {
        check("(");
        // '(' <args> ')'
        for ( ;; ) {
            struct msg m;
            m.type = token._token;
            if (m.type == T_STRING) {
                string s;
                s.append("msg"+(to_string(msgs_count)));
                msgslist[s] = token._name;
                m.name[s] = token._name;
                msgs_count ++;
            } else {
                m.name[token._name] = token._name;
            }
            msgs.push(m);
            if (match(")")) {
                break;
            } else {
                L -> lexer();
                if (match(")")) {
                    break;
                }
                check(",");
            }
        }
        check(")");
        check(";");
        gen_print();
    } else if (next("return")) {
        if (!match(";")) {       // eg: return  a+b;
            expr();
        }
        check(";");
		gen_pop(vars_count); /* remove all locals from stack (except return address) */
		gen_ret();
        //debug("RET\n");
    } else {
        expr();
        check(";");
    }
}

int
Parser::expr() {
    int type = logical_expr();
    if (type != TYPE_NUM) {
        if (next("=")) {
            gen_push();
            expr();
            //debug("token: %s", token._name.c_str() );
            if (type == TYPE_INTVAR) {
                gen_assign();
            }
            type = TYPE_NUM;
            //debug(" := \n");
        } else {
            gen_unref(type);
        }
    }
    return type;
}

int
Parser::logical_expr() {
    int type = cmp_expr();
    while (match("|") || match("&")) {
        L -> lexer();
        if (next("|")) {
            cmp_expr();
            //debug(" OR ");
        } else if (next("&")) {
            cmp_expr();
            //debug(" AND ");
        }
    }
    return type;
}

int
Parser::cmp_expr() {
    //int type = rel_expr();
    int type = alo_expr();
    while (match("==") || match("!=") 
           || match(">") || match("<") || match(">=") || match("<="))  {
        if (next("==")) {
            type = binary(type, &Parser::postfix_expr);
            if (cmp_flag == "while") {
                gen_while_start(T_EQU);
                while_flag = T_NEQU;
            } else {
                gen_if_start(T_EQU);
            }
            //debug(" == ");
        } else if (next("!=")) {
            type = binary(type, &Parser::postfix_expr);
            if (cmp_flag == "while") {
                gen_while_start(T_NEQU);
                while_flag = T_NEQU;
            } else {
                gen_if_start(T_NEQU);
            }
            //debug("!=");
        } else if (next(">")) {
            type = binary(type, &Parser::postfix_expr);
            if (cmp_flag == "while") {
                gen_while_start(T_GT);
                while_flag = T_GT;
            } else {
                gen_if_start(T_GT);
            }
            //debug(">");
        } else if (next("<")) {
            type = binary(type, &Parser::postfix_expr);
            if (cmp_flag == "while") {
                gen_while_start(T_LT);
                while_flag = T_LT;
            } else {
                gen_if_start(T_LT);
            }
            //debug("<");
        } else if (next(">=")) {
            type = binary(type, &Parser::postfix_expr);
            if (cmp_flag == "while") {
                gen_while_start(T_GE);
                while_flag = T_GE;
            } else {
                gen_if_start(T_GE);
            }
            //debug(">=");
        } else if (next("<=")) {
            type = binary(type, &Parser::postfix_expr);
            if (cmp_flag == "while") {
                gen_while_start(T_LE);
                while_flag = T_LE;
            } else {
                gen_if_start(T_LE);
            }
            //debug("<=");
        }
    }
    return type;
}

int 
Parser::binary(int type, int (Parser::*func)()) 
{
    if (type != TYPE_NUM) {
        gen_unref(type);
    }
    gen_push();
    type = (this->*func)();
    if (type != TYPE_NUM) {
        gen_unref(type);
    }
    return TYPE_NUM;

}

int
Parser::alo_expr() {
    int type = postfix_expr();
    while (match("+") || match("-") || match("*") || match("/") || match("++") || match("--")) {
        if (next("+")) {
            type = binary(type, &Parser::postfix_expr);
            gen_add();
            //debug(" + ");
        } else if (next("-")) {
            type = binary(type, &Parser::postfix_expr);
            gen_sub();
            //debug(" - ");
        } else if (next("*")) {
            type = binary(type, &Parser::postfix_expr);
            gen_mul();
            //debug(" * ");
        } else if (next("/")) {
            type = binary(type, &Parser::postfix_expr);
            gen_div();
            //debug(" / ");
        } else if (next("++")) {
            gen_inc(); 
        } else if (next("--")) {
            gen_dec();
        }
    }
    return type;
}


int
Parser::postfix_expr() {
    int type = factor_expr();
    if (next("[")) {
        expr();
        check("]");
        //debug(" [] ");
    } else if (next("(")) {
		int prev_vars_count = vars_count;
        for ( ;; ) {
            if (match(")")) {  //  eg: func() 无参
                break;
            } else {           //  eg: func(int, int, ...); 有参
                L -> lexer();
                if (match(")")) {
                    break;
                }
                check(",");
            }
        }
        check(")");
        gen_call_function();
        gen_pop(vars_count-prev_vars_count);
        type = TYPE_NUM;
    }
    return type;
}

int
Parser::factor_expr() {
    int type = TYPE_NUM;
    if (token._token == T_NUMBER) {
        gen_int_push(token._val); 
    } else if (token._token == T_IDENTIFIER) {
        symbol *s = findSymbol(token._name);
        if (s == nullptr) {
            error("\033[41;37m(error)\033[0m\nLine : %d \"%s\": undeclared (first use in this function)\n", lineNum, token._name.c_str());
        }
        if (s->type == 'L') {
            gen_stack_addr(s->addr);
        } else if (s->type == 'G'){
            gen_var_push(token._name);
        }
        type = TYPE_INTVAR;
    } else if (next("(")) {
        type = expr();
        check(")");
        return type;
    } else {
        error("\033[41;37m(error)\033[0m\nLine : %d Unchecked  expression : \"%s\"\n", lineNum, token._name.c_str());
    }
    L -> lexer();
    return type;
}

void 
Parser::parser()
{
    while(L->lexer()) {
        if (token._name == "") {
            continue;
        }
        //cout << "token: " << token._name << endl;
        //continue;
        if (!Typename()) {                   // 声明类型
            error("\033[41;37m(error)\033[0m Line : %d type name checked\n", lineNum);
        }
        addSymbol('G', token._name, -1);
        gen_function(token._name);
        //debug("identifier: %s\n", token._name.c_str());   //标识符
        L -> lexer();
        /**
         * next(";") ? 变量 : 函数
         */
        if (next(";")) {
        //   debug("Variable defintion\n");
            continue;
        }
        /**
         * 函数参数列表  start
         */
        check("(");
        // '(' <args> ')'
        for ( ;; ) {
            if (match(")")) {  //  eg: func() 无参
                break;
            } else {           //  eg: func(int, int, ...); 有参
                if (Typename()) {
                    //debug("Function argument: %s\n", token._name.c_str());
                }
                L -> lexer();
                if (match(")")) {
                    break;
                }
                check(",");
            }
        }
        check(")");
        /**
         * 函数参数列表  end
         */
        
        /**
         * !match(";") ? 函数体 : 声明|func-call  start
         */
        if (!next(";")) {
            //debug("function body\n");
            //return;
            statement();
        }
        /**
         * !match(";") ? 函数体 : 声明|func-call  end
         */
    }
}
int 
main(int argc,char *argv[])
{
    if (argc < 2 || string(argv[1]) == "--help") {
        cerr << "Usage : ./parser [src_file] [dest_file] [--help]" << endl;
        exit(1);
    }
    if (string(argv[1]) == "--help") {
        cout << "Usage : ./parser [src_file] [dest_file] [--help]" << endl;
        cout << "Compiler : gcc -m32 [src_file] [-o] [dest_file]" << endl; 
    }
    In.open(argv[1]);
    if (argc == 3) {
        Out.open(argv[2]);
    } else {
        Out.open("a.s");
    }
    Parser *P = new Parser();
    P -> parser();
    gen_pop(vars_count);
    gen_ret();
    gen_finish();
    Out.close();
    return 0;
}
