#pragma once

#include <string>
#include <fstream>
#include <vector>
using namespace std;

namespace utils
{
	string readTextFromFile(const string& file);
	
	string sha1(const vector<char>& vals);
	string sha1(const string& str);
	string sha1(const experimental::filesystem::path& path);

	bool restrictedDelete(const std::string& file);

	vector<char> readBinaryFromFile(const string& file);

	void writeBinaryToFile(const string& file, const vector<char>& data);

	experimental::filesystem::path join(const string& file, const vector<string>& others);

	void writeTextToFile(const string& file, const string& text,bool overwrite);
	
};

