#include "gen.h"
#include "common.h"
#include <iostream>
using namespace std;
#define TYPE_NUM_SIZE 4

void gen_unref(int type) {
    /**
     * 取出对应变量值，间接寻址，通过eax中变量地址去访问地址对应值
     */
	if (type == TYPE_INTVAR) {
		Out << "\tmovl (%eax), %eax\n";
    }
}
void gen_push() {
	Out << "\tpushl %eax\n";
}


void gen_stack_addr(int addr) 
{
    /**
     * 获取当前变量位置, 通过基地址查询
     */
    Out <<  "\tmov %esi, %eax\n" <<  "\tadd $" << addr*TYPE_NUM_SIZE << ", %eax\n";
}

void gen_add()
{
    /**
     * addl src, dest       src与dest的加法, 结果在dest中
     */
    Out << "\tpopl %ebx\n\taddl %ebx, %eax\n";
}

void gen_inc()
{
    /**
     * 自加
     */
    Out << "\tincl (%eax)\n";
}

void gen_dec()
{
    /**
     * 自减
     */
    Out << "\tdecl (%eax)\n";
}

void gen_call_function()
{
    /**
     * 调用函数
     */
    Out << "\tpopl %ebx\n";
    Out << "\tcall *%ebx\n";
}

void gen_function(string func)
{
    /**
     * 函数段
     */

    if (func == "main") {
        Out << ".align 4\n";
        Out << ".globl main\n";
        Out << "main:\n";
        Out << '\t' << "push %ebp\n";
        Out << '\t' << "movl %esp, %ebp\n";
    } else {
        Out << ".globl " << func << "\n";
        Out << ".type " << func << ", @function\n";
        Out << func << ":\n";
        Out << '\t' << "push %ebp\n";
        Out << '\t' << "movl %esp, %ebp\n";
    }
}


void gen_int_push(int val)
{
	 Out <<  "\tmovl $"<< val << ", %eax\n";
}

void gen_if_start(Tokentype t)
{
    /**
     * cmpl right_op lift_op
     */ 
    Out << "\tpopl %ebx\n";
    Out << "\tcmpl %eax, %ebx\n"; //当前%eax为边界
    switch(t)
    {
        case T_GT:
                Out << string("\tjg ")+string("__if")+to_string(if_count) << endl;
                break;
        case T_GE:
                Out << string("\tjge ")+string("__if")+to_string(if_count) << endl;
                break;
        case T_LT:
                Out << string("\tjl ")+string("__if")+to_string(if_count) << endl;
                break;
        case T_LE:
                Out << string("\tjle ")+string("__if")+to_string(if_count) << endl;
                break;
        case T_EQU:
                Out << string("\tje ")+string("__if")+to_string(if_count) << endl;
                break;
        case T_NEQU:
                Out << string("\tjne ")+string("__if")+to_string(if_count) << endl;
                break;
        default:
            break;
    }
    Out << string("__if")+to_string(if_count)+":\n";
}

void gen_while_start(Tokentype t)
{
    /**
     * cmpl right_op lift_op
     */ 
    Out << "\tpopl %ebx\n";
    Out << "\tcmpl %eax, %ebx\n"; //当前%eax为边界
    Out << "\tmovl %eax, %edx\n"; //边界暂存
    switch(t)
    {
        case T_GT:
                Out << string("\tjg ")+string("__while")+to_string(while_count) << endl;
                break;
        case T_GE:
                Out << string("\tjge ")+string("__while")+to_string(while_count) << endl;
                break;
        case T_LT:
                Out << string("\tjl ")+string("__while")+to_string(while_count) << endl;
                break;
        case T_LE:
                Out << string("\tjle ")+string("__while")+to_string(while_count) << endl;
                break;
        case T_EQU:
                Out << string("\tje ")+string("__while")+to_string(while_count) << endl;
                break;
        case T_NEQU:
                Out << string("\tjne ")+string("__while")+to_string(while_count) << endl;
                break;
        default:
            break;
    }
    Out << string("__while")+to_string(while_count)+":\n";
}
void gen_while_end(Tokentype t)
{
    Out << "\tcmpl %edx, %eax\n"; //%edx 为边界
    switch(t)
    {
        case T_GT:
                Out << string("\tjg ")+string("__while")+to_string(while_count) << endl;
                break;
        case T_GE:
                Out << string("\tjge ")+string("__while")+to_string(while_count) << endl;
                break;
        case T_LT:
                Out << string("\tjl ")+string("__while")+to_string(while_count) << endl;
                break;
        case T_LE:
                Out << string("\tjle ")+string("__while")+to_string(while_count) << endl;
                break;
        case T_EQU:
                Out << string("\tje ")+string("__while")+to_string(while_count) << endl;
                break;
        case T_NEQU:
                Out << string("\tjne ")+string("__while")+to_string(while_count) << endl;
        default:
            break;
    }
    Out << string("__while_end")+to_string(while_count)+":\n";
    while_count ++;
}
void gen_cmp_gt()
{
    Out << "\tpopl %ebx\n";
    Out << "\tcmpl %eax, %ebx\n";
    Out << "\tjg __w_end\n";
    Out << "__while:\n";
    Out << "\tinc %ebx\n";
    Out << "\tcmpl %eax, %ebx\n";
    Out << "\tjl __while\n";
    Out << "__w_end:\n";

}

