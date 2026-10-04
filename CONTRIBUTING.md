# Contributing

Group members can work in parallel by creating a branch for each change and opening a pull request to `main`. Give the branch a short descriptive name, such as `feature/borrower-search`.

Before opening a pull request, build `finals-signinup.sln` in Visual Studio with **Debug | x64** and test the changed menu path. Describe the change and its test steps in the pull request.

Do not commit `borrowing_data.txt`, passwords, personal borrower records, `.vs` files, or compiled `x64` output. The `.gitignore` excludes generated files, but check `git status` before every commit.

Repository collaborators can push branches directly. Other contributors can fork the public repository and submit a pull request.
