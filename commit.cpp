#include "commit.h"
commit::commit() {

}

commit::commit(const std::string& msg, const std::unordered_map<std::string, std::string>& blobMap, const std::string& parent)
	:message(msg), blobs(blobMap), parentHash(parent) {
	datetime = "";//currentDateTime();
	ownHash = ""; //calcHash();
}