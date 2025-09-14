#pragma once
#include "users.h"
#include "book.h"
#include <algorithm>
#include <unordered_set>
#include <optional>
#include <memory>
#include <deque>
#include <set>

enum class BorrowOperationType {
    BORROW,
    RETURN
};

class IdGenerator {
public:
    IdGenerator() : current_id_(0) {}
    int get_next_id() { return current_id_++; }
private:
    int current_id_;
};


class LibraryOperationException : public std::exception {
public:
    explicit LibraryOperationException(const std::string& message)
        : message_(message) {}

    virtual const char* what() const noexcept override {
        return message_.c_str();
    }

private:
    std::string message_;
};

template <typename Duration>
class Library {
public:
    explicit Library(Duration day_duration): clock_(day_duration), id_generator_() {}

    void add_user(std::shared_ptr<User> user) {
        if (id_to_user_.find(user->get_id()) != id_to_user_.end()) {
            throw LibraryOperationException("User with this ID already exists");
        }
        id_to_user_.emplace(user->get_id(), user);
    }

    void add_book(const Book& book) {
        if (id_to_book_.find(book.get_id()) != id_to_book_.end()) {
            throw LibraryOperationException("Book with this ID already exists");
        }
        id_to_book_.emplace(book.get_id(), book);
        genres_.insert(book.get_genre());
        authors_.insert(book.get_author());
        
        books_by_author_[book.get_author()].insert(book.get_id());
        books_by_genre_[book.get_genre()].insert(book.get_id());
        books_by_name_[book.get_name()].insert(book.get_id());
    }

    void remove_user(int user_id) {
        auto it = id_to_user_.find(user_id);
        if (it == id_to_user_.end()) {
            throw LibraryOperationException("User ID not found");
        }
        auto user = it->second;
        if (user->get_borrowed_books().size() > 0) {
            throw LibraryOperationException("User has borrowed books");
        }
        if (user->get_penalty_value() > 0) {
            throw LibraryOperationException("User has unpaid penalties");
        }
        id_to_user_.erase(user_id);
    }

    void remove_book(int book_id) {
        if (id_to_book_.find(book_id) == id_to_book_.end()) {
            throw LibraryOperationException("Book ID not found");
        }
        Book& book = id_to_book_.at(book_id);
        if (!book.is_available()) {
            throw LibraryOperationException("Book is borrowed");
        }
        
        books_by_author_[book.get_author()].erase(book_id);
        books_by_genre_[book.get_genre()].erase(book_id);
        books_by_name_[book.get_name()].erase(book_id);
        if (books_by_author_[book.get_author()].empty()) {
            books_by_author_.erase(book.get_author());
            authors_.erase(book.get_author());
        }
        if (books_by_genre_[book.get_genre()].empty()) {
            books_by_genre_.erase(book.get_genre());
            genres_.erase(book.get_genre());
        }
        if (books_by_name_[book.get_name()].empty()) {
            books_by_name_.erase(book.get_name());
        }
        
        id_to_book_.erase(book_id);
    }

    void borrow_book(int user_id, int book_id) {
        auto user_it = id_to_user_.find(user_id);
        if (user_it == id_to_user_.end()) {
            throw LibraryOperationException("User ID not found");
        }
        if (id_to_book_.find(book_id) == id_to_book_.end()) {
            throw LibraryOperationException("Book ID not found");
        }
        auto user = user_it->second;
        Book& book = id_to_book_.at(book_id);
        if (!user->can_borrow()) {
            throw LibraryOperationException("User has reached borrow limit");
        }
        if (!book.is_available()) {
            throw LibraryOperationException("Book is not available");
        }
        user->borrow_book(book);
        books_ownership_[book_id] = user_id;
        borrow_history_.emplace_front(user_id, book_id, BorrowOperationType::BORROW);
    }

