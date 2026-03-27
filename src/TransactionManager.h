#ifndef TRANSACTIONMANAGER_H
#define TRANSACTIONMANAGER_H

#include "Book.h"
#include "Fine.hpp"
#include "Loan.h"
#include "Repository.h"
#include <string>
#include <iostream>

class Member;

class TransactionManager {
private:
    Repository<Loan> loans;
    int maxLoansPerMember;

    int findActiveLoan(const std::string& bookId,
                       const std::string& memberUsername) const;
    int countActiveLoans(const std::string& memberUsername) const;

public:
    explicit TransactionManager(int maxLoansPerMember = 3);

    bool issueBook(Book* book, Member* member);
    Fine returnBook(Book* book, Member* member);

    void showActiveLoans() const;
    void showOverdueLoans() const;
    int getTotalLoans() const;
    bool hasActiveLoanForBook(const std::string& bookId) const;
    void displayMemberSummary(const std::string& uname);

    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

#endif
