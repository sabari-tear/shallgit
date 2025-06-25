#include "utils.h"
using namespace std;

string utils::readTextFromFile(const string& path) {
	ifstream file(path);
	return {istreambuf_iterator<char>(file), istreambuf_iterator<char>()};
}

string utils::sha1(const vector<char>& vals) {
	// to store the 20-byte (160-bit) SHA-1 hash output
	unsigned char hash[SHA_DIGEST_LENGTH];

	// Compute the SHA-1 hash of the entire input data (vals)
	SHA1(reinterpret_cast<const unsigned char*> (vals.data()), vals.size(), hash);

	// Create a stringstream to build the final hexadecimal hash string

	std::stringstream ss;
	// Convert each byte of the hash into a two-digit hexadecimal representation
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
		ss << std::hex               // Set stream to hexadecimal format
			<< std::setw(2)           // Each byte is represented with 2 digits
			<< std::setfill('0')      // Pad with 0 if less than 2 digits (e.g., '0a')
			<< static_cast<int>(hash[i]); // Convert unsigned char to int for output
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

std::filesystem::path utils::join(const string& file, const vector<string>& others) {
	std::filesystem::path re_path = file;
	for (const auto& part : others) {
		re_path /= part;
	}
	return re_path;
}

void utils::writeTextToFile(const std::string& file, const std::string& text, bool overwrite) {
	std::ofstream ofs;
	if (overwrite) {
		ofs.open(file, std::ofstream::out | std::ofstream::trunc);
	} else {
		ofs.open(file, std::ofstream::out | std::ofstream::app);
	}
	if (!ofs) {
		throw std::runtime_error("Could not open file for writing: " + file);
	}
	ofs << text;
	if (!ofs) {
		throw std::runtime_error("Could not write to file: " + file);
	}
	ofs.close();
}