    // returns penalty for late return, 0 if no penalty
    int return_book(int book_id) {
        if (id_to_book_.find(book_id) == id_to_book_.end()) {
            throw LibraryOperationException("Book ID not found");
        }
        if (books_ownership_.find(book_id) == books_ownership_.end()) {
            throw LibraryOperationException("Book was not borrowed");
        }
        Book& book = id_to_book_.at(book_id);
        int user_id = books_ownership_.at(book_id);
        auto user_it = id_to_user_.find(user_id);
        if (user_it == id_to_user_.end()) {
            throw LibraryOperationException("User not found for borrowed book");
        }
        auto user = user_it->second;
        int days_borrowed = clock_.days_since(book.get_taken_time());
        books_ownership_.erase(book_id);
        book.return_book();
        borrow_history_.emplace_back(user_id, book_id, BorrowOperationType::RETURN);
        
        if (days_borrowed <= user->max_borrowed_days()) {
            return 0;
        }
        int penalty = (days_borrowed - user->max_borrowed_days()) * user->get_penalty_for_one_day();
        user->add_penalty(penalty);
        return penalty;
    }

    std::unordered_set<std::string> get_all_genres() const {
        return genres_;
    }

    std::unordered_set<std::string> get_all_authors() const {
        return authors_;
    }

    std::unordered_map<int, Book> get_all_books() const {
        return id_to_book_;
    }

    std::unordered_map<int, std::shared_ptr<User>> get_all_users() const {
        return id_to_user_;
    }

    std::optional<Book> get_book_by_id(int book_id) {
        if (id_to_book_.find(book_id) == id_to_book_.end()) {
            return std::nullopt;
        }
        return id_to_book_.at(book_id);
    }

    std::shared_ptr<User> get_user_by_id(int user_id) {
        auto it = id_to_user_.find(user_id);
        if (it == id_to_user_.end()) {
            return nullptr;
        }
        return it->second;
    }

    std::vector<Book> get_books_by_name(const std::string& name) {
        if (books_by_name_.find(name) == books_by_name_.end()) {
            return {};
        }
        std::vector<Book> result;
        for (int book_id : books_by_name_.at(name)) {
            result.push_back(id_to_book_.at(book_id));
        }
        return result;
    }

    std::vector<Book> get_books_by_author(const std::string& author) {
        if (books_by_author_.find(author) == books_by_author_.end()) {
            return {};
        }
        std::vector<Book> result;
        for (int book_id : books_by_author_.at(author)) {
            result.push_back(id_to_book_.at(book_id));
        }
        return result;
    }

    std::vector<Book> get_books_by_genre(const std::string& genre) {
        if (books_by_genre_.find(genre) == books_by_genre_.end()) {
            return {};
        }
        std::vector<Book> result;
        for (int book_id : books_by_genre_.at(genre)) {
            result.push_back(id_to_book_.at(book_id));
        }
        return result;
    }

    std::deque<std::tuple<int, int, BorrowOperationType>> get_borrow_history() const {
        return borrow_history_;
    }


    std::set<Book> get_borrowed_books() const {
        std::set<Book> result;
        for (auto [book_id, user_id]: books_ownership_) {
            const Book& book = id_to_book_.at(book_id);
            result.insert(book);
        }
        return result;
    }

    std::set<Book> get_overdue_books() const {
        std::set<Book> result;
        for (const auto& [book_id, user_id] : books_ownership_) {
            const Book& book = id_to_book_.at(book_id);
            const auto& user = id_to_user_.at(user_id);
            int days_borrowed = clock_.days_since(book.get_taken_time());
            if (days_borrowed > user->max_borrowed_days()) {
                result.insert(book);
            }
        }
        return result;
    }
    
    int get_next_book_id() {
        return id_generator_.get_next_id();
    }

    int get_next_user_id() {
        return id_generator_.get_next_id();
    }

private:
    Clock<Duration> clock_;
    IdGenerator id_generator_;
    std::unordered_map<int, std::shared_ptr<User>> id_to_user_;
    std::unordered_map<int, Book> id_to_book_;
    std::unordered_map<int, int> books_ownership_; // book_id -> user_id
    std::unordered_set<std::string> genres_;
    std::unordered_set<std::string> authors_;
    std::unordered_map<std::string, std::unordered_set<int>> books_by_author_;
    std::unordered_map<std::string, std::unordered_set<int>> books_by_genre_;
    std::unordered_map<std::string, std::unordered_set<int>> books_by_name_;
    std::deque<std::tuple<int, int, BorrowOperationType>> borrow_history_; // (user_id, book_id, operation_type)

};
