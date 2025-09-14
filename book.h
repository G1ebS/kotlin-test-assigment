#pragma once
#include <chrono>
#include <string>
#include <ctime>


template<class T> 
struct is_duration : std::false_type {};

template<class Rep, class Period> 
struct is_duration<std::chrono::duration<Rep, Period>> : std::true_type {};


template <typename Duration>
class Clock {
    static_assert(is_duration<Duration>::value, "Duration must be a std::chrono::duration");
public:
    explicit Clock(Duration day_duration): day_duration_(std::chrono::duration_cast<std::chrono::seconds>(day_duration)) {};

    std::chrono::seconds day_length() const { return day_duration_; }


    int days_since(std::chrono::system_clock::time_point start) const {
        auto now = std::chrono::system_clock::now();
        auto diff = now - start;
        return std::chrono::duration_cast<std::chrono::seconds>(diff).count()
            / day_duration_.count();
    }

private:
    std::chrono::seconds day_duration_;
};

class Book {
public:
    Book(std::string name, std::string author, std::string genre, int id)
        : name_(std::move(name)), author_(std::move(author)), genre_(std::move(genre)), id_(id) {}

    bool is_available() const { return available_; }

    void return_book() {
        available_ = true;
        taken_time_ = {};
    }

    void take() {
        available_ = false;
        taken_time_ = std::chrono::system_clock::now();
    }

    std::chrono::system_clock::time_point get_taken_time() const {
        return taken_time_;
    }



    std::string get_genre() const { return this->genre_; }

    std::string get_author() const { return this->author_; }

    std::string get_name() const { return this->name_; }

    int get_id() const { return this->id_; }

    bool operator<(const Book& other) const {
        if (this->taken_time_ != other.taken_time_) {
            return this->taken_time_ < other.taken_time_;
        }
        return this->id_ < other.id_;
    }
    bool operator==(const Book& other) const {
        return this->id_ == other.id_;
    }
    bool operator!=(const Book& other) const {
        return !(*this == other);
    }
    bool operator>(const Book& other) const {
        if (this->taken_time_ != other.taken_time_) {
            return this->taken_time_ > other.taken_time_;
        }
        return this->id_ > other.id_;
    }


    ~Book() = default;

private:
    std::string name_, author_, genre_;
    int id_;
    bool available_ = true;
    std::chrono::system_clock::time_point taken_time_{};


};