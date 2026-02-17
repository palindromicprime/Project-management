#ifndef BOOKCATALOG_H
#define BOOKCATALOG_H

#include "Book.h"
#include <vector>
#include <string>


class BookCatalog{
private:
    std::vector<Book> books;
public:
    BookCatalog();
    void addBook(const Book& book);
    Book* getBook(const std::string& id);
    void displayAllBooks()const;
    int getTotalBooks()const;
};

#endif