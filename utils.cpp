#include "utils.h"
#include <openssl/sha.h>
using namespace std;

string utils::readTextFromFile(string& path) {
	ifstream file(path);
	return {istreambuf_iterator<char>(file), istreambuf_iterator<char>()};
}

string utils::sha1(vector<char>& vals) {
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1(reinterpret_cast<const unsigned char*> (vals.data()), vals.size(), hash);

}