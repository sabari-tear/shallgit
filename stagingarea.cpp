#include "stagingarea.h"

stagingarea::stagingarea() {

}

void stagingarea::add(const std::string& filename, const std::string& sha1) {
	addedFiles[filename] = sha1;
}
void stagingarea::addToRemovedFiles(const std::string& filename) {
	removedFiles.push_back(filename);
}