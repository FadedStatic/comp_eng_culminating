#include "library.hpp"

int library::register_member(const std::string_view name, std::string_view password) {
    // no attempting to break the database.
    if (name.contains(';') || name.contains(","))
        return std::printf("Error: Name cannot contain blacklisted characters (; or ,)\r\n"), -1;

    if (password.length() < 6)
        return std::printf("Error: Password must have at least six characters.\r\n"), -1;

    if (!_strcmpi(name.data(), "admin"))
        return std::printf("Error: A member already exists with this name.\r\n"), -1;

    for (const auto& member : members)
        if (!_stricmp(name.data(), member.name.data()))
            return std::printf("Error: A member already exists with this name.\r\n"), -1;


    // rshash will hash the string...
    const auto chall_code = rshash(std::string(password));
    members.push_back({std::string(name), chall_code, static_cast<std::uint16_t>(members.size()), {}});

    return members.size()-1;
}

int library::add_book(std::uint16_t year, const std::string_view author, const std::string_view title, const std::string_view genre) {
    if (author.contains(";") || title.contains(";") || genre.contains(";"))
        return std::printf("Error: One of the inputs failed sanitation. No using semicolons.\r\n"), -1;

    books.push_back({true, static_cast<std::uint16_t>(books.size()), -1, year, 0, author.data(), title.data(), genre.data() });

    return books.size()-1;
}

int library::login(const std::string_view name, const std::string_view password) const {
    const auto chall_code = rshash(std::string(password));
    for (const auto& member : members) {
        if (!_stricmp(name.data(), member.name.data()) && chall_code == member.challenge_code)
            return std::printf("Welcome to the library, %s!\r\n", name.data()), member.id;
    }

    if (!_stricmp(name.data(), "admin") &&
        !_stricmp(password.data(), "admin"))
        return std::printf("Welcome back, admin!\r\n"), -1337;

    return std::printf("Error: Failed to login, invalid credentials\r\n"), -1;
}

// splits a line by the delimiter
std::vector<std::string> split(const std::string& s, const char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(s);

    while (std::getline(token_stream, token, delimiter))
        tokens.push_back(token);

    return tokens;
}

std::vector<std::uint16_t> parse_split_ints(const std::string& s, const char delimiter) {
    std::vector<std::uint16_t> result;
    const auto split_string = split(s, delimiter);
    for (const auto& token : split_string) {
        result.emplace_back(std::stoi(token));
    }
    return result;
}

member parse_member(const std::string_view str)
{
    member ret;
    const auto ir = split(str.data(), ';');
    ret.id = std::stoi(ir.at(0));
    ret.name=ir.at(1);
    ret.challenge_code=std::stoul(ir.at(2));
    ret.books=parse_split_ints(ir.at(3), ',');
    return ret;
}

book parse_book(const std::string_view str) {
    book ret;
    const auto ir = split(str.data(), ';');
    ret.availability=std::stoi(ir.at(0));
    ret.id=std::stoi(ir.at(1));
    ret.owner=std::stoi(ir.at(2));
    ret.year=std::stoi(ir.at(3));
    ret.lease_count=std::stoi(ir.at(4));
    ret.author=ir.at(5);
    ret.title=ir.at(6);
    ret.genre=ir.at(7);
    return ret;
}

std::string fmt_member(const member& member) {
    std::string ret;
    ret+=std::to_string(member.id) + ";";
    ret+=member.name + ";";
    ret+=std::to_string(member.challenge_code) + ";";
    for (const auto& book : member.books)
        ret+=(std::to_string(book) + ",");
    if (member.books.size() > 0)
        ret.pop_back();
    ret.append(";\n");
    return ret;
}

std::string fmt_book(const book& book) {
    std::string ret;
    ret+=std::to_string(book.availability) + ';';
    ret+=std::to_string(book.id) + ";";
    ret+=std::to_string(book.owner) + ';';
    ret+=std::to_string(book.year) + ';';
    ret+=std::to_string(book.lease_count) + ';';
    ret+=book.author + ';';
    ret+=book.title + ';';
    ret+=book.genre + ';';
    ret.append("\r\n");
    return ret;

}
bool library::init_db() {
    if (!std::filesystem::exists("members.txt"))
        std::fstream("members.txt", std::ios::out).close();

    if (!std::filesystem::exists("books.txt"))
        std::fstream("books.txt", std::ios::out).close();

    auto member_fs = std::fstream("members.txt", std::ios::in | std::ios::out | std::ios::binary);
    auto book_fs = std::fstream("books.txt", std::ios::in | std::ios::out | std::ios::binary);
    std::string tmp_str;

    if (member_fs.peek() == std::ifstream::traits_type::eof())
        goto book_jmp;

    while (std::getline(member_fs, tmp_str))
        this->members.push_back(parse_member(tmp_str));

    book_jmp:
    if (book_fs.peek() == std::ifstream::traits_type::eof())
        return false;

    while (std::getline(book_fs, tmp_str))
        this->books.push_back(parse_book(tmp_str));
    return true;
}

bool library::close_db() const {
    if (!std::filesystem::exists("members.txt"))
        std::fstream("members.txt", std::ios::out).close();

    if (!std::filesystem::exists("books.txt"))
        std::fstream("books.txt", std::ios::out).close();



    auto member_fs = std::fstream("members.txt", std::ios::out | std::ios::binary | std::ios::trunc);
    auto book_fs = std::fstream("books.txt", std::ios::out | std::ios::binary | std::ios::trunc);

    for (const auto& member : this->members) {
        const auto member_fmt = fmt_member(member);
        member_fs.write(member_fmt.data(), member_fmt.size());
    }

    for (const auto& book : this->books) {
        const auto book_fmt = fmt_book(book);
        book_fs.write(book_fmt.data(), book_fmt.size());
    }

    member_fs.close();
    book_fs.close();
    std::printf("Thanks for coming to the library! Goodbye\r\n");
    return true;
}
