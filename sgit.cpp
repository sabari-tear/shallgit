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

void sgit::handleConflict(const std::string& fileName, const std::string& currentBlobHash, const std::string& branchBlobHash) {
    std::string conflictMarkerhead = "<<<<<<< head\n";
    std::string conflictMarkerMid = "=======\n";
    std::string conflictMarkerEnd = ">>>>>>>\n";
    std::string currentContents = currentBlobHash.empty() ? std::string() :
        std::string(utils::readBinaryFromFile((workingDir / ".shallgit/blobs" / (currentBlobHash + ".txt")).string()).begin(),
            utils::readBinaryFromFile((workingDir / ".shallgit/blobs" / (currentBlobHash + ".txt")).string()).end());
    std::string branchContents = branchBlobHash.empty() ? std::string() :
        std::string(utils::readBinaryFromFile((workingDir / ".shallgit/blobs" / (branchBlobHash + ".txt")).string()).begin(),
            utils::readBinaryFromFile((workingDir / ".shallgit/blobs" / (branchBlobHash + ".txt")).string()).end());
    std::string conflictData = conflictMarkerhead + currentContents + conflictMarkerMid + branchContents + conflictMarkerEnd;
    std::vector<char> conflictDataVec(conflictData.begin(), conflictData.end());
    utils::writeBinaryToFile((workingDir / fileName).string(), conflictDataVec);
    stage.add(fileName, utils::sha1(conflictDataVec));
}

void sgit::checkoutFile(commit& comit, const std::string& fileName) {
    std::string blobHash = comit.getBlobs().at(fileName);
    std::vector<char> blobData = utils::readBinaryFromFile((workingDir / ".shallgit/blobs" / (blobHash + ".txt")).string());
    utils::writeBinaryToFile((workingDir / fileName).string(), blobData);
}

commit sgit::findSplitPoint(commit& currentCommit, commit& branchCommit) {
    std::unordered_set<std::string> currentAncestors = getAllAncestors(currentCommit);
    std::unordered_set<std::string> branchAncestors = getAllAncestors(branchCommit);

    for (const auto& ancestorHash : currentAncestors) {
        if (branchAncestors.find(ancestorHash) != branchAncestors.end()) {
            return deserializeCommit((workingDir / ".shallgit/commits" / (ancestorHash + ".txt")).string());
        }
    }
    return commit(); // Return an empty commit if no common ancestor is found
}

#include <queue>
std::unordered_set<std::string> sgit::getAllAncestors(commit& comit) {
    std::unordered_set<std::string> ancestors;
    std::queue<std::string> toVisit;
    toVisit.push(comit.getOwnHash());

    while (!toVisit.empty()) {
        std::string currentHash = toVisit.front();
        toVisit.pop();
        ancestors.insert(currentHash);

        commit currentCommit = deserializeCommit((workingDir / ".shallgit/commits" / (currentHash + ".txt")).string());
        if (!currentCommit.getParentHash().empty()) {
            toVisit.push(currentCommit.getParentHash());
        }
    }

    return ancestors;
}


void sgit::serializeStage() {
    std::ofstream ofs(".shallgit/staging/stage.txt");
    boost::archive::text_oarchive oa(ofs);
    oa << stage; // Assuming 'stage' is an instance of StagingArea
}

void sgit::deserializeStage() {
    std::ifstream ifs(".shallgit/staging/stage.txt");
    if (ifs.good()) { // Check if the file exists and is not empty
        boost::archive::text_iarchive ia(ifs);
        ia >> stage; // Assuming 'stage' is an instance of StagingArea
    }
}

void sgit::add(const std::string& fileName) {
    if (fileName == ".") {
        for (const auto& file : directory_iterator(workingDir)) {
            if (is_regular_file(file) && file.path().extension() != ".shallgit" && file.path().filename() != "Shallgit") {
                //slicing only the filename from the path
                add(file.path().filename().string());
            }
        }
    }
    else {
        if (!exists(workingDir / fileName)) {
            std::cout << "File does not exist." << std::endl;
            return;
        }

        std::vector<char> fileContents = utils::readBinaryFromFile((workingDir / fileName).string());
        std::string sha1 = utils::sha1(fileContents);
        std::string blobPath = (workingDir / ".shallgit/blobs" / (sha1 + ".txt")).string();
        if (!exists(blobPath)) {
            utils::writeBinaryToFile(blobPath, fileContents);
        }

        stage.add(fileName, sha1);
        serializeStage();
    }
}

