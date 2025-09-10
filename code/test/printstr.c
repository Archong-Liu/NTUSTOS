#include "syscall.h"

main()
{
    int len;
    len = Print("Hello Nachos2025!\n");
    PrintInt(len);
    len = Print("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz\n");
    PrintInt(len);
}