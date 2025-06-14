#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include "sgit.h"  
#include <filesystem>  
using namespace utils;
using namespace std;
using namespace std::filesystem;

sgit::sgit() {  
    workingDir = current_path();
    deserializeStage();
    head = readTextFromFile((workingDir / ".shallgit/branches/head.txt").string());
}  

void sgit::init() {  
    path repo = workingDir / ".shallgit";
    path blobs = repo / "blobs";
    path commits = repo / "commits";
    path branches = repo / "branches";
    path staging = repo / "staging";
    path globalLog = repo / "global-log";

    if (exists(repo)) {
        cerr << "A shallgit repository already exists in " << absolute(repo) << endl;
        return;
	}

    create_directories(blobs);
    create_directories(commits);
    create_directories(branches);
    create_directories(staging);
    create_directories(globalLog);

    string s = "initial commit";
    commit initialCommit(s , {}, string(""));
    string commitHash = initialCommit.getOwnHash();
    serializeCommit(initialCommit, (commits/(commitHash + ".txt")).string());

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
    serializeStage();

    cout << "Intialized an empty shallgit repository in " << absolute(repo) << endl;

    ofstream globalLogFile(globalLog / "gl.txt");
    globalLogFile.close();
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
    commit curr = getCurrentCommit();

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
    serializeStage();
}

void sgit::rm(const std::string& fileName) {
    bool isStaged = (stage.getAddedFiles().find(fileName)!=stage.getAddedFiles().end());

    commit curr = getCurrentCommit();
    bool isTracked = (curr.getBlobs().find(fileName) != curr.getBlobs().end());

    if (isTracked) {
        remove(workingDir / fileName);
        stage.addToRemovedFiles(fileName);
        if (isStaged) {
            stage.getAddedFiles().erase(fileName);
        }   
        serializeStage();
    }
    else if (isStaged) {
        stage.getAddedFiles().erase(fileName);
        serializeStage();
    }
    else {
        std::cout << "No reason to remove the file" << std::endl;
    }
}

void sgit::log() {
    commit curr = getCurrentCommit();
    while (!curr.getOwnHash().empty()) {
        std::cout << curr.globalLog();
        if (curr.getParentHash().empty()) {
            break;
        }
        curr = deserializeCommit((const string)(workingDir / ".shallgit/commits" / (curr.getOwnHash() + ".txt")).string());
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
    for (const auto& entry : directory_iterator(workingDir / ".shallgit/commits")) {
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
    std::string currentCommitHash = utils::readTextFromFile((workingDir / ".shallgit/branches" / (currentBranch + ".txt")).string());
    return deserializeCommit((workingDir / ".shallgit/commits" / (currentCommitHash + ".txt")).string());
}

void sgit::checkout(const std::vector<std::string>& args) {
    if (args.size() == 1) {
        std::string branchName = args[0];
        path  branchPath = workingDir / ".shallgit/branches" / (branchName + ".txt");
        if (!exists(branchPath)) {
            std::cout << "No such branch exists." << std::endl;
            return;
        }
        std::string commitID = utils::readTextFromFile(branchPath.string());
        commit comit = deserializeCommit((workingDir / ".shallgit/commits" / (commitID + ".txt")).string());
        for (const auto& fileName : comit.getBlobs()) {
            checkoutFile(comit, fileName.first);
        }

        // remove files that are not in the commit to rest to
        for (const auto& file : directory_iterator(workingDir)) {
            if (is_regular_file(file) && file.path().filename() != ".shallgit" && file.path().filename() != "Shallgit") {
                std::string relativePath = relative(file.path(), workingDir).string();
                if (comit.getBlobs().find(relativePath) == comit.getBlobs().end()) {
                    remove(file);
                }
            }
        }

        //overwrite
        std::string headBranchPath = (workingDir / ".shallgit/branches/head.txt").string();
        utils::writeTextToFile(branchName, head, true);
    }
    else if (args.size() == 3 && args[1] == "--") {
        std::string commitID = args[0];
        string fileName = args[2];
        commit comit = deserializeCommit((workingDir / ".shallgit/commits" / (commitID + ".txt")).string());
        checkoutFile(comit, fileName);
    }
    else {
        cout << "incorrect operands" << '\n';
    }
}

std::string sgit::gethead() {
    return head;
}

stagingarea sgit::getStage() {
    return stage;
}

//void sgit::add(const string& fileName) {
//    if (fileName == ".") {
//        for (const auto& file : directory_iterator(workingDir)) {
//            // check condition
//
//            add(file.path().filename().string());
//        }
//    }
//
//    //else condition
//}


void sgit::branch(const std::string& branchName) {
    path branchPath = workingDir / ".shallgit/branches" / (branchName + ".txt");
    if (exists(branchPath)) {
        cout << "a branch with that name is already exists" << '\n';
        return;
    }
    string sha1 = utils::readTextFromFile((workingDir / ".shallgit/branches" / head).string() + ".txt");
    utils::writeTextToFile(sha1, branchPath.string(), false);
}

void sgit::rmb(const std::string& branchName) {
    if (branchName == utils::readTextFromFile((workingDir / ".shallgit/branches/head.txt").string())) {
        std::cout << "Cannot remove the current branch." << "\n";
        return;
    }

    path branchPath = workingDir / ".shallgit/branches" / (branchName + ".txt");
    if (remove(branchPath)) {
        cout << "branch:" << branchName << " removed" << '\n';
    }
    else {
        cout << "a branch with that name does not exist.\n";
    }
}

void sgit::reset(const std::string& commitID) {
    commit commitToreset = deserializeCommit(".shallgit/commit/" + commitID + ".txt");
    if (commitToreset.getOwnHash().empty()) {
        cout << "NO commit with that id." << '\n';
        return;
    }

    for (const auto& [fileName, bloblHash] : commitToreset.getBlobs()) {
        checkoutFile(commitToreset, fileName);
    }

    for (const auto& file : directory_iterator(workingDir)) {
        if (is_regular_file(file) && file.path().filename() != ".shallgit" && file.path().filename() != "Shallgit") {
            std::string relativePath = relative(file.path(), workingDir).string();
            if (commitToreset.getBlobs().find(relativePath) == commitToreset.getBlobs().end()) {
                remove(file);
            }
        }
    }

    string headBranchPath = (workingDir / ".shallgit/branches/head.txt").string();
    string currentBranch = utils::readTextFromFile(headBranchPath);
    string branchPath = (workingDir / ".shallgit/branches" / currentBranch).string();
    utils::writeTextToFile(commitID, branchPath, true);

    cout << "reset to commit " << commitID << '\n';
}

//--------------------------------

