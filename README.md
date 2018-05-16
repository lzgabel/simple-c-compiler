# simple-c-compiler
c语言子集的一个小型编译器（更新中。。 主要是太菜）
## 使用
* src中make编译
* ./parser [src_file] [dest_file] (eg: ./parser a.c a.s)
* gcc -m32 -o a.out a.s
* ./a.out
## 查看编译情况
* gcc -v -m32 -o a.out a.s
* 由编译信息可知  
1)  gcc 采用 as --32 对汇编进行编译   
2)  [collect2(对ld进行封装)](http://gcc.gnu.org/onlinedocs/gccint/Collect2.html)   
进行链接 因此单独使用 ld 链接时无法正常执行
* [collect2与ld区别](https://blog.csdn.net/yuyin86/article/details/8122666)
