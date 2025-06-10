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

    head = readTextFromFile((workingDir / ".shallgit/branches.head.txt").string());
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
    serializeCommit();

    path masterBranchPath = branches / "master.txt";
    ofstream masterFile(masterBranchPath.string());
    if (!masterFile) {
        cerr << "Failed to create the master branch file." << endl;
        return;
    }

    masterFile << commitHash;
    masterFile.close();

    path headPath = branches / "head.txt";
    ofstream headFile(headPath.string());
    if (!headFile) {
        cerr << "Falided to set the head" << endl;
        return;
    }

    headFile << " master";
    headFile.close();

    stage = stagingarea();
    //searilaizeStage();

    cout << "Intialized an empty shallgit repository in " << absolute(repo) << endl;

    ofstream globalLogFile(globalLog / "gl.txt");
    globalLogFile.close();
}

void sgit::add(const string& fileName) {
    if (fileName == ".") {
        for (const auto& file : directory_iterator(workingDir)) {
            // check condition

            add(file.path().filename().string());
        }
    }

    //else condition
}

void sgit::commitment(const std::string& msg) {
    //stage needed
    if (stage.getAddedFiles().empty() && stage.getRemovedFiles().empty()) {
        std::cout << "No changes added to the commit." << std::endl;
        return;
    }
    else if (msg.empty()) {
        std::cout << "Please enter commit msg." << std::endl;
        return;
    }
    commit curr;// = getCurrentCommit();

    std::unordered_map<std::string, std::string> copiedBlobs = curr.getBlobs();

    for (const auto& file : stage.getAddedFiles()) {
        copiedBlobs[file.first] = file.second;
    }

    for (const auto& file : stage.getRemovedFiles()) {
        copiedBlobs.erase(file);
    }

    commit newCommit(msg, copiedBlobs, curr.getOwnHash());
    std::string commitPathString = (workingDir / ".shallgit/commits" / (newCommit.getOwnHash() + ".txt")).string();

    std::ofstream ofs(commitPathString);
    boost::archive::text_oarchive oa(ofs);
    oa << newCommit;
    ofs.close();

    std::string branchPathString = (workingDir / ".shallgit/branches" / (head + ".txt")).string();
    std::ofstream branchFile(branchPathString);

    branchFile << newCommit.getOwnHash();
    branchFile.close();

    stage.clear();
    //serializeStage();
}


void sgit::rm(const std::string& fileName) {
    bool isStaged = (stage.getAddedFiles().find(fileName)!=stage.getAddedFiles().end());

    commit curr;// = getCurrentCommit();
    bool isTracked = (curr.getBlobs().find(fileName) != curr.getBlobs().end());

    if (isTracked) {
        remove(workingDir / fileName);
        stage.addToRemovedFiles(fileName);
        if (isStaged) {
            stage.getAddedFiles().erase(fileName);
        }   
        //serializeStage();
    }
    else if (isStaged) {
        stage.getAddedFiles().erase(fileName);
        //serializeStage();
    }
    else {
        std::cout << "No reason to remove the file" << std::endl;
    }
}

void sgit::log() {
    commit curr; //getCurrentCommit();
    while (!curr.getOwnHash().empty()) {
        std::cout << curr.globalLog();
        if (curr.getParentHash().empty()) {
            break;
        }
        curr = deserializeCommit(workingDir / ".shallgit/commits" / (curr.getOwnHash() + ".txt"));
    }
}

void sgit::serializeCommit(const commit& commit, const std::string& path) {
    std::ofstream ofs(path);
    boost::archive::text_oarchive oa(ofs);
    oa << commit;
}

commit sgit::deserializeCommit(const std::string& path) {
    commit Commit;
    std::ifstream ifs(path);
    if (ifs.good()) {
        boost::archive::text_iarchive ia(ifs);
        ia >> Commit;
    }
    return Commit;
}

void sgit::global() {
    std::ifstream inFIle(workingDir / ".shallgit/global-log/gl.txt");
    std::string line;
    while (std::getline(inFIle, line)) {
        std::cout << line << std::endl;
    }
}

void sgit::find(const std::string& msg) {
    bool found = false;
    for (const auto& entry : directory_iterator(workingDir / ".gitlet/commits")) {
        commit currCommit = deserializeCommit(entry.path().string());
        if (currCommit.getMessage() == msg) {
            std::cout << currCommit.getOwnHash() << std::endl;
            found = true;
        }
    }

    if (!found) {
        std::cout << "Found no commit" << std::endl;
    }
}

commit sgit::getCurrentCommit() {
    std::string headBranchPath = (workingDir / ".shallgit/branches/head.txt").string();
    std::string currentBranch = utils::readTextFromFile(headBranchPath);
    //error
    std::string currentCommitHash = utils::readTextFromFile((workingDir / ".gitlet/branches" / (currentBranch + ".txt")).string());
    //error
    return deserializeCommit((workingDir / ".shallgit/commits" / (currentCommitHash + ".txt")).string());
}

void sgit::checkout(const std::vector<std::string>& args) {
    if (args.size() == 1) {
        std::string branchName = args[0];
        path  branchPath = workingDir / ".gitlet/branches" / (branchName + ".txt");
        if (!exists(branchPath)) {
            std::cout << "No such branch exists." << std::endl;
            return;
		}
		std::string commitID = utils::readTextFromFile(branchPath.string());

    }
}