#pragma once

#include <unordered_map>
#include <string>
#include <chrono>
using namespace std;
class commit
{
	string message;
	unordered_map<string, string> blobs;
	string parentHash;
	string datetime;
	string ownHash;

	commit();
	commit(string& msg, unordered_map<string, string>& blobMap, string& parent);
};

