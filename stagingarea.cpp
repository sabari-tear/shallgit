#include "stagingarea.h"

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