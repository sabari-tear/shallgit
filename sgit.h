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

class sgit
{
public:
	path workingDir;
	string head;

	sgit();

	void init();
	void add(const string& fileName);
};

