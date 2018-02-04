#include <stdio.h>

int main(void)
{
    int selector_ldt =0;
    selector_ldt += 1 << 3;
    selector_ldt += 1 << 3;
    printf("%d\n",selector_ldt);
    return 0;
}