#include <stdio.h>
#include <signal.h>
#include "chat.h"

Chat chat("../chat/html/head", "../chat/html/foot", "../chat/html/forms");

static void handler (int sig, siginfo_t *siginfo, void *context) {
    std::cout << "Signal number: " << sig << std::endl;
    chat.stop();
}

void replaceSignals(){
    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_sigaction = &handler;
    act.sa_flags = SA_SIGINFO;
    if (sigaction(SIGHUP, &act, NULL) < 0 ||
        sigaction(SIGINT, &act, NULL) < 0 ||
        sigaction(SIGQUIT, &act, NULL) < 0 ||
        sigaction(SIGTERM, &act, NULL) < 0) {
        std::cerr << "Can't replace signals.\n";
        throw;
    }
}

int main() {
    replaceSignals();
    try {
        chat.addPort(4321);
        chat.start();
    } catch (std::runtime_error &e){
        std::cerr << "Can't add port to chat: " << e.what() << std::endl;
    }
}
