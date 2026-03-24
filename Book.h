#ifndef BOOK_H
#define BOOK_H
#include <string>

class Book {
private:
    std::string id;
    std::string title;
    std::string author;
    bool isAvailable;
    static int nextIdNumber;   // shared across all Book objects

public:
    // Auto-generates an ID like B001, B002, ...
    Book(const std::string& title, const std::string& author);

    // Used when loading a book from file (ID already known)
    Book(const std::string& id,
         const std::string& title,
         const std::string& author);

    const std::string& getId()     const;
    const std::string& getTitle()  const;
    const std::string& getAuthor() const;
    bool getAvailability()         const;

    void setAvailability(bool status);

    void display() const;

    // Lets other classes preview the next ID without creating a Book
    static std::string peekNextId();
};

#endif
