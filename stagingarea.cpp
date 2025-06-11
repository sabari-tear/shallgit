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

void stagingarea::deserilaizeFromString(const std::string& str) {
	std::istringstream archive_stream(str);
	boost::archive::text_iarchive archive(archive_stream);
	archive >> *this;
}

template<class Archive>
void stagingarea::serialize(Archive& ar, const unsigned int version) {
	ar& addedFiles;
	ar& removedFiles;
}

template<class Archive>
void stagingarea::deserialize(Archive& ar, const unsigned int version) {
	ar& addedFiles;
	ar& removedFiles;
}