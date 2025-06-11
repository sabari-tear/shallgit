#include <iostream>
#include <string>
#include <vector>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include "sgit.h"
using namespace std;
int main(int argc, char* argv[])
{
    sgit s;
    vector<string> cmd;
    for (int i = 1; i < argc; i++) {
        cmd.push_back(argv[i]);
	}

    if (cmd.empty())
        cout << "shallgit needs command" << '\n';
    else
    {
        string op = cmd[0];

        if (op == "init") {
			s.init();
            cout << "git init hub moment" << '\n';
        }
        else if (op == "add") {
            cout << "git add ah moment" << '\n';
        }
        else if (op == "commit") {
            cout << "git committing" << '\n';
        }
        else if (op == "log") {
            cout << "git log showing" << '\n';
         }
    }
}
