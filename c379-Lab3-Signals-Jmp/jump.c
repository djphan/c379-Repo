#include <stdio.h>
#include <setjmp.h>

static jmp_buf env;

void second(void)
{
    printf("Second!\n");
    longjmp(env, 1);
}

void first(void)
{
    second();
    printf("First\n");
}

int main(void)
{
    if(!setjmp(env)) {
		// When longjmp is executed, it replaces env with 1
		// Therefore we continue with !1
        printf("JUMP SET\n");
        first();s
    } else {
        printf("In Main\n");
    }
    return 0;
}
