#pragma once

#include <string>
#include <fstream>
#include <vector>
using namespace std;

namespace utils
{
	string readTextFromFile(string& file);
	string sha1(vector<char>& vals);
};

