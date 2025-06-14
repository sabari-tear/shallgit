#include <iostream>
#include <string>
#include <vector>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include "sgit.h"

using namespace std;

bool inputChecker(int expectedLength, const vector<string>& args) {
    if (args.size() == expectedLength) {
        return true;
    }
    cout << "Incorrect Operands" << endl;
    return false;
}

int main(int argc, char* argv[]) {
    sgit s;
    vector<string> args(argv + 1, argv + argc);

    if (args.empty()) {
        cout << "Please enter a command." << endl;
    }
    else {
        string command = args[0];

        if (command == "init") {
            if (inputChecker(1, args)) {
                s.init();
            }
        }
        else if (command == "add") {
            if (inputChecker(2, args)) {
                s.add(args[1]);
            }
        }
        else if (command == "commit") {
            if (inputChecker(2, args)) {
                s.commitment(args[1]);
            }
        }
        else if (command == "rm") {
            if (inputChecker(2, args)) {
                s.rm(args[1]);
            }
        }
        else if (command == "log") {
            if (inputChecker(1, args)) {
                s.log();
            }
        }
        else if (command == "global-log") {
            if (inputChecker(1, args)) {
                s.global();
            }
        }
        else if (command == "find") {
            if (inputChecker(2, args)) {
                s.find(args[1]);
            }
        }
        else if (command == "status") {
            if (inputChecker(1, args)) {
                s.status();
            }
        }
        else if (command == "checkout") {
            if (args.size() == 2) {
                s.checkout({ args.begin() + 1, args.end() });
            }
            else if (args.size() == 3 && args[1] == "--") {
                vector<string> checkoutArgs = { args[1], args[2] };
                s.checkout(checkoutArgs);
            }
            else if (args.size() == 4 && args[2] == "--") {
                vector<string> checkoutArgs = { args[1], args[2], args[3] };
                s.checkout(checkoutArgs);
            }
            else {
                cout << "Incorrect Operands" << endl;
            }
        }
        else if (command == "branch") {
            if (inputChecker(2, args)) {
                s.branch(args[1]);
            }
        }
        else if (command == "rm-branch") {
            if (inputChecker(2, args)) {
                s.rmb(args[1]);
            }
        }
        else if (command == "reset") {
            if (inputChecker(2, args)) {
                s.reset(args[1]);
            }
        }
        else if (command == "merge") {
            if (inputChecker(2, args)) {
                s.merge(args[1]);
            }
        }
        else {
            cout << "No command with that name exists." << endl;
        }
    }

    return 0;
}
