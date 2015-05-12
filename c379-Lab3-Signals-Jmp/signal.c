#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void ouch(int sig) {
    printf("OUCH recieved signal %d\n", sig);
    (void) signal(SIGINT, SIG_DFL);
}

int main(int argc, const char *argv[]) {
    (void) signal(SIGINT, ouch);

    while(1) {
        printf("Hello World!\n");
        sleep(1);
    }
    return 0;
}
