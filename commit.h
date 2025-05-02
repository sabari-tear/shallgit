#pragma once

#include <unordered_map>
using namespace std;
class commit
{
	commit();
	commit(string& msg, unordered_map<string, string>& blobMap, string& parent);
};

