#include <signal.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>

static sigjmp_buf env;	                //set up the buffer for using in setjmp

void my_handler(int signo) {
    printf("Caught SIGSEGV\n");
    siglongjmp(env, 1);
}

int main() {
    printf("Program Starting\n");
    //register my_handler() for handling SIGSEGV
    signal(SIGSEGV, my_handler);        
    int count = 0;
	// if 0,it acts as setjump and long jump
	// 1 tells compiler to store masks
    int x = sigsetjmp(env,1);

    if(x == 0) {
        printf("setjmp called first time\n");
        char *m = "Hello";
        //cause a SIGSEGV (eqv to raise(SIGSEGV)
        m[2] = 'z';		
    } else {
        printf("setjmp called from longjmp\n");
        count++;
        if(count == 5) {
            exit(1);
        }
        else{
            char *m = "Hello";
            //cause a SIGSEGV
            m[2] = 'z';		
        }
    }

    return 0;
}
