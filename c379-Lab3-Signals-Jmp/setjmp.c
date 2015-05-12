#include <signal.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

static sigjmp_buf env;

void sig_handler(int signo) {
    printf("In sig_handler\n");
    siglongjmp(env, 1);
}

int main() {
    (void) signal(SIGSEGV, sig_handler);

    if(!sigsetjmp(env, 1)) {
        printf("Jump marker set\n");
    }// else {
     //   printf("Custom error: Segmentation fault occurred\n");
     //   exit(-1);
    //}

    char *str = "Hello";
    str[2] = 'Z';
}
