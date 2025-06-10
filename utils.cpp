#include "utils.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

using namespace std;

string utils::readTextFromFile(const string& path) {
	ifstream file(path);
	return {istreambuf_iterator<char>(file), istreambuf_iterator<char>()};
}

string utils::sha1(vector<char>& vals) {
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1(reinterpret_cast<const unsigned char*> (vals.data()), vals.size(), hash);

	std::stringstream ss;
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
	}
	return ss.str();
}