#pragma once

#include <sstream>
#define _CRT_SECURE_NO_WARNINGS
#include <chrono>
#include <iomanip>
#include <unordered_map>
#include <string>
#include <vector>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/string.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/access.hpp>





#include "utils.h"
class commit
{
public:
	commit();
	commit(const std::string& msg, const std::unordered_map<std::string, std::string>& blobMap, const std::string& parent);

	std::string getOwnHash();
	std::string calcHash();
	std::string getParentHash();
	std::string getMessage();
	std::string getDatetime();
	std::unordered_map<std::string, std::string> getBlobs();
	std::string globalLog();

	
private:
	std::string message;
	std::string parentHash;
	std::string datetime;
	std::string ownHash;
	std::unordered_map<std::string, std::string> blobs;

	friend class boost::serialization::access;

	template <class Archive>
	void serialize(Archive& ar, const unsigned int version) {
		ar& message;
		ar& blobs;
		ar& parentHash;
		ar& datetime;
		ar& ownHash;
	}
	std::string currentDateTime();
};


