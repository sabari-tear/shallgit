#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#define _CRT_SECURE_NO_WARNINGS
#include "commit.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <fstream>
#include <sstream>
#include <chrono>
commit::commit() {

}

commit::commit(const std::string& msg, const std::unordered_map<std::string, std::string>& blobMap, const std::string& parent)
	:message(msg), blobs(blobMap), parentHash(parent) {
	datetime = currentDateTime();
	ownHash = calcHash();
}

std::string commit::getOwnHash() {
	return ownHash;
}

std::string commit::getParentHash() {
	return parentHash;
}

std::string commit::getMessage() {
	return message;
}

std::string commit::getDatetime() {
	return datetime;
}

std::unordered_map<std::string, std::string> commit::getBlobs() {
	return blobs;
}

std::string commit::globalLog() {
	std::ostringstream log;
	log << "====\n"
		<< "Commit " << ownHash << '\n'
		<< datetime << '\n'
		<< message << "\n\n";
	return log.str();
}

std::string commit::currentDateTime() {
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
	return ss.str();
}

std::string commit::calcHash() {
	std::ostringstream archive_stream;
	boost::archive::text_oarchive archive(archive_stream);
	archive << *this;
	std::string serializedCommit = archive_stream.str();
	std::vector<char> commitData(serializedCommit.begin(),serializedCommit.end());
	return utils::sha1(commitData);
}
