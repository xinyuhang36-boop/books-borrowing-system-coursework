#include <algorithm>
#include <array>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>
using namespace std;

// Coursework methods: arrays of structures, functions, validation and file processing.
constexpr size_t LIMIT = 200;
struct Admin { string id, password; };
struct Borrower { string name, address, contact; };
struct Book { string title, author, isbn; int total, available; };
struct Loan { string borrower, isbn; };
struct State {
    array<Admin, LIMIT> admins{};
    array<Borrower, LIMIT> borrowers{};
    array<Book, LIMIT> books{};
    array<Loan, LIMIT> loans{};
    size_t adminCount = 0, borrowerCount = 0, bookCount = 0, loanCount = 0;
};

string trim(const string& s) {
    auto a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    return s.substr(a, s.find_last_not_of(" \t\r\n") - a + 1);
}
string folded(string s) {
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return s;
}
bool textOk(const string& s, size_t maxLength) {
    return !s.empty() && s.size() <= maxLength &&
        none_of(s.begin(), s.end(), [](unsigned char c) { return c < 32 || c == 127; });
}
bool copiesOk(const string& s, int& value) {
    string t = trim(s);
    if (t.empty() || t.size() > 6 ||
        !all_of(t.begin(), t.end(), [](unsigned char c) { return isdigit(c) != 0; })) return false;
    value = stoi(t);
    return value >= 1 && value <= 100000;
}
string normalizedIsbn(const string& s) {
    string result;
    for (unsigned char c : s) if (c != '-' && c != ' ') result += static_cast<char>(toupper(c));
    return result;
}
bool isbnOk(const string& s) {
    if (s.size() != 10 && s.size() != 13) return false;
    for (size_t i = 0; i < s.size(); ++i)
        if (!isdigit(static_cast<unsigned char>(s[i])) && !(s.size() == 10 && i == 9 && s[i] == 'X'))
            return false;
    return true;
}
int adminAt(const State& s, const string& id) {
    for (size_t i = 0; i < s.adminCount; ++i)
        if (folded(s.admins[i].id) == folded(id)) return static_cast<int>(i);
    return -1;
}
int borrowerAt(const State& s, const string& name) {
    for (size_t i = 0; i < s.borrowerCount; ++i)
        if (folded(s.borrowers[i].name) == folded(name)) return static_cast<int>(i);
    return -1;
}
int bookAtIsbn(const State& s, const string& isbn) {
    for (size_t i = 0; i < s.bookCount; ++i)
        if (s.books[i].isbn == isbn) return static_cast<int>(i);
    return -1;
}
int bookAtTitle(const State& s, const string& title) {
    for (size_t i = 0; i < s.bookCount; ++i)
        if (folded(s.books[i].title) == folded(title)) return static_cast<int>(i);
    return -1;
}
int loanAt(const State& s, const string& borrower, const string& isbn) {
    for (size_t i = 0; i < s.loanCount; ++i)
        if (folded(s.loans[i].borrower) == folded(borrower) && s.loans[i].isbn == isbn)
            return static_cast<int>(i);
    return -1;
}
string createAdmin(State& s, const string& id, const string& password) {
    if (!textOk(id, 40) || id.find(' ') != string::npos) return "ID must be 1-40 characters without spaces.";
    if (!textOk(password, 80) || password.size() < 8) return "Password must be 8-80 characters.";
    if (adminAt(s, id) >= 0) return "Admin ID already exists.";
    if (s.adminCount == LIMIT) return "Admin storage is full.";
    s.admins[s.adminCount++] = {id, password};
    return "";
}
string addBorrower(State& s, const string& name, const string& address, const string& contact) {
    if (!textOk(name, 80) || !textOk(address, 120) || !textOk(contact, 60))
        return "Name, address and contact are required (limits: 80, 120, 60).";
    if (borrowerAt(s, name) >= 0) return "Borrower name already exists.";
    if (s.borrowerCount == LIMIT) return "Borrower storage is full.";
    s.borrowers[s.borrowerCount++] = {name, address, contact};
    return "";
}
string addBook(State& s, const string& title, const string& author, const string& isbn, int copies) {
    if (!textOk(title, 120) || !textOk(author, 80)) return "Title and author are required.";
    if (!isbnOk(isbn)) return "ISBN must have 10 or 13 digits (X can end ISBN-10).";
    if (copies < 1 || copies > 100000) return "Copies must be between 1 and 100000.";
    if (bookAtIsbn(s, isbn) >= 0 || bookAtTitle(s, title) >= 0) return "ISBN or title already exists.";
    if (s.bookCount == LIMIT) return "Book storage is full.";
    s.books[s.bookCount++] = {title, author, isbn, copies, copies};
    return "";
}
string addCopies(State& s, const string& title, int copies) {
    int book = bookAtTitle(s, title);
    if (book < 0) return "Book not found.";
    if (copies < 1 || copies > 100000 - s.books[book].total) return "Total copies cannot exceed 100000.";
    s.books[book].total += copies;
    s.books[book].available += copies;
    return "";
}
string borrowBook(State& s, const string& borrower, const string& title) {
    int person = borrowerAt(s, borrower), book = bookAtTitle(s, title);
    if (person < 0) return "Borrower not found.";
    if (book < 0) return "Book not found.";
    Book& b = s.books[book];
    if (b.available == 0) return "No copies available.";
    if (loanAt(s, s.borrowers[person].name, b.isbn) >= 0) return "Borrower already has this book.";
    if (s.loanCount == LIMIT) return "Loan storage is full.";
    s.loans[s.loanCount++] = {s.borrowers[person].name, b.isbn};
    --b.available;
    return "";
}
string returnBook(State& s, const string& borrower, const string& title) {
    int person = borrowerAt(s, borrower), book = bookAtTitle(s, title);
    if (person < 0) return "Borrower not found.";
    if (book < 0) return "Book not found.";
    int loan = loanAt(s, s.borrowers[person].name, s.books[book].isbn);
    if (loan < 0) return "This borrower has no loan for that book.";
    for (size_t i = static_cast<size_t>(loan); i + 1 < s.loanCount; ++i) s.loans[i] = s.loans[i + 1];
    --s.loanCount;
    ++s.books[book].available;
    return "";
}
bool stateOk(const State& s) {
    if (s.adminCount > LIMIT || s.borrowerCount > LIMIT || s.bookCount > LIMIT || s.loanCount > LIMIT) return false;
    for (size_t i = 0; i < s.adminCount; ++i) {
        if (!textOk(s.admins[i].id, 40) || !textOk(s.admins[i].password, 80)) return false;
        for (size_t j = i + 1; j < s.adminCount; ++j)
            if (folded(s.admins[i].id) == folded(s.admins[j].id)) return false;
    }
    for (size_t i = 0; i < s.borrowerCount; ++i) {
        const Borrower& b = s.borrowers[i];
        if (!textOk(b.name, 80) || !textOk(b.address, 120) || !textOk(b.contact, 60)) return false;
        for (size_t j = i + 1; j < s.borrowerCount; ++j)
            if (folded(b.name) == folded(s.borrowers[j].name)) return false;
    }
    for (size_t i = 0; i < s.bookCount; ++i) {
        const Book& b = s.books[i];
        if (!textOk(b.title, 120) || !textOk(b.author, 80) || !isbnOk(b.isbn) ||
            b.total < 1 || b.total > 100000 || b.available < 0 || b.available > b.total) return false;
        int checkedOut = 0;
        for (size_t j = 0; j < s.loanCount; ++j) if (s.loans[j].isbn == b.isbn) ++checkedOut;
        if (b.total - b.available != checkedOut) return false;
        for (size_t j = i + 1; j < s.bookCount; ++j)
            if (b.isbn == s.books[j].isbn || folded(b.title) == folded(s.books[j].title)) return false;
    }
    for (size_t i = 0; i < s.loanCount; ++i) {
        if (borrowerAt(s, s.loans[i].borrower) < 0 || bookAtIsbn(s, s.loans[i].isbn) < 0) return false;
        for (size_t j = i + 1; j < s.loanCount; ++j)
            if (folded(s.loans[i].borrower) == folded(s.loans[j].borrower) &&
                s.loans[i].isbn == s.loans[j].isbn) return false;
    }
    return true;
}

