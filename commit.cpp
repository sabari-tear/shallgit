#include "commit.h"
commit::commit() {

}

commit::commit(const std::string& msg, const std::unordered_map<std::string, std::string>& blobMap, const std::string& parent)
	:message(msg), blobs(blobMap), parentHash(parent) {
	datetime = "";//currentDateTime();
	ownHash = ""; //calcHash();
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
