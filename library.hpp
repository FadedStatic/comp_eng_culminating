#pragma once

#include <utility>
#include <filesystem>
#include <fstream>
#include "util.hpp"


struct book {
    bool availability;
    std::uint16_t id;
    int owner;

    std::uint16_t year,
        lease_count;

    // you can use commas to perform multiple initializations
    std::string author,
            title,
            genre;
};

struct member {
    std::string name;
    unsigned challenge_code;
    std::uint16_t id;
    std::vector<std::uint16_t> books;

    member(std::string name_, const unsigned challenge, const std::uint16_t id_, std::vector<std::uint16_t> books_) : name(std::move(name_)), challenge_code(challenge), id(id_), books(std::move(books_)) {}
    member()=default;
};

class library {
public:
    // bottom two vectors are replicated to disk (saved and loaded on exit and entry)
    std::vector<book> books; // this is the real array of books, no point in copying it to each member
    std::vector<member> members;

    int add_book(std::uint16_t year, std::string_view author, std::string_view title, std::string_view genre);
    int register_member(std::string_view name, std::string_view password);
    int login(std::string_view name, std::string_view password) const;
    bool close_db() const;
    bool init_db();
};

