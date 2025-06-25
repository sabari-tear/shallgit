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
    //initializing paths
    path repo = workingDir / ".shallgit";
    path blobs = repo / "blobs";
    path commits = repo / "commits";
    path branches = repo / "branches";
    path staging = repo / "staging";
    path globalLog = repo / "global-log";

    //check whether the path exists
    if (exists(repo)) {
        cerr << "A shallgit repository already exists in " << absolute(repo) << endl;
        return;
	}

    //creating the folders
    create_directories(blobs);
    create_directories(commits);
    create_directories(branches);
    create_directories(staging);
    create_directories(globalLog);

    //convert the commit object to hash to binary text and save it in the commit path
    string s = "initial commit";
    commit initialCommit(s , {}, string(""));
    string commitHash = initialCommit.getOwnHash();
    serializeCommit(initialCommit, (commits/(commitHash + ".txt")).string());

    //creating the master branch
    path masterBranchPath = branches / "master.txt";
    ofstream masterFile(masterBranchPath.string());
    if (!masterFile) {
        cerr << "Failed to create the master branch file." << endl;
        return;
    }

    //setting the master branch top as commitHash
    masterFile << commitHash;
    masterFile.close();

    //setting the head branch as master
    path headPath = branches / "head.txt";
    ofstream headFile(headPath.string());
    if (!headFile) {
        cerr << "Falided to set the head" << endl;
        return;
    }

    headFile << " master";
    headFile.close();

    // initializing the stagingarea object
    stage = stagingarea();
    // initing the empty stage to disk
    serializeStage();

    cout << "Intialized an empty shallgit repository in " << absolute(repo) << endl;

    //initing the global log
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

    //getting the last commit details
    commit curr = getCurrentCommit();

    //get the last commit to inlucde all the files in the commit rather than only the files in the staged 
    //the commit supposed to contains whole verison
    std::unordered_map<std::string, std::string> copiedBlobs = curr.getBlobs();

    // newly added filename , hashes
    for (const auto& file : stage.getAddedFiles()) {
        copiedBlobs[file.first] = file.second;
    }

    //remove the rm command files
    for (const auto& file : stage.getRemovedFiles()) {
        copiedBlobs.erase(file);
    }

    //init a newCommit
    commit newCommit(msg, copiedBlobs, curr.getOwnHash());
    std::string commitPathString = (workingDir / ".shallgit/commits" / (newCommit.getOwnHash() + ".txt")).string();

    //serialize the commit object to the file
    try {
        std::ofstream ofs(commitPathString);
        if (!ofs) throw std::runtime_error("Could not open commit file for writing");
        boost::archive::text_oarchive oa(ofs);
        oa << newCommit;
        ofs.close();
    } catch (const std::exception& e) {
        std::cerr << "Error serializing commit: " << e.what() << std::endl;
        return;
    }
    
    //global log update
    std::ofstream globalLogFile(workingDir / ".shallgit/global-log/gl.txt", std::ios::app);
    globalLogFile << newCommit.globalLog();
    globalLogFile.close();

    //write the commit has the branch file
    std::string branchPathString = (workingDir / ".shallgit/branches" / (head + ".txt")).string();
    std::ofstream branchFile(branchPathString);

    branchFile << newCommit.getOwnHash();
    branchFile.close();

    //empty the stage
    stage.clear();
    serializeStage();
}

void sgit::rm(const std::string& fileName) {
    // check whether it is added first
    bool isStaged = (stage.getAddedFiles().find(fileName)!=stage.getAddedFiles().end());

    // get the last commit
    commit curr = getCurrentCommit();
    // check whether file exists in the blob
    bool isTracked = (curr.getBlobs().find(fileName) != curr.getBlobs().end());

    if (isTracked) {
        // if blob is already there then delete the file
        try {
            remove(workingDir / fileName);
        } catch (const std::exception& e) {
            std::cerr << "Error removing file: " << e.what() << std::endl;
        }
        stage.addToRemovedFiles(fileName);
        if (isStaged) {
            stage.removeFromAddedFiles(fileName);
        }
        serializeStage();
    }
    else if (isStaged) {
        //just remove the file from staged
        stage.removeFromAddedFiles(fileName);
        serializeStage();
    }
    else {
        std::cout << "No reason to remove the file" << std::endl;
    }
}

