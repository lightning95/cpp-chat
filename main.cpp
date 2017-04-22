#include <stdio.h>
#include <signal.h>
#include <vector>

#include "chat.h"

using namespace std;

struct action {
    static const int NONE = -1;

    int sig;

    action()
        :sig(NONE) {
    }

    action(int sig, struct sigaction n)
        : sig(sig) {
        if (sigaction(sig, &n, NULL) < 0)
            throw std::runtime_error("Can't replace signal " + std::to_string(sig) + "\n");
    }

    action(action && rhs)
        : sig(rhs.sig){
        rhs.sig = NONE;
    }

    action& operator=(action&& rhs){
        swap(rhs);
        return *this;
    }

    void swap(action& rhs){
        std::swap(sig, rhs.sig);
    }

    ~action(){
        if (sig == NONE) return;
        std::cout << "removing signal " << sig << "\n";
        if (signal(SIGINT, SIG_DFL) == SIG_ERR)
            std::cerr << "Can't replace signal back " << sig << "\n";
    }

    action(action const&) = delete;
    action& operator=(action const&) = delete;
};

Chat* refChat = NULL;

static void handler (int sig, siginfo_t *siginfo, void *context) {
   std::cout << "Signal number: " << sig << std::endl;
   refChat->stop();
}

void replaceSignals(vector<action>& actions){
    struct sigaction act;
    memset (&act, '\0', sizeof(act));
    act.sa_sigaction = &handler;
    act.sa_flags = SA_SIGINFO;// | SA_RESETHAND; // returns to default after it had been called

    actions.emplace_back(SIGHUP, act);
    actions.emplace_back(SIGINT, act);
    actions.emplace_back(SIGQUIT, act);
    actions.emplace_back(SIGTERM, act);

    std::cout << "Signals replaced\n";
}

struct Setter{
    Chat*& old;

    Setter(Chat*& o, Chat& c)
        : old(o) {
        old = &c;
    }

    ~Setter(){
        old = NULL;
    }
};

int main() {
    try {
        Chat chat(":/html/head", ":/html/foot", ":/html/forms");
        Setter setter(refChat, chat);

        vector<action> actions;
        replaceSignals(actions);

        chat.addPort(4321);
        chat.start();
    } catch (std::runtime_error &e){
    }
}
