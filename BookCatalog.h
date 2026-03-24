#ifndef BOOKCATALOG_H
#define BOOKCATALOG_H
#include "Book.h"
#include <vector>
#include <string>

class BookCatalog {
private:
    std::vector<Book> books;

public:
    BookCatalog();

    void  addBook(const Book& book);
    bool  removeBook(const std::string& id);
    Book* getBook(const std::string& id);    // returns nullptr if not found

    void displayAllBooks()                      const;
    void searchByTitle(const std::string& keyword)  const;
    void searchByAuthor(const std::string& keyword) const;

    int getTotalBooks() const;

    // File I/O — format: ID|Title|Author|Availability(1/0)
    void saveToFile(const std::string& filename) const;
    void loadFromFile(const std::string& filename);
};

#endif