void sgit::log() {
    //get the last commit
    commit curr = getCurrentCommit();
    while (!curr.getOwnHash().empty()) {
        //details of the commit
        std::cout << curr.globalLog();
        if (curr.getParentHash().empty()) {
            break;
        }
        //go the the parent and deserialize it
        curr = deserializeCommit((const string)(workingDir / ".shallgit/commits" / (curr.getParentHash() + ".txt")).string());
    }
}

void sgit::serializeCommit(const commit& commit, const std::string& path) {
    std::ofstream ofs(path);
    boost::archive::text_oarchive oa(ofs);
    oa << commit;
}

commit sgit::deserializeCommit(const std::string& path) {
    commit Commit;
    //setting input stream for load to commit var
    std::ifstream ifs(path);
    if (ifs.good()) {
        try {
            boost::archive::text_iarchive ia(ifs);
            ia >> Commit;
        } catch (const std::exception& e) {
            std::cerr << "Error deserializing commit: " << e.what() << std::endl;
        }
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

//returns the commit hash of the given commit message
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
        //branch switching operation
        std::string branchName = args[0];
        path  branchPath = workingDir / ".shallgit/branches" / (branchName + ".txt");
        if (!exists(branchPath)) {
            std::cout << "No such branch exists." << std::endl;
            return;
        }
        //get the branch top commit and restore its files
        std::string commitID = utils::readTextFromFile(branchPath.string());
        commit comit = deserializeCommit((workingDir / ".shallgit/commits" / (commitID + ".txt")).string());
        for (const auto& fileName : comit.getBlobs()) {
            //restore file from a commit
            checkoutFile(comit, fileName.first);
        }

        // remove files that are not in the commit to rest to
        for (const auto& file : directory_iterator(workingDir)) {
            if (is_regular_file(file)) {
                std::string relativePath = relative(file.path(), workingDir).string();
                if (comit.getBlobs().find(relativePath) == comit.getBlobs().end()) {
                    try {
                        remove(file);
                    } catch (const std::exception& e) {
                        std::cerr << "Error removing file during checkout: " << e.what() << std::endl;
                    }
                }
            }
        }

        //overwrite the head.txt to current branch
        std::string headBranchPath = (workingDir / ".shallgit/branches/head.txt").string();
        utils::writeTextToFile(headBranchPath, branchName, true);
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
    //creates a new branch and mark the commit to the txt file
    path branchPath = workingDir / ".shallgit/branches" / (branchName + ".txt");
    if (exists(branchPath)) {
        cout << "a branch with that name is already exists" << '\n';
        return;
    }
    string commitHash = utils::readTextFromFile((workingDir / ".shallgit/branches" / head).string() + ".txt");
    utils::writeTextToFile(commitHash, branchPath.string(), false);
}

void sgit::rmb(const std::string& branchName) {
    // check whether it the current branch
    if (branchName == utils::readTextFromFile((workingDir / ".shallgit/branches/head.txt").string())) {
        std::cout << "Cannot remove the current branch." << "\n";
        return;
    }
    // if not delete thr txt file alone
    path branchPath = workingDir / ".shallgit/branches" / (branchName + ".txt");
    if (remove(branchPath)) {
        cout << "branch:" << branchName << " removed" << '\n';
    }
    else {
        cout << "a branch with that name does not exist.\n";
    }
}

void sgit::reset(const std::string& commitID) {
    // get the commit object
    commit commitToreset = deserializeCommit(".shallgit/commits/" + commitID + ".txt");
    if (commitToreset.getOwnHash().empty()) {
        cout << "NO commit with that id." << '\n';
        return;
    }

    // checkout the file from the blob based on the commit
    for (const auto& [fileName, bloblHash] : commitToreset.getBlobs()) {
        checkoutFile(commitToreset, fileName);
    }

    //remove the files not needed in the commit
    for (const auto& file : directory_iterator(workingDir)) {
        if (is_regular_file(file) && file.path().filename() != ".shallgit" && file.path().filename() != "Shallgit") {
            std::string relativePath = relative(file.path(), workingDir).string();
            if (commitToreset.getBlobs().find(relativePath) == commitToreset.getBlobs().end()) {
                remove(file);
            }
        }
    }

    //set the commit to the branch
    string headBranchPath = (workingDir / ".shallgit/branches/head.txt").string();
    string currentBranch = utils::readTextFromFile(headBranchPath);
    string branchPath = (workingDir / ".shallgit/branches" / currentBranch).string();
    utils::writeTextToFile(commitID, branchPath, true);

    cout << "Reset to commit: " << commitID << '\n';
}

void sgit::handleConflict(const std::string& fileName, const std::string& currentBlobHash, const std::string& branchBlobHash) {
    std::string conflictMarkerhead = "<<<<<<< head\n";
    std::string conflictMarkerMid = "=======\n";
    std::string conflictMarkerEnd = ">>>>>>>\n";
    std::string currentContents;
    if (!currentBlobHash.empty()) {
        std::vector<char> currVec = utils::readBinaryFromFile((workingDir / ".shallgit/blobs" / (currentBlobHash + ".txt")).string());
        currentContents = std::string(currVec.begin(), currVec.end());
    }
    else {
        currentContents = "";
    }

    std::string branchContents;
    if (!branchBlobHash.empty()) {
        std::vector<char> branchVec = utils::readBinaryFromFile((workingDir / ".shallgit/blobs" / (branchBlobHash + ".txt")).string());
        branchContents = std::string(branchVec.begin(), branchVec.end());
    }
    else {
        branchContents = "";
    }
    
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
    // root to current commit hashes
    std::unordered_map<std::string, int> currentAncestors = getAllAncestors(currentCommit);
    std::unordered_map<std::string, int> branchAncestors = getAllAncestors(branchCommit);

    int minDistance = INT_MAX;
    std::string splitPointHash;

    for (const auto& [hash, dist1] : currentAncestors) {
        if (branchAncestors.find(hash) != branchAncestors.end()) {
            int totalDist = dist1 + branchAncestors[hash];
            if (totalDist < minDistance) {
                minDistance = totalDist;
                splitPointHash = hash;
            }
        }
    }

    if (!splitPointHash.empty()) {
        return deserializeCommit((workingDir / ".shallgit/commits" / (splitPointHash + ".txt")).string());
    }
    return commit(); // Return empty commit if no common ancestorr is found
}

#include <queue>
#include <unordered_map>
std::unordered_map<std::string, int> sgit::getAllAncestors(commit& comit) {
    std::unordered_map<std::string, int> ancestors;
    std::queue<std::pair<std::string, int>> toVisit;
    toVisit.push({comit.getOwnHash(), 0 });

    while (!toVisit.empty()) {
        auto [currentHash, dist] = toVisit.front();
        toVisit.pop();

        if (ancestors.find(currentHash) != ancestors.end()) continue;
        ancestors[currentHash] = dist;

        commit currentCommit = deserializeCommit((workingDir / ".shallgit/commits" / (currentHash + ".txt")).string());
        if (!currentCommit.getParentHash().empty()) {
            toVisit.push({ currentCommit.getParentHash(), dist + 1 });
        }
    }
    return ancestors;
}


void sgit::serializeStage() {
    //output stream path where it saves
    try {
        std::ofstream ofs(".shallgit/staging/stage.txt");
        if (!ofs) throw std::runtime_error("Could not open stage file for writing");
        boost::archive::text_oarchive oa(ofs);
        // push the stage to archive it desiralize it and save it in disk
        oa << stage; 
    } catch (const std::exception& e) {
        std::cerr << "Error serializing stage: " << e.what() << std::endl;
    }
}

void sgit::deserializeStage() {
    //getting the input stream file path (stage object exits)
    std::ifstream ifs(".shallgit/staging/stage.txt");
    //checking if theres is any file in the staging area
    if (ifs.good()) {
        try {
            boost::archive::text_iarchive ia(ifs);
            ia >> stage;
        } catch (const std::exception& e) {
            std::cerr << "Error deserializing stage: " << e.what() << std::endl;
        }
    }
}

void sgit::add(const std::string& fileName) {
    if (fileName == ".") {
        for (const auto& file : directory_iterator(workingDir)) {
            if (is_regular_file(file)) {
                //recursively call the same function as the individual name of the file
                add(file.path().filename().string());
            }
        }
    }
    else {
        if (!exists(workingDir / fileName)) {
            std::cout << "File does not exist." << std::endl;
            return;
        }

        //convert the file into binary
        try {
            std::vector<char> fileContents = utils::readBinaryFromFile((workingDir / fileName).string());
            //getting hash of the binary to set name for the file
            std::string sha1 = utils::sha1(fileContents);
            //creating txt file named the hash of the binary
            std::string blobPath = (workingDir / ".shallgit/blobs" / (sha1 + ".txt")).string();
            if (!exists(blobPath)) {
                //write the binary to hash named file
                utils::writeBinaryToFile(blobPath, fileContents);
            }

            //add the file name to the stage
            stage.add(fileName, sha1);
            //serialize the stage to disk
            serializeStage();
        } catch (const std::exception& e) {
            std::cerr << "Error adding file: " << e.what() << std::endl;
        }
    }
}

void sgit::status() {
    // list all the branches
    std::vector<std::string> branches;
    for (const auto& entry : directory_iterator(workingDir / ".shallgit/branches")) {
        std::string branchName = entry.path().filename().string();
        if (branchName != "head.txt") {
            branches.push_back(branchName);
        }
    }
    std::sort(branches.begin(), branches.end());

    // list staged files
    std::vector<std::string> stagedFiles;
    for (const auto& [fileName, _] : stage.getAddedFiles()) {
        stagedFiles.push_back(fileName);
    }
    std::sort(stagedFiles.begin(), stagedFiles.end());

    // list removed files
    std::vector<std::string> removedFiles(stage.getRemovedFiles().begin(), stage.getRemovedFiles().end());
    std::sort(removedFiles.begin(), removedFiles.end());

    // Display status
    std::cout << "=== Branches ===\n";
    for (const auto& branchName : branches) {
        //remove txt extension from branch name
        //read current branch name from head.txt

        std::string branch = branchName.substr(0, branchName.size() - 4);
        if (branch == head) {
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
 }


void sgit::merge(const std::string& branchName) {
    // get the branch top commit
    std::string currentBranch = utils::readTextFromFile((workingDir / ".shallgit/branches/head.txt").string());
    if (currentBranch == branchName) {
        std::cout << "Cannot merge a branch with itself." << std::endl;
        return;
    }

    //get the both commit object from file
    std::string currentCommitHash = utils::readTextFromFile((workingDir / ".shallgit/branches" / (currentBranch + ".txt")).string());
    std::string branchCommitHash = utils::readTextFromFile((workingDir / ".shallgit/branches" / (branchName + ".txt")).string());
    commit currentCommit = deserializeCommit((workingDir / ".shallgit/commits" / (currentCommitHash + ".txt")).string());
    commit branchCommit = deserializeCommit((workingDir / ".shallgit/commits" / (branchCommitHash + ".txt")).string());

    // find the splitpoint
    commit splitPoint = findSplitPoint(currentCommit, branchCommit);
    if (splitPoint.getOwnHash().empty()) {
        //if there is no split point
        std::cout << "Already up-to-date." << std::endl;
        return;
    }
    else if (splitPoint.getOwnHash() == branchCommit.getOwnHash()) {
        //if it is an empty branch
        utils::writeTextToFile(branchName, (workingDir / ".shallgit/branches/head.txt").string(), true);
        std::cout << "Current branch fast-forwarded." << std::endl;
        return;
    }

    // Check for uncommitted changes
    if (!stage.getAddedFiles().empty() || !stage.getRemovedFiles().empty()) {
        std::cout << "You have uncommitted changes." << std::endl;
        return;
    }

    // Check for untracked files i.e not in blobs
    for (const auto& file : directory_iterator(workingDir)) {
        if (is_regular_file(file) ) {
            std::string relativePath = relative(file.path(), workingDir).string();
            if (currentCommit.getBlobs().find(relativePath) == currentCommit.getBlobs().end() && branchCommit.getBlobs().find(relativePath) == branchCommit.getBlobs().end()) {
                std::cout << "There is an untracked file in the way; delete it, or add and commit it first." << std::endl;
                return;
            }
        }
    }

    // Check for conflicts
    // can be merge becuase both has different version of the file from the splitpoint
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




