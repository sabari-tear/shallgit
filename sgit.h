#pragma once


#include <iostream>
#include <fstream> 
#include <filesystem>  
using namespace std::filesystem;
#include <string>
#include <unordered_set>
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
	commit deserializeCommit(const std::string& path);
	void global();
	void find(const std::string& msg);
	commit getCurrentCommit();
	void checkout(const std::vector<std::string>& args);

	string getHEAD();
	stagingarea getStage();
	void status();
	void branch(const std::string& branchName);
	void rmb(const std::string& branchName);
	void reset(const std::string& commitID);
	void merge(const std::string& bName);

	commit findSplitPoint(const commit& currentCommit, const commit& branchCommit);
	void checkoutFile(const commit& commit, const std::string& fileName);
	std::unordered_set<std::string> getAllAncestors(commit& comit);
	void serializeStage();
	void deserializeStage();
	void handleConflict(const std::string& fileName, const std::string& currentBlobHash, const std::string& branchBlobHash);

	path findfile(const std::string& fileName, const path& dir);
	vector<char> addStuff(const std::vector<char>& addThisStuff, const std::vector<char>& newStuffs);
};

