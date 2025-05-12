#include "stagingarea.h"

void stagingarea::add(const std::string& filename, const std::string& sha1) {
	addedFiles[filename] = sha1;
}