void gen_var_push(string var)
{
    if (vars_count == 1) {   //以第一个变量为栈底基准
        Out << "\tmovl $" << var << ", %esi\n";
    }
    Out <<  "\tmovl $" << var << ", %eax\n";
    gen_push();
}

void gen_assign()
{
    Out << "\tpopl %ebx\n\tmovl %eax, (%ebx)\n";
}

void gen_sub()
{
    /**
     * subl src, dest  src与dest的减法, 结果在dest中
     */
    Out << "\tpopl %ebx\n\tsubl %ebx, %eax\n\tneg %eax\n";
}

void gen_mul() 
{
    /**
     * imul src, dest  src与dest的乘积, 结果在dest中
     */
    Out << "\tpopl %ebx\n\timul %ebx, %eax\n";
}
void gen_div() 
{
    /**
     * movl %eax, %ecx //除数放于ecx中暂存
     * popl %eax //获取被除数放于eax中
     * cdq  //extend eax into edx
     *      假设 EAX 是 FFFFFFFB (-5) ，它的第 31 bit (最左边) 是 1，
     *      执行 CDQ 后， CDQ 把第 31 bit 复制至 EDX 所有 bit
     *      EDX 变成 FFFFFFFF
     *      这时候， EDX:EAX 变成 FFFFFFFF FFFFFFFB ，它是一个 64 bit 的大型数字，数值依旧是 -5。
     * movl %ecx, %ebx  //除数放于ebx
     * idiv %ebx  //值放于eax, 余数放于edx
     */

    Out <<  "\tmovl %eax, %ecx\n\tpopl %eax\n\tcdq\n\tmovl %ecx, %ebx\n\tidivl %ebx\n";
}
void gen_read()
{
    /**
     * 将终端输入读入对应变量对应地址中(call scanf from libc.so)
     */
    while(!msgs.empty()) {
        msg m = msgs.top();
        for (auto it = m.name.begin(); it != m.name.end(); it++) {
            if (m.type == T_STRING) {
                Out <<"\tpushl $" << it->first << endl;
            } else if (m.type == T_IDENTIFIER) {
               Out << "\tpushl $" << it->first << endl; 
            }
        }
        msgs.pop();
    }
    Out <<  "\tcall scanf\n";
}

void gen_print()
{
    while(!msgs.empty()) {
        msg m = msgs.top();
        for (auto it = m.name.begin(); it != m.name.end(); it++) {
            if (m.type == T_STRING) {
                Out <<"\tpushl $" << it->first << endl;
            } else if (m.type == T_IDENTIFIER) {
               Out << "\tpushl " << it->first << endl; 
            }
        }
        msgs.pop();
    }
    Out <<  "\tcall printf\n";
}
void gen_finish() 
{
    /**
     * .bss 未初始化数据段
     */
    Out << ".section .bss\n";
    for (vector<symbol>::iterator it = symbols.begin(); it != symbols.end(); it++) {
        if (it->type == 'L')
        Out <<  "\t.lcomm " << it->name  << ',' << TYPE_NUM_SIZE << endl; 
    }

    /**
     * .data 静态数据区
     */
    Out << ".section .data\n";
    for (map<string, string>::iterator it = msgslist.begin(); it != msgslist.end(); it++) {
        Out << it->first << ":\n\t" << ".string\t" << "\"" << it->second << "\"" << endl;
    }
}

void gen_pop(int n)
{
    /**
     * 变量出栈
     */

    if (n > 0) {
        Out << "\tadd $" << (n)*TYPE_NUM_SIZE << ", %esp\n";
    }
}

void gen_ret()
{
    /**
     * 返回, 现场返回
     */
    Out << "\tleave\n";
    Out << "\tret\n";
}
