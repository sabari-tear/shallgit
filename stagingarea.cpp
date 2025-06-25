#include "stagingarea.h"
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
stagingarea::stagingarea() {

}

void stagingarea::add(const std::string& filename, const std::string& sha1) {
	addedFiles[filename] = sha1;
}
void stagingarea::addToRemovedFiles(const std::string& filename) {
	removedFiles.push_back(filename);
}

void stagingarea::clear() {
	addedFiles.clear();
	removedFiles.clear();
}

std::unordered_map<std::string,std::string> stagingarea::getAddedFiles() {
	return addedFiles;
}

std::vector<std::string> stagingarea::getRemovedFiles() {
	return removedFiles;
}

std::string stagingarea::serializeToString() {
	std::ostringstream archive_stream;
	boost::archive::text_oarchive archive(archive_stream);
	archive << *this;
	return archive_stream.str();
}

void stagingarea::deserializeFromString(const std::string& str) {
	std::istringstream archive_stream(str);
	boost::archive::text_iarchive archive(archive_stream);
	archive >> *this;
}

void stagingarea::removeFromAddedFiles(const std::string& filename) {
	addedFiles.erase(filename);
}

void stagingarea::removeFromRemovedFiles(const std::string& filename) {
	auto it = std::find(removedFiles.begin(), removedFiles.end(), filename);
	if (it != removedFiles.end()) {
		removedFiles.erase(it);
	}
}