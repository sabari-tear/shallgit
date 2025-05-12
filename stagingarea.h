#pragma once
#include<string>
#include <unordered_map>
class stagingarea
{
public:
	stagingarea();
	
	void add(const std::string& filename, const std::string& sha1);
	void addToRemovedFiles(const std::string& filename);
	void clear();
	std::unordered_map<std::string, std::string> getAddedFiles();
	std::vector<std::string> getRemovedFiles();

	std::unordered_map<std::string, std::string> addedFiles;
	std::vector<std::string> removedFiles;
};

