#pragma once


#include <iostream>
#include <fstream>
#if __cplusplus >= 201703L  
#include <filesystem>  
namespace fs = std::filesystem;
#else  
#include <experimental/filesystem>  
using namespace std::experimental::filesystem;
#endif  
#include <string>;

#include "utils.h"
#include "commit.h"
#include "stagingarea.h"

class sgit
{
public:
	path workingDir;
	string head;
	stagingarea stage;
	sgit();

	void init();
	void add(const string& fileName);
	void commitment(const string& msg);
	void rm(const std::string& fileName);
	void log();


	void serializeCommit(const commit& commit, const std::string& path);
		
};

