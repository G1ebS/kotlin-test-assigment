#pragma once

#include "library.h"
#include "users.h"
#include "book.h"
#include <chrono>
#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <string>
#include <optional>
#include <memory>
#ifdef _WIN32
    #include <windows.h>
#endif

struct UserParams {
    std::string name;
    std::string email;
    int id;
};




template <typename Duration>
class LibraryConsole {
public:
    LibraryConsole(Duration day_duration) : library_(day_duration) {};

    void run() {
        #ifdef _WIN32
            SetConsoleCP(1251);
            SetConsoleOutputCP(1251); 
        #endif
        while (true) {
            MainMenu();
            int choice = getUserChoice();
            handleUserChoice(choice);
        }
    }

private:
    Library<Duration> library_;

    void MainMenu() {
        std::cout << "=== Library Management ===\n";
        std::cout << "1. Book Management\n";
        std::cout << "2. User Management\n";
        std::cout << "3. Borrowing Operations\n";
        std::cout << "4. Exit\n";
    }

    int getUserInt(const std::string& prompt) {
        std::string input;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, input);
            try {
                return std::stoi(input);
            } catch (const std::exception&) {
                std::cout << "Invalid input. Please enter a number.\n";
            }
        }
    }



    std::string getValidString(const std::string& prompt) {
        std::string input;
        while (true) {
            std::cout << prompt;
            std::getline(std::cin, input);
            
            input.erase(0, input.find_first_not_of(" \t\n\r"));
            input.erase(input.find_last_not_of(" \t\n\r") + 1);
            
            if (!input.empty()) {
                return input;
            }
            std::cout << "Input cannot be empty. Please try again.\n";
        }
    }

    int getUserChoice() {
        return getUserInt("Enter your choice: ");
    }

    void handleUserChoice(int choice) {
        switch (choice) {
        case 1:
            BookManagementMenu();
            int book_menu_choice;
            book_menu_choice = getUserChoice();
            handleBookManagementChoice(book_menu_choice);
            break;
        case 2:
            UserManagementMenu();
            int user_menu_choice;
            user_menu_choice = getUserChoice();
            handleUserManagementChoice(user_menu_choice);
            break;
        case 3:
            BorrowingOperationsMenu();
            int borrowing_menu_choice;
            borrowing_menu_choice = getUserChoice();
            handleBorrowingOperationsChoice(borrowing_menu_choice);
            break;
        case 4:
            std::cout << "Exiting...\n";
            exit(0);
        default:
            std::cout << "Invalid choice. Please try again.\n";
        }
    }

    void BookManagementMenu() {
        std::cout << "=== Book Management ===\n";
        std::cout << "1. Add Book\n";
        std::cout << "2. Remove Book\n";
        std::cout << "3. View All Books\n";
        std::cout << "4. Search Book by ID\n";
        std::cout << "5. Search Book by Name\n";
        std::cout << "6. View Books by Author\n";
        std::cout << "7. View Books by Genre\n";
        std::cout << "8. View All Genres\n";
        std::cout << "9. View All Authors\n";
        std::cout << "10. Back to Main Menu\n";
    }

    void handleBookManagementChoice(int choice) {
        while (true) {
            switch (choice) {
            case 1:
                addBook();
                break;
            case 2:
                removeBook();
                break;
            case 3:
                viewAllBooks();
                break;
            case 4:
                searchBookByID();
                break;
            case 5:
                searchBookByName();
                break;
            case 6:
                searchBookByAuthor();
                break;
            case 7:
                searchBookByGenre();
                break;
            case 8:
                getAllGenres();
                break;
            case 9:
                getAllAuthors();
                break;
            case 10:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
            }
            
            BookManagementMenu();
            choice = getUserChoice();
        }
    }


    void addBook() {
        int id = library_.get_next_book_id();
        std::string name = getValidString("Enter book name: ");
        std::string author = getValidString("Enter book author: ");
        std::string genre = getValidString("Enter book genre: ");

        Book new_book(name, author, genre, id);
        try {
            library_.add_book(new_book);
            std::cout << "Book added successfully with ID: " << id << "\n";
        } catch (const std::exception& e) {
            std::cout << "Error adding book: " << e.what() << "\n";
        }
    }


    void removeBook() {
        int id;
        id = getUserInt("Enter book ID to remove: ");
        try {
            library_.remove_book(id);
            std::cout << "Book removed successfully.\n";
        } catch (const std::exception& e) {
            std::cout << "Error removing book: " << e.what() << "\n";
            return;
        }
    }

    void viewAllBooks() {
        std::cout << "=== All Books ===\n";
        for (const auto& pair : library_.get_all_books()) {
            const Book& book = pair.second;
            std::cout << "ID: " << book.get_id() << ", Name: " << book.get_name()
                      << ", Author: " << book.get_author() << ", Genre: " << book.get_genre() << "\n";
        }
    }

    void searchBookByID() {
        int id;
        id = getUserInt("Enter book ID to search: ");
        try {
            std::optional<Book> book = library_.get_book_by_id(id);
            if (book) {
                std::cout << "Book found: ID: " << book->get_id() << ", Name: " << book->get_name()
                          << ", Author: " << book->get_author() << ", Genre: " << book->get_genre() << "\n";
            } else {
                std::cout << "Book not found with ID: " << id << "\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Error searching book: " << e.what() << "\n";
        }
    }

    void searchBookByName() {
        std::string name = getValidString("Enter book name to search: ");
        try {
            auto books = library_.get_books_by_name(name);
            if (books.empty()) {
                std::cout << "No books found with the name: " << name << "\n";
                return;
            }
            for (const auto& book : books) {
                std::cout << "ID: " << book.get_id() << ", Name: " << book.get_name()
                        << ", Author: " << book.get_author() << ", Genre: " << book.get_genre() << "\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Error searching book: " << e.what() << "\n";
        }
    }

    void searchBookByAuthor() {
        std::string author = getValidString("Enter author name to search: ");
        try {
            auto books = library_.get_books_by_author(author);
            if (books.empty()) {
                std::cout << "No books found by the author: " << author << "\n";
                return;
            }
            for (const auto& book : books) {
                std::cout << "ID: " << book.get_id() << ", Name: " << book.get_name()
                        << ", Author: " << book.get_author() << ", Genre: " << book.get_genre() << "\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Error searching book: " << e.what() << "\n";
        }
    }

    void searchBookByGenre() {
        std::string genre = getValidString("Enter genre to search: ");
        try {
            auto books = library_.get_books_by_genre(genre);
            if (books.empty()) {
                std::cout << "No books found in the genre: " << genre << "\n";
                return;
            }
            for (const auto& book : books) {
                std::cout << "ID: " << book.get_id() << ", Name: " << book.get_name()
                        << ", Author: " << book.get_author() << ", Genre: " << book.get_genre() << "\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Error searching book: " << e.what() << "\n";
        }
    }

    void getAllGenres() {
        auto genres = library_.get_all_genres();
        std::cout << "=== All Genres ===\n";
        for (const auto& genre : genres) {
            std::cout << genre << "\n";
        }
    }

    void getAllAuthors() {
        auto authors = library_.get_all_authors();
        std::cout << "=== All Authors ===\n";
        for (const auto& author : authors) {
            std::cout << author << "\n";
        }
    }



    void UserManagementMenu() {
        std::cout << "=== User Management ===\n";
        std::cout << "1. Add User\n";
        std::cout << "2. Remove User\n";
        std::cout << "3. View All Users\n";
        std::cout << "4. Search User by ID\n";
        std::cout << "5. Back to Main Menu\n";
    }

    void handleUserManagementChoice(int choice) {
        while (true) {
            switch (choice) {
            case 1: {
                UserParams params = addUser();
                UserTypeChoiceMenu();
                int user_type_choice = getUserChoice();
                handleUserTypeChoice(user_type_choice, params);
                break;
            }
            case 2:
                removeUser();
                break;
            case 3:
                viewAllUsers();
                break;
            case 4:
                searchUserById();
                break;
            case 5:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
            }
            
            UserManagementMenu();
            choice = getUserChoice();
        }
    }



    void UserTypeChoiceMenu() {
        std::cout << "=== User Type ===\n";
        std::cout << "1. Student\n";
        std::cout << "2. Faculty\n";
        std::cout << "3. Guest\n";
    }

    void handleUserTypeChoice(int choice, UserParams params) {
        while (true) {
            switch (choice) {
            case 1:
                addStudent(params);
                return;
                break;
            case 2:
                addFaculty(params);
                return;
                break;
            case 3:
                addGuest(params);
                return;
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
            }
            UserTypeChoiceMenu();
            choice = getUserChoice();
        }
    }

    UserParams addUser() {
        UserParams params;
        params.name = getValidString("Enter user name: ");
        params.email = getValidString("Enter user email: ");
        params.id = library_.get_next_user_id();
        return params;
    }


    void addFaculty(UserParams params) {
        Faculty new_faculty(params.name, params.email, params.id);
        try {
            library_.add_user(std::make_shared<Faculty>(new_faculty));
            std::cout << "Faculty added successfully.\n";
        } catch (const std::exception& e) {
            std::cout << "Error adding faculty: " << e.what() << "\n";
            return;
        }   
    }

    void addGuest(UserParams params) {
        Guest new_guest(params.name, params.email, params.id);
        try {
            library_.add_user(std::make_shared<Guest>(new_guest));
            std::cout << "Guest added successfully.\n";
        } catch (const std::exception& e) {
            std::cout << "Error adding guest: " << e.what() << "\n";
            return;
        }   
    }

    void addStudent(UserParams params) {
        Student new_student(params.name, params.email, params.id);
        try {
            library_.add_user(std::make_shared<Student>(new_student));
            std::cout << "Student added successfully.\n";
        } catch (const std::exception& e) {
            std::cout << "Error adding student: " << e.what() << "\n";
            return;
        }   
    }


    void removeUser() {
        int id;
        id = getUserInt("Enter user ID to remove: ");
        try {
            library_.remove_user(id);
            std::cout << "User removed successfully.\n";
        } catch (const std::exception& e) {
            std::cout << "Error removing user: " << e.what() << "\n";
            return;
        }
    }

    void viewAllUsers() {
        auto users = library_.get_all_users();
        std::cout << "=== All Users ===\n";
        for (const auto& [id, user] : users) {
            std::cout << "User: " << user->get_name() << " email: " << user->get_email() << " has " << user->get_borrowed_books().size() << " borrowed books with penalty: " << user->get_penalty_value() << " (ID: " << id << ")\n";
        }
    }

    void searchUserById() {
        int id;
        id = getUserInt("Enter user ID to search: ");
        auto user = library_.get_user_by_id(id);
        if (user) {
            std::cout << "Found User: " << user->get_name() << " email: " << user->get_email() << " has " << user->get_borrowed_books().size() << " borrowed books with penalty: " << user->get_penalty_value() << " (ID: " << id << ")\n";
        } else {
            std::cout << "User not found.\n";
        }
    }


    void BorrowingOperationsMenu() {
        std::cout << "=== Borrowing Operations ===\n";
        std::cout << "1. Borrow Book\n";
        std::cout << "2. Return Book\n";
        std::cout << "3. View Borrowed Operations\n";
        std::cout << "4. Get Borrowed Books\n";
        std::cout << "5. Get Overdue Books\n";
        std::cout << "6. Back to Main Menu\n";
    }


    void handleBorrowingOperationsChoice(int choice) {
        while (true) {
            switch (choice) {
            case 1:
                borrowBook();
                break;
            case 2:
                returnBook();
                break;
            case 3:
                viewAllBorrowedOperations();
                break;
            case 4:
                viewBorrowedBooks();
                break;
            case 5:
                viewOverdueBooks();
                break;
            case 6:
                return;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
            }
            BorrowingOperationsMenu();
            choice = getUserChoice();
        }
    }


    void borrowBook() {
        int user_id, book_id;
        user_id = getUserInt("Enter user ID: ");
        book_id = getUserInt("Enter book ID: ");
        try {
            library_.borrow_book(user_id, book_id);
            std::cout << "Book borrowed successfully.\n";
        } catch (const std::exception& e) {
            std::cout << "Error borrowing book: " << e.what() << "\n";
        }
    }

    void returnBook() {
        int book_id;
        book_id = getUserInt("Enter book ID to return: ");
        try {
            int penalty = library_.return_book(book_id);
            if (penalty > 0) {
                std::cout << "Book returned with a penalty of: " << penalty << "\n";
            } else {
                std::cout << "Book returned successfully with no penalty.\n";
            }
        } catch (const std::exception& e) {
            std::cout << "Error returning book: " << e.what() << "\n";
        }
    }


    void viewAllBorrowedOperations() {
        auto history = library_.get_borrow_history();
        std::cout << "=== Borrowed Operations(from newest to oldest) ===\n";
        for (const auto& record : history) {
            int user_id, book_id;
            BorrowOperationType op_type;
            std::tie(user_id, book_id, op_type) = record;
            std::string operation = (op_type == BorrowOperationType::BORROW) ? "BORROW" : "RETURN";
            std::cout << "User ID: " << user_id << ", Book ID: " << book_id << ", Operation: " << operation << "\n";
        }
    }


    void viewBorrowedBooks() {
        std::set<Book> borrowed_books = library_.get_borrowed_books();
        for (auto book: borrowed_books) {
            std::cout << "ID: " << book.get_id() << ", Name: " << book.get_name() << "\n";
        }
    }

    void viewOverdueBooks() {
        std::set<Book> overdue_books = library_.get_overdue_books();
        for (auto book: overdue_books) {
            std::cout << "ID: " << book.get_id() << ", Name: " << book.get_name() << "\n";
        }
    }

};