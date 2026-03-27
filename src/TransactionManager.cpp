#include "TransactionManager.h"
#include "LibrarySystem.h"
#include "ProjectExceptions.h"
#include <iostream>
#include<iomanip>

using namespace std;

// ============================================================
//  Constructor
// ============================================================
TransactionManager::TransactionManager(int maxLoans)
    : maxLoansPerMember(maxLoans) {}

// ============================================================
//  Private helpers
// ============================================================
int TransactionManager::findActiveLoan(const string& bookId,
                                       const string& memberUsername) const {
    for (int i = 0; i < loans.getSize(); i++) {
        const Loan* l = &loans[i];
        if (l->getBookId() == bookId &&
            l->getMemberUsername() == memberUsername &&
            !l->isReturned())
            return i;
    }
    return -1;
}

int TransactionManager::countActiveLoans(const string& memberUsername) const {
    int count = 0;
    for (int i = 0; i < loans.getSize(); i++) {
        const Loan* l = &loans[i];
        if (l->getMemberUsername() == memberUsername && !l->isReturned())
            count++;
    }
    return count;
}

// ============================================================
//  issueBook
// ============================================================
bool TransactionManager::issueBook(Book* book, Member* member) {
    if (!book || !member) {
        cout << "[TM] Error: null book or member.\n";
        return false;
    }
    if (!book->getAvailability()) {
        cout << "[TM] \"" << book->getTitle() << "\" is currently borrowed.\n";
        return false;
    }
    if (!member->getIsLoggedIn()) {
        cout << "[TM] Member must be logged in to borrow.\n";
        return false;
    }
    if (countActiveLoans(member->getUsername()) >= maxLoansPerMember) {
        cout << "[TM] Borrow limit reached (" << maxLoansPerMember << " books).\n";
        return false;
    }

    // Loan constructor calculates dueDate automatically
    Loan newLoan(book->getId(), member->getUsername(), Loan::today());
    loans.add(newLoan);
    book->setAvailability(false);

    cout << "[TM] \"" << book->getTitle() << "\" issued to " << member->getName()
         << ". Due: " << newLoan.getDueDate() << ".\n";
    return true;
}

// ============================================================
//  returnBook
//  Marks the loan returned, then delegates all fine logic to Loan.
// ============================================================
Fine TransactionManager::returnBook(Book* book, Member* member) {
    Fine noFine;

    if (!book || !member) {
        cout << "[TM] Error: null book or member.\n";
        return noFine;
    }

    int idx = findActiveLoan(book->getId(), member->getUsername());
    if (idx == -1) {
        cout << "[TM] No active loan found for \"" << book->getTitle() << "\".\n";
        return noFine;
    }

    // Get a writable pointer and mark returned
    Loan* loan = &loans[idx];
    loan->markReturned();   // sets returnDate = today
    book->setAvailability(true);

    cout << "[TM] \"" << book->getTitle() << "\" returned by " << member->getName() << ".\n";

    // isOverdue() and fineAmount() are now Loan's responsibility
    if (loan->isOverdue()) {
        double amt = loan->fineAmount();
        cout << "[TM] Overdue by " << loan->overdueDays()
             << " day(s). Fine: $" << amt << ".\n";
        return Fine(amt, "Late return: " + book->getTitle(), loan->overdueDays());
    }

    cout << "[TM] Returned on time. No fine.\n";
    return noFine;
}

// ============================================================
//  Display
// ============================================================
void TransactionManager::showActiveLoans() const {
    cout << "\n--- Active Loans ---\n";
    bool any = false;
    for (int i = 0; i < loans.getSize(); i++) {
        const Loan* l = &loans[i];
        if (!l->isReturned()) {
            cout << *l << "\n";
            any = true;
        }
    }
    if (!any) cout << "  (none)\n";
}

void TransactionManager::showOverdueLoans() const {
    cout << "\n--- Overdue Loans ---\n";
    bool any = false;
    for (int i = 0; i < loans.getSize(); i++) {
        const Loan* l = &loans[i];
        if (!l->isReturned() && l->isOverdue()) {
            cout << *l << " | Fine so far: $" << l->fineAmount() << "\n";
            any = true;
        }
    }
    if (!any) cout << "  (none)\n";
}

int TransactionManager::getTotalLoans() const { return loans.getSize(); }

bool TransactionManager::hasActiveLoanForBook(const string& bookId) const {
    for (int i = 0; i < loans.getSize(); i++) {
        const Loan* l = &loans[i];
        if (l->getBookId() == bookId && !l->isReturned()) {
            return true;
        }
    }
    return false;
}

// Summary of Members loans
void TransactionManager::displayMemberSummary(const string& uname) {
    double totalFines = 0;
    bool hasLoans = false;

    cout << "\n--- ACCOUNT SUMMARY FOR: " << uname << " ---\n";
    cout << left << setw(15) << "Book ID" << setw(15) << "Overdue?" << "Fine Accrued" << endl;
    cout << string(45, '-') << endl;

    for (int i = 0; i < loans.getSize(); i++) {
        const Loan* l = &loans[i];
        if (l->getMemberUsername() == uname && !l->isReturned()) {
            hasLoans = true;
            double fine = l->fineAmount();
            totalFines += fine;

            cout << left << setw(15) << l->getBookId() 
                 << setw(15) << (l->isOverdue() ? "YES" : "No")
                 << "$" << fixed << setprecision(2) << fine << endl;
        }
    }

    if (!hasLoans) cout << "No active loans found.\n";
    cout << string(45, '-') << endl;
    cout << "TOTAL FINES INCURRED: $" << totalFines << endl;
}

// ============================================================
//  File I/O
// ============================================================
void TransactionManager::saveToFile(const string& filename) const {
    loans.saveAll(filename);
}

void TransactionManager::loadFromFile(const string& filename) {
    loans.loadAll(filename);
}