int main() {
    // Console and disk I/O live in main; helpers above only validate and change data.
    State s;
    ifstream data("borrowing_data.txt");
    if (data) {
        string line;
        if (!getline(data, line) || line != "BOOKS_BORROWING_V1") {
            cerr << "Invalid data header; no data changed.\n"; return 1;
        }
        while (getline(data, line)) {
            istringstream row(line);
            char type = '\0';
            row >> type;
            if (type == 'A' && s.adminCount < LIMIT) {
                Admin a; if (!(row >> quoted(a.id) >> quoted(a.password))) break;
                s.admins[s.adminCount++] = a;
            } else if (type == 'B' && s.borrowerCount < LIMIT) {
                Borrower b; if (!(row >> quoted(b.name) >> quoted(b.address) >> quoted(b.contact))) break;
                s.borrowers[s.borrowerCount++] = b;
            } else if (type == 'K' && s.bookCount < LIMIT) {
                Book b;
                if (!(row >> quoted(b.title) >> quoted(b.author) >> quoted(b.isbn) >> b.total >> b.available)) break;
                s.books[s.bookCount++] = b;
            } else if (type == 'L' && s.loanCount < LIMIT) {
                Loan l; if (!(row >> quoted(l.borrower) >> quoted(l.isbn))) break;
                s.loans[s.loanCount++] = l;
            } else break;
            row >> ws;
            if (!row.eof()) break;
        }
        if (!data.eof() || !stateOk(s)) {
            cerr << "Data file is invalid; no data changed.\n"; return 1;
        }
        data.close();
    }
    bool loggedIn = false;
    string activeAdmin;
    while (true) {
        State previous = s;
        string choice, message;
        bool changed = false;
        if (!loggedIn) {
            cout << "\nBOOKS BORROWING SYSTEM\n1. Create admin account\n2. Log in\n3. Quit\nChoice: ";
            if (!getline(cin, choice)) return 0;
            if (choice == "1") {
                string id, password;
                cout << "Admin ID: "; if (!getline(cin, id)) return 0;
                cout << "Password (8+ characters): "; if (!getline(cin, password)) return 0;
                message = createAdmin(s, trim(id), password);
                changed = message.empty();
            } else if (choice == "2") {
                string id, password;
                cout << "Admin ID: "; if (!getline(cin, id)) return 0;
                cout << "Password: "; if (!getline(cin, password)) return 0;
                int a = adminAt(s, trim(id));
                if (a >= 0 && s.admins[a].password == password) {
                    loggedIn = true; activeAdmin = s.admins[a].id;
                    cout << "Logged in as " << activeAdmin << ".\n";
                } else cout << "Invalid ID or password.\n";
            } else if (choice == "3") return 0;
            else cout << "Invalid menu choice.\n";
        } else {
            cout << "\nADMIN MENU (" << activeAdmin << ")\n1. Add borrower\n2. Add book\n"
                 << "3. Borrow book\n4. Return book\n5. Display inventory\n"
                 << "6. Display borrowers and loans\n7. Add copies\n8. Log out\nChoice: ";
            if (!getline(cin, choice)) return 0;
            if (choice == "1") {
                string name, address, contact;
                cout << "Borrower name: "; if (!getline(cin, name)) return 0;
                cout << "Address: "; if (!getline(cin, address)) return 0;
                cout << "Contact: "; if (!getline(cin, contact)) return 0;
                message = addBorrower(s, trim(name), trim(address), trim(contact));
                changed = message.empty();
            } else if (choice == "2") {
                string title, author, isbn, quantity;
                int copies = 0;
                cout << "Book title: "; if (!getline(cin, title)) return 0;
                cout << "Author: "; if (!getline(cin, author)) return 0;
                cout << "ISBN: "; if (!getline(cin, isbn)) return 0;
                cout << "Number of copies: "; if (!getline(cin, quantity)) return 0;
                if (!copiesOk(quantity, copies)) message = "Copies must be a whole number from 1 to 100000.";
                else message = addBook(s, trim(title), trim(author), normalizedIsbn(isbn), copies);
                changed = message.empty();
            } else if (choice == "3" || choice == "4") {
                string name, title;
                cout << "Borrower name: "; if (!getline(cin, name)) return 0;
                cout << "Book title: "; if (!getline(cin, title)) return 0;
                message = choice == "3" ? borrowBook(s, trim(name), trim(title))
                                        : returnBook(s, trim(name), trim(title));
                changed = message.empty();
            } else if (choice == "5") {
                if (!s.bookCount) cout << "No books in inventory.\n";
                for (size_t i = 0; i < s.bookCount; ++i) {
                    const Book& b = s.books[i];
                    cout << "\nTitle: " << b.title << "\nAuthor: " << b.author << "\nISBN: " << b.isbn
                         << "\nTotal: " << b.total << "\nAvailable: " << b.available
                         << "\nChecked out: " << b.total - b.available << "\n";
                }
            } else if (choice == "6") {
                if (!s.borrowerCount) cout << "No borrowers registered.\n";
                for (size_t i = 0; i < s.borrowerCount; ++i) {
                    const Borrower& b = s.borrowers[i];
                    cout << "\nName: " << b.name << "\nAddress: " << b.address
                         << "\nContact: " << b.contact << "\nChecked-out books:\n";
                    bool any = false;
                    for (size_t j = 0; j < s.loanCount; ++j) {
                        if (folded(s.loans[j].borrower) != folded(b.name)) continue;
                        int k = bookAtIsbn(s, s.loans[j].isbn);
                        cout << "  - " << s.books[k].title << " (ISBN " << s.books[k].isbn << ")\n";
                        any = true;
                    }
                    if (!any) cout << "  None\n";
                }
            } else if (choice == "7") {
                string title, quantity;
                int copies = 0;
                cout << "Book title: "; if (!getline(cin, title)) return 0;
                cout << "Additional copies: "; if (!getline(cin, quantity)) return 0;
                if (!copiesOk(quantity, copies)) message = "Copies must be a whole number from 1 to 100000.";
                else message = addCopies(s, trim(title), copies);
                changed = message.empty();
            } else if (choice == "8") {
                loggedIn = false; activeAdmin.clear(); cout << "Logged out.\n";
            } else cout << "Invalid menu choice.\n";
        }
        if (!changed) {
            if (!message.empty()) cout << message << '\n';
            continue;
        }
        ofstream out("borrowing_data.tmp", ios::trunc);
        if (out) {
            out << "BOOKS_BORROWING_V1\n";
            for (size_t i = 0; i < s.adminCount; ++i)
                out << "A " << quoted(s.admins[i].id) << ' ' << quoted(s.admins[i].password) << '\n';
            for (size_t i = 0; i < s.borrowerCount; ++i)
                out << "B " << quoted(s.borrowers[i].name) << ' ' << quoted(s.borrowers[i].address)
                    << ' ' << quoted(s.borrowers[i].contact) << '\n';
            for (size_t i = 0; i < s.bookCount; ++i)
                out << "K " << quoted(s.books[i].title) << ' ' << quoted(s.books[i].author) << ' '
                    << quoted(s.books[i].isbn) << ' ' << s.books[i].total << ' ' << s.books[i].available << '\n';
            for (size_t i = 0; i < s.loanCount; ++i)
                out << "L " << quoted(s.loans[i].borrower) << ' ' << quoted(s.loans[i].isbn) << '\n';
            out.flush();
        }
        bool wrote = out.good();
        out.close();
        if (!wrote || !MoveFileExA("borrowing_data.tmp", "borrowing_data.txt",
                                   MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            s = previous;
            cerr << "Could not save; change cancelled.\n";
        } else cout << "Saved successfully.\n";
    }
}
