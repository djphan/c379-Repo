#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void ouch(int sig) {
    printf("OUCH recieved signal %d\n", sig);
    (void) signal(SIGINT, SIG_DFL);
}

int main(int argc, const char *argv[]) {
    struct sigaction act;
    act.sa_handler = ouch;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    (void) sigaction(SIGINT, &act, 0);

    while(1) {
        printf("Hello World!\n");
        sleep(1);
    }
    return 0;
}
