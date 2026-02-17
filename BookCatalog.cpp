#include<iostream>
#include "Book.h"
#include "BookCatalog.h"

    BookCatalog::BookCatalog(){

    }

    void BookCatalog::addBook(const Book& book){
        books.push_back(book);
        std::cout<<"The book \"" <<book.getTitle()<< "\" has been added successfully!!"<<std::endl;
    }

    Book* BookCatalog::getBook(const std::string& id){
        for(size_t i=0; i<books.size(); i++){
            if(books[i].getId() == id){
                return &books[i];
            }
        }
        return nullptr;
    } 

    void BookCatalog::displayAllBooks()const{
        if(books.empty()){
            std::cout<<"No Books for the moment come later!!"<<std::endl;
            return;
        }
        std::cout<<"---Welcome to the Book Catalog!!!--- "<<std::endl;

        for(size_t i=0; i<books.size(); i++){
            books[i].display();
        }
        std::cout<<"Thank you for the visit!!"<<std::endl;
    }

    int BookCatalog::getTotalBooks()const{
        return books.size();
    }
