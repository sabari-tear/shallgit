#include "utils.h"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

#include <experimental/filesystem>
using namespace std;

string utils::readTextFromFile(const string& path) {
	ifstream file(path);
	return {istreambuf_iterator<char>(file), istreambuf_iterator<char>()};
}

string utils::sha1(const vector<char>& vals) {
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1(reinterpret_cast<const unsigned char*> (vals.data()), vals.size(), hash);

	std::stringstream ss;
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
		ss << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
	}
	return ss.str();
}

vector<char> utils::readBinaryFromFile(const string& path) {
	ifstream file(path, ios::binary | ios::ate);
	if (!file) {
		throw runtime_error("Could not open file: " + path);
	}

	auto end = file.tellg();
	file.seekg(0, ios::beg);

	vector<char> bytes(end - file.tellg());
	if (!file.read(bytes.data(), bytes.size())) {
		throw runtime_error("Could not read file: " + path);
	}
	return bytes;
}

void utils::writeBinaryToFile(const string& path, const vector<char>& bytes) {
	ofstream ofs(path, ios::binary);
	if (!ofs) {
		throw runtime_error("Could not open file for writing: " + path);
	}
	ofs.write(bytes.data(), bytes.size());
	if (!ofs) {
		throw runtime_error("Could not write to file: " + path);
	}
}

experimental::filesystem::path utils::join(const string& file, const vector<string>& others) {
	experimental::filesystem::path re_path = file;
	for (const auto& part : others) {
		re_path /= part;
	}
	return re_path;
}