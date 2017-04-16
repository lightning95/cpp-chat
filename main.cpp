#include <stdio.h>
#include <signal.h>
#include "chat.h"

struct action {
    int sig;

    action(){}

    action(int sig, struct sigaction n)
        : sig(sig) {
        if (sigaction(sig, &n, NULL) < 0)
            throw std::runtime_error("Can't replace signal " + std::to_string(sig) + "\n");
    }

    ~action(){
        if (signal(SIGINT, SIG_DFL) == SIG_ERR)
            std::cerr << "Can't replace signal back " << sig << "\n";
    }
};

Chat* ref = NULL;
action** actions;

static void handler (int sig, siginfo_t *siginfo, void *context) {
   std::cout << "Signal number: " << sig << std::endl;
   ref->stop();
}

void replaceSignals(){
    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_sigaction = &handler;
    act.sa_flags = SA_SIGINFO;// | SA_RESETHAND; // returns to default after it had been called

    actions = new action*[4];
    actions[0] = new action(SIGHUP, act);
    actions[1] = new action(SIGINT, act);
    actions[2] = new action(SIGQUIT, act);
    actions[3] = new action(SIGTERM, act);

    std::cout << "Signals replaced\n";
}

int main() {
    try {
        Chat chat("../cpp-chat/html/head", "../cpp-chat/html/foot", "../cpp-chat/html/forms");
        ref = &chat;
        replaceSignals();
        chat.addPort(4321);
        chat.start();
    } catch (std::runtime_error &e){
    }
    ref = NULL;
    for (int i = 0; i < 4; ++i)
        delete actions[i];
    delete[] actions;
}
