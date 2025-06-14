#pragma once
#include<string>
#include <unordered_map>
#include <sstream>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/string.hpp>

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


	std::string serializeToString();
	void deserilaizeFromString(const std::string& str);
	
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& addedFiles;
		ar& removedFiles;
	}

	template<class Archive>
	void deserialize(Archive& ar, const unsigned int version) {
		ar& addedFiles;
		ar& removedFiles;
	}
	std::unordered_map<std::string, std::string> addedFiles;
	std::vector<std::string> removedFiles;
};

