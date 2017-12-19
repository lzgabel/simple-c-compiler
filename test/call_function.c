int func()
{
    int a = 1;
    a = a*10;
    if (a == 10) 
    print("a : %d\n", a);
    return a;
}
int main()
{
    int b = func();
    print("%d\n", b);
    return 0;
}
