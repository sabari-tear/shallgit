#include "commit.h"

commit::commit(string& msg, unordered_map<string, string>& blobMap, string& parent) 
	: message(msg), blobs(blobMap), parentHash(parent) {
	datetime = ""//currentDateTime();
	ownHash = "" //calcHash();
}