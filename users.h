#pragma once
#include <string>
#include <vector>
#include "book.h"
#include <unordered_map>
#include <stdexcept>

// enum MAX_BORROW_BOOK {
//     STUDENT = 5,
//     FACULTY = 10,
//     GUEST = 2
// };


enum class UserType {
    STUDENT,
    FACULTY,
    GUEST
};

const std::unordered_map<UserType, int> user_type_to_borrow_limit = {
    {UserType::STUDENT, 5},
    {UserType::FACULTY, 10},
    {UserType::GUEST, 2}
};


const std::unordered_map<UserType, int> user_type_to_max_borrowed_days = {
    {UserType::STUDENT, 3},
    {UserType::FACULTY, 10},
    {UserType::GUEST, 1}
};

const std::unordered_map<UserType, int> user_type_to_fine = {
    {UserType::STUDENT, 10},
    {UserType::FACULTY, 5},
    {UserType::GUEST, 20}
};

class User {
public:
    User(std::string name, std::string email, int id): name_(name), email_(email), id_(id), borrowed_books_() {};



    int max_borrow_limit() const {
        return user_type_to_borrow_limit.at(get_user_type());
    };

    int max_borrowed_days() const {
        return user_type_to_max_borrowed_days.at(get_user_type());
    };

    int get_penalty_for_one_day() const {
        return user_type_to_fine.at(get_user_type());
    }

    int get_penalty_value() const {
        return penalty_;
    }

    void add_penalty(int amount) {
        if (amount < 0) {
            throw std::invalid_argument("Penalty amount cannot be negative");
        }
        penalty_ += amount;
    }

    std::vector<Book> get_borrowed_books() const {
        return borrowed_books_;
    }

    std::string get_name() const { return this->name_; }

    std::string get_email() const { return this->email_; }

    int get_id() const { return this->id_; }

    bool can_borrow() {
        return borrowed_books_.size() < max_borrow_limit();
    };

    void borrow_book(Book& book) {
        if (!can_borrow()) {throw std::logic_error("Borrow limit exceeded");}
        if (!book.is_available()) {throw std::logic_error("Book is not available");}
        book.take();
        borrowed_books_.push_back(book);
    };




    virtual UserType get_user_type() const = 0;

    virtual ~User() = default;

private:
    std::string name_, email_;
    int id_, penalty_ = 0;
    std::vector<Book> borrowed_books_;
};


class Student: public User {
public:
    using User::User;
    UserType get_user_type() const override{
        return UserType::STUDENT;
    }
    
};



class Faculty: public User {
public:
    using User::User;

    UserType get_user_type() const override {
        return UserType::FACULTY;
    }
    
};


class Guest: public User {
public:
    using User::User;

    UserType get_user_type() const override {
        return UserType::GUEST;
    }
};