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

std::unordered_map<std::string, std::string> commit::getBlobs(){
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
	//getting the current time point
	auto now = std::chrono::system_clock::now();
	//converting the time point into c-standard time_t format
	auto in_time_t = std::chrono::system_clock::to_time_t(now);
	
	std::tm tm_buf;
	localtime_s(&tm_buf, &in_time_t);  // safe, thread-safe version

	//format the time_t to 2025-06-18 13:43:02
	std::stringstream ss;
	ss << std::put_time(&tm_buf, "%Y-%m-%d %X");
	return ss.str();
}

std::string commit::calcHash() {
	//output string stream
	std::ostringstream archive_stream;

	//this line init the output text archive that use the above stream
	boost::archive::text_oarchive archive(archive_stream);
	//sending the current object so it will serialize the object in to text since it is text archive
	archive << *this;
	
	// get the serialized text
	std::string serializedCommit = archive_stream.str();
	// covert the string to individual char for easy processing
	std::vector<char> commitData(serializedCommit.begin(),serializedCommit.end());
	//return sha1 hash value of the string
	return utils::sha1(commitData);
}
