#include "sgit.h"  
#if __cplusplus >= 201703L  
namespace fs = std::filesystem;  
#else
using namespace std::experimental::filesystem;  
#endif  
using namespace utils;
using namespace std;

sgit::sgit() {  
    workingDir = current_path();
    //desearilize functionality

    head = readTextFromFile((workingDir/".shallgit/branches.head.txt").string())
}  

void sgit::init() {  
    path repo = workingDir / ".shallgit";
    path blobs = repo / "blobs";
    path commits = repo / "commits";
    path branches = repo / "branches";
    path staging = repo / "staging";
    path globalLog = repo / "gloabal-log";

    create_directories(blobs);
    create_directories(commits);
    create_directories(branches);
    create_directories(staging);
    create_directories(globalLog);

    string s = "initial commit";
    commit initialCommit(s , {}, string(""));
    string commitHash = initialCommit.getOwnHash();

}