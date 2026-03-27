#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include "BookCatalog.h"
#include "TransactionManager.h"
#include "LibrarySystem.h"
#include "ProjectExceptions.h"

using namespace std;

void pause() {
    cout << "\nPress 0 to go back to the previous menu...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

void clearBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void promptBorrowAfterSearch(Member* m, BookCatalog& cat, TransactionManager& tm) {
    cout << "\n--> Would you like to borrow a book from these results? (y/n): ";
    char choice;
    cin >> choice;

    if (choice == 'y' || choice == 'Y') {
        string id;
        cout << "Enter the Book ID to borrow: ";
        cin >> id;

        Book* targetBook = cat.getBook(id);
        if (targetBook) {
            tm.issueBook(targetBook, m);
        } else {
            cout << "Error: Book ID '" << id << "' not found.\n";
        }
    }

    clearBuffer();
    pause();
}

void registerNewMember(Repository<UserAccount*>& userRepo) {
    string name, uname, pwd;
    int age;

    cout << "\n--- Join the Library ---\n";
    cout << "Enter desired username: ";
    cin >> uname;

    try {
        for (int i = 0; i < userRepo.getSize(); i++) {
            if (userRepo[i]->getUsername() == uname) {
                throw UsernameTakenException(uname);
            }
        }

        cout << "Enter Full Name: ";
        clearBuffer();
        getline(cin, name);

        cout << "Enter Age: ";
        cin >> age;

        cout << "Enter Password: ";
        cin >> pwd;

        Member* m = new Member(name, age, uname, pwd);
        userRepo.add(m);
        userRepo.saveAll("users.txt");

        cout << "\n[SUCCESS] Account created! Your Member ID is: " << m->getMemberId() << endl;
    } catch (const UsernameTakenException& e) {
        cout << "\a\n[!] " << e.what() << endl;
    } catch (const FileAccessException& e) {
        cout << "\a\n[!] " << e.what() << endl;
    }

    pause();
}

void memberDashboard(Member* m, BookCatalog& cat, TransactionManager& tm) {
    int choice = -1;
    while (choice != 0) {
        cout << "\n>>> MEMBER DASHBOARD: " << m->getName() << " <<<\n";
        cout << "1. Browse Entire Catalog\n";
        cout << "2. Search by Title\n";
        cout << "3. Search by Author\n";
        cout << "4. Borrow a Book (Direct via ID)\n";
        cout << "5. Return a Book\n";
        cout << "6. My Account Summary & Fines\n";
        cout << "0. Logout / Back\n";
        cout << "Selection: ";

        if (!(cin >> choice)) {
            clearBuffer();
            continue;
        }

        switch (choice) {
            case 1:
                cat.displayAllBooks();
                promptBorrowAfterSearch(m, cat, tm);
                break;
            case 2: {
                string keyword;
                cout << "Enter Title keyword: ";
                clearBuffer();
                getline(cin, keyword);
                cat.searchByTitle(keyword);
                promptBorrowAfterSearch(m, cat, tm);
                break;
            }
            case 3: {
                string keyword;
                cout << "Enter Author keyword: ";
                clearBuffer();
                getline(cin, keyword);
                cat.searchByAuthor(keyword);
                promptBorrowAfterSearch(m, cat, tm);
                break;
            }
            case 4: {
                string id;
                cout << "Enter Book ID to check out: ";
                cin >> id;
                try {
                    tm.issueBook(cat.getBook(id), m);
                } catch (const DuplicateLoanException& e) {
                    cout << "[TM] " << e.what() << "\n";
                }
                clearBuffer();
                pause();
                break;
            }
            case 5: {
                string id;
                cout << "Enter Book ID to return: ";
                cin >> id;
                tm.returnBook(cat.getBook(id), m);
                clearBuffer();
                pause();
                break;
            }
            case 6:
                tm.displayMemberSummary(m->getUsername());
                pause();
                break;
            case 0:
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid selection. Please try again.\n";
                pause();
                break;
        }
    }
}

void librarianDashboard(Librarian* l, BookCatalog& cat, TransactionManager& tm) {
    int choice = -1;
    while (choice != 0) {
        cout << "\n>>> LIBRARIAN CONSOLE (Staff ID: " << l->getStaffId() << ") <<<\n";
        cout << "1. Add New Book to Inventory\n";
        cout << "2. Remove Book from Inventory\n";
        cout << "3. View All Books\n";
        cout << "4. View Active Global Loans\n";
        cout << "0. Logout / Back\n";
        cout << "Selection: ";

        if (!(cin >> choice)) {
            clearBuffer();
            continue;
        }

        switch (choice) {
            case 1: {
                string t, a;
                cout << "Enter Book Title: ";
                clearBuffer();
                getline(cin, t);
                cout << "Enter Author Name: ";
                getline(cin, a);

                Book newBook(t, a);
                cat.addBook(newBook);

                Book* stableBook = cat.getBook(newBook.getId());
                if (stableBook) {
                    l->addBook(*stableBook);
                    cout << "[Success] Book added with ID: " << stableBook->getId() << endl;
                }
                pause();
                break;
            }
            case 2: {
                string id;
                cout << "Enter Book ID to remove: ";
                cin >> id;
                cat.removeBook(id);
                clearBuffer();
                pause();
                break;
            }
            case 3:
                cat.displayAllBooks();
                clearBuffer();
                pause();
                break;
            case 4:
                tm.showActiveLoans();
                clearBuffer();
                pause();
                break;
            case 0:
                cout << "Logging out...\n";
                break;
            default:
                cout << "Invalid selection.\n";
                pause();
                break;
        }
    }
}

int main() {
    BookCatalog catalog;
    TransactionManager tm;
    Repository<UserAccount*> userRepo;

    try {
        catalog.loadFromFile("books.txt");
        tm.loadFromFile("loans.txt");
        userRepo.loadAll("users.txt");
    } catch (const FileAccessException& e) {
        cout << "[Warning] " << e.what() << "\n";
    }

    if (userRepo.getSize() == 0) {
        userRepo.add(new Librarian("System Admin", 99, "admin", "admin", "ADM-001"));
    }

    int mainChoice = -1;
    while (mainChoice != 0) {
        cout << "\n=================================\n";
        cout << "     LIBRARY MANAGEMENT SYSTEM    \n";
        cout << "=================================\n";
        cout << "1. Login\n";
        cout << "2. Register New Member\n";
        cout << "0. Shut Down System\n";
        cout << "Choice: ";

        if (!(cin >> mainChoice)) {
            clearBuffer();
            continue;
        }

        if (mainChoice == 2) {
            registerNewMember(userRepo);
        } else if (mainChoice == 1) {
            try {
                string u, p;
                cout << "Username: "; cin >> u;
                cout << "Password: "; cin >> p;

                UserAccount* sessionUser = nullptr;
                for (int i = 0; i < userRepo.getSize(); i++) {
                    UserAccount* acc = userRepo[i];
                    if (acc->getUsername() == u && acc->verifyPassword(p)) {
                        sessionUser = acc;
                        break;
                    }
                }

                if (!sessionUser) {
                    throw LoginFailedException();
                }

                sessionUser->login();
                if (sessionUser->getRole() == "Member") {
                    memberDashboard(static_cast<Member*>(sessionUser), catalog, tm);
                } else if (sessionUser->getRole() == "Librarian") {
                    librarianDashboard(static_cast<Librarian*>(sessionUser), catalog, tm);
                }
                sessionUser->logout();
            } catch (const LoginFailedException& e) {
                cout << "\a\n[!] " << e.what() << "\n";
                pause();
            }
        }
    }

    try {
        catalog.saveToFile("books.txt");
        tm.saveToFile("loans.txt");
        userRepo.saveAll("users.txt");
    } catch (const FileAccessException& e) {
        cout << "[Warning] " << e.what() << "\n";
    }

    for (int i = 0; i < userRepo.getSize(); i++) {
        delete userRepo[i];
    }

    cout << "\nSystem safely shut down. Data saved to disk.\n";
    return 0;
}
