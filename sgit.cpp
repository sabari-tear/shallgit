#include "sgit.h"  
#if __cplusplus >= 201703L  
namespace fs = std::filesystem;  
#else
using namespace std::experimental::filesystem;  
#endif  
using namespace utils;


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

}