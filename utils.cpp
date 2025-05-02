#include "utils.h"

using namespace std;

string utils::readTextFromFile(string& path) {
	ifstream file(path);
	return {istreambuf_iterator<char>(file), istreambuf_iterator<char>()};
}