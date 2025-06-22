# shallgit

A simple version control system inspired by Git, written in C++. It provides basic version control features such as initializing a repository, adding files, committing changes, branching, merging, and more—all from the command line.

---

## Features
- Initialize a new repository
- Add and remove files
- Commit changes
- View commit logs and global logs
- Branching and merging
- File and branch checkout
- Status reporting

---

## Dependencies
- **C++17** or later
- [Boost](https://www.boost.org/) (for serialization)
- [OpenSSL](https://www.openssl.org/) (for SHA-1 hashing)

> **Note:** This project is designed for Windows and uses Visual Studio project files.

---

## Building the Project

1. **Install Dependencies:**
   - Install [Boost](https://www.boost.org/) and [OpenSSL](https://www.openssl.org/) libraries on your system.
   - Make sure they are discoverable by Visual Studio (set up include/lib paths if needed).

2. **Open the Solution:**
   - Open `shallgit.sln` in Visual Studio 2019 or later.

3. **Build:**
   - Select the desired configuration (Debug/Release, x64/x86).
   - Build the solution (Ctrl+Shift+B).

4. **Run:**
   - The executable will be located in `x64/Release/` or `x64/Debug/` depending on your build configuration.

---

## Usage

All commands are run from the command line in your project directory:

```sh
shallgit <command> [arguments]
```

### Typical Workflow

```sh
# Initialize a new repository
shallgit init

# Add a file to the staging area
shallgit add myfile.txt

# Commit staged changes
shallgit commit "Initial commit"

# View commit log
shallgit log

# Create a new branch
shallgit branch feature-x

# Switch to a branch
shallgit checkout feature-x

# Merge a branch into the current branch
shallgit merge feature-x

# Remove a file
shallgit rm myfile.txt

# View status
shallgit status
```

---

## Command Reference

| Command                | Description                                      |
|------------------------|--------------------------------------------------|
| `init`                 | Initialize a new shallgit repository             |
| `add <file>`           | Add a file to the staging area                   |
| `commit <msg>`         | Commit staged changes with a message             |
| `rm <file>`            | Remove a file from the working directory         |
| `log`                  | Show commit history for the current branch       |
| `global-log`           | Show all commits in the repository               |
| `find <msg>`           | Find commit(s) by commit message                 |
| `status`               | Show the status of the working directory         |
| `checkout <branch>`    | Switch to another branch                         |
| `checkout <commit> -- <file>` | Restore a file from a specific commit      |
| `branch <name>`        | Create a new branch                              |
| `rm-branch <name>`     | Remove a branch                                  |
| `reset <commit>`       | Reset current branch to a specific commit        |
| `merge <branch>`       | Merge a branch into the current branch           |

---

## Notes
- All repository data is stored in a hidden `.shallgit` directory.
- Only basic error handling is implemented; use with caution for important data.
- This project is for educational purposes and is not a replacement for Git.

---

## License
MIT License (or specify your own) 