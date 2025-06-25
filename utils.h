#pragma once

#include <string>
#include <fstream>
#include <vector>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace utils
{
	std::string readTextFromFile(const std::string& file);
	
	std::string sha1(const std::vector<char>& vals);

	std::vector<char> readBinaryFromFile(const std::string& file);

	void writeBinaryToFile(const std::string& file, const std::vector<char>& data);

	std::filesystem::path join(const std::string& file, const std::vector<std::string>& others);

	void writeTextToFile(const std::string& file, const std::string& text,bool overwrite);
	
};

