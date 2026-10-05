# Books Borrowing System

C++ console program for the PRG2203 group assignment. The program uses arrays of structures, functions, input validation, menus, and file storage. Console and file input/output are in `main()`; the other functions validate and process records.

## Build and run

Open `PRG_Library.sln` in Visual Studio with the Desktop development with C++ workload. Select **Debug | x64**, build with **Ctrl+Shift+B**, then run with **Ctrl+F5**. Set the working directory to this project folder so that `borrowing_data.txt` is created beside the source files. Alternatively, from a PowerShell window in this folder, run the freshly built executable at `x64\Debug\PRG_Library.exe`.

The menu starts with admin account creation, login and quit. After login, an admin can register borrowers, add books, borrow and return books, view inventory, view borrowers and their current loans, add copies, and log out.

## Records and rules

- Borrowers have a unique name, address and contact. The unique name is used to identify a borrower in the menu.
- Books have a unique title, author, ISBN, total copies and available copies. ISBN accepts 10 or 13 characters after removing spaces and hyphens. Copy counts must be whole numbers from 1 to 100000.
- A borrower may have one copy of a particular book at a time. Borrowing requires an available copy; returning requires an active loan. Each successful change is saved immediately.
- Inventory shows total, available and checked-out copies. The borrower list shows every current loan.
- The program stores admins, borrowers, books and loans in `borrowing_data.txt`. Keep this file private: for this coursework prototype, admin passwords are stored as plain text.

The previous `library_records.txt` and `userProfiles.txt`, if present locally, use a different format and are not imported. The new program starts with an empty database when `borrowing_data.txt` does not exist. It refuses to overwrite an invalid database file.

The report, member-specific source files, weekly monitoring records and demonstration video mentioned in the coursework brief are separate submission items; this repository contains the merged program.

## Collaboration and license

See `CONTRIBUTING.md` for the branch and pull request workflow. The source code is available under the MIT License; see `LICENSE`.
