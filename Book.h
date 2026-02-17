#ifndef BOOK_H
#define BOOK_H
#include<string>

class Book{
private:
    std::string id; 
    std::string title;
    std::string author;
    bool isAvailable;
    static int nextIdNumber;

public:

    Book(const std::string& title, const std::string& author);

    Book(const std::string& id, 
        const std::string& title,
        const std::string& author);

    const std::string& getTitle() const;
    const std::string& getAuthor() const;
    const std::string& getId() const;
    bool getAvailability() const; 

    void setAvailability(bool status);

    void display() const;

    static std::string peekNextId(); 
};

#endif


