#pragma once
#include<string>
#include <unordered_map>
#include <sstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
class stagingarea
{
public:
	stagingarea();
	
	void add(const std::string& filename, const std::string& sha1);
	void addToRemovedFiles(const std::string& filename);
	void clear();
	
	std::unordered_map<std::string, std::string> getAddedFiles();
	std::vector<std::string> getRemovedFiles();
	
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version);

	template<class Archive>
	void deserialize(Archive& ar, const unsigned int version);

	std::string serializeToString();
	void deserilaizeFromString(const std::string& str);

	std::unordered_map<std::string, std::string> addedFiles;
	std::vector<std::string> removedFiles;
};