void sgit::status() {
    // List branches
    std::vector<std::string> branches;
    for (const auto& entry : directory_iterator(workingDir / ".shallgit/branches")) {
        std::string branchName = entry.path().filename().string();
        if (branchName != "head.txt") {
            branches.push_back(branchName == head ? "*" + branchName : branchName);
        }
    }
    std::sort(branches.begin(), branches.end());

    // List staged files
    std::vector<std::string> stagedFiles;
    for (const auto& [fileName, _] : stage.getAddedFiles()) {
        stagedFiles.push_back(fileName);
    }
    std::sort(stagedFiles.begin(), stagedFiles.end());

    // List removed files
    std::vector<std::string> removedFiles(stage.getRemovedFiles().begin(), stage.getRemovedFiles().end());
    std::sort(removedFiles.begin(), removedFiles.end());

    // Display status
    std::cout << "=== Branches ===\n";
    for (const auto& branchName : branches) {
        //remove txt extension from branch name
        //read current branch name from head.txt

        std::string branch = branchName.substr(0, branchName.size() - 4);
        std::string headBranchPath = (workingDir / ".shallgit/branches/head.txt").string();
        std::string currentBranch = utils::readTextFromFile(headBranchPath);

        if (branch == currentBranch) {
            std::cout << "*";
        }

        std::cout << branch << "\n";
    }
    std::cout << "\n=== Staged Files ===\n";
    for (const auto& file : stagedFiles) {
        std::cout << file << "\n";
    }
    std::cout << "\n=== Removed Files ===\n";
    for (const auto& file : removedFiles) {
        std::cout << file << "\n";
    }

    std::cout << "\n=== Modifications Not Staged For Commit ===\n\n=== Untracked Files ===\n";
}


void sgit::merge(const std::string& branchName) {
    std::string currentBranch = utils::readTextFromFile((workingDir / ".shallgit/branches/head.txt").string());
    if (currentBranch == branchName) {
        std::cout << "Cannot merge a branch with itself." << std::endl;
        return;
    }

    std::string currentCommitHash = utils::readTextFromFile((workingDir / ".shallgit/branches" / (currentBranch + ".txt")).string());
    std::string branchCommitHash = utils::readTextFromFile((workingDir / ".shallgit/branches" / (branchName + ".txt")).string());
    commit currentCommit = deserializeCommit((workingDir / ".shallgit/commits" / (currentCommitHash + ".txt")).string());
    commit branchCommit = deserializeCommit((workingDir / ".shallgit/commits" / (branchCommitHash + ".txt")).string());

    commit splitPoint = findSplitPoint(currentCommit, branchCommit);
    if (splitPoint.getOwnHash().empty()) {
        std::cout << "Already up-to-date." << std::endl;
        return;
    }
    else if (splitPoint.getOwnHash() == branchCommit.getOwnHash()) {
        utils::writeTextToFile(branchName, (workingDir / ".shallgit/branches/head.txt").string(), true);
        std::cout << "Current branch fast-forwarded." << std::endl;
        return;
    }

    std::unordered_set<std::string> currentAncestors = getAllAncestors(currentCommit);
    std::unordered_set<std::string> branchAncestors = getAllAncestors(branchCommit);
    std::unordered_set<std::string> splitPointAncestors = getAllAncestors(splitPoint);

    // Check for uncommitted changes
    if (!stage.getAddedFiles().empty() || !stage.getRemovedFiles().empty()) {
        std::cout << "You have uncommitted changes." << std::endl;
        return;
    }

    // Check for untracked files
    for (const auto& file : directory_iterator(workingDir)) {
        if (is_regular_file(file) && file.path().filename() != ".shallgit" && file.path().filename() != "Shallgit") {
            std::string relativePath = relative(file.path(), workingDir).string();
            if (currentCommit.getBlobs().find(relativePath) == currentCommit.getBlobs().end() && branchCommit.getBlobs().find(relativePath) == branchCommit.getBlobs().end()) {
                std::cout << "There is an untracked file in the way; delete it, or add and commit it first." << std::endl;
                return;
            }
        }
    }

    // Check for conflicts
    for (const auto& [fileName, currentBlobHash] : currentCommit.getBlobs()) {
        std::string branchBlobHash = branchCommit.getBlobs().find(fileName) != branchCommit.getBlobs().end() ? branchCommit.getBlobs().at(fileName) : "";
        std::string splitPointBlobHash = splitPoint.getBlobs().find(fileName) != splitPoint.getBlobs().end() ? splitPoint.getBlobs().at(fileName) : "";
        if (currentBlobHash != branchBlobHash && currentBlobHash != splitPointBlobHash && branchBlobHash != splitPointBlobHash) {
            std::cout << "Encountered a merge conflict." << std::endl;
            handleConflict(fileName, currentBlobHash, branchBlobHash);
            return;
        }
    }

    // Update the current branch's commit ID
    std::string headBranchPath = (workingDir / ".shallgit/branches/head.txt").string();
    utils::writeTextToFile(currentBranch, headBranchPath, true);
    std::cout << "Merged " << branchName << " into " << currentBranch << "." << std::endl;
}




