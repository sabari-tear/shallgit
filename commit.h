#pragma once

#include <unordered_map>
#include <string>
class commit
{
public:
	std::string message;
	std::unordered_map<std::string, std::string> blobs;
	std::string parentHash;
	std::string datetime;
	std::string ownHash;

	commit();
	commit(const std::string& msg, const std::unordered_map<std::string, std::string>& blobMap, const std::string& parent);

	std::string getOwnHash();
	std::string calcHash();
	std::string getParentHash();
	std::string getMessage();
	std::string getDatetime();
	std::unordered_map<std::string, std::string> getBlobs();
	string globalLog();
};

