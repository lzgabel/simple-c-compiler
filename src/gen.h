/*************************************************************************
    > File Name: gen.h
    > Author: 
    > Mail: 
    > Function: 
    > Remark: 
    > Created Time: 2017年12月08日 星期五 23时39分38秒
    > Last Modified: 2017年12月08日 星期五 23时39分38秒
 ************************************************************************/

#ifndef _GEN_H
#define _GEN_H
#include "common.h"
#include <vector>
#include <string>
void gen_add();
void gen_inc();
void gen_sub();
void gen_dec();
void gen_mul();
void gen_div();
void gen_int_push(int val);
void gen_var_push(std::string var);
void gen_if_start(Tokentype t);
void gen_while_start(Tokentype t);
void gen_while_end(Tokentype t);
void gen_unref(int type);
void gen_stack_addr(int n);
void gen_push();
void gen_assign();
void gen_finish();
void gen_print();
void gen_read();
void gen_pop(int n);
void gen_ret();
void gen_function(std::string func);
void gen_call_function();
extern std::vector<symbol> symbols;
extern std::stack<msg> msgs;
extern std::map<std::string, std::string> msgslist;
extern int vars_count;
extern int stack_pos;
extern int while_count;
extern int if_count;
extern std::ofstream Out;
#endif
