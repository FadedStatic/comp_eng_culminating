#include <ranges>

#include "library.hpp"

int main() {
    auto librarian = library();
    librarian.init_db();
    std::printf("Welcome to the Library! Are you a new or returning user?\r\n1. New User (Register)\r\n2. Returning User (Login)\r\n");
    const auto choice = collect_input<int>(2);
    int user_id = -1;
    if (choice == 0) return librarian.close_db();
    // this is basically like a tag for a jmp. i can do "goto back" and it will jump back to this point in the control flow
back:
    const auto name = collect_input_str("Please enter your name (0 to exit): ");
    if (name == "0") return librarian.close_db();
    const auto password = collect_input_str("Please enter your password: ");

    if (choice == 1) {
        const auto res = librarian.register_member(name, password);
        if (res == -1)
            goto back;
        user_id=res;
        std::printf("Welcome to the Library! Your member ID is %d\r\n", res+1);
    }

    else if (choice == 2) {
        const auto res = librarian.login(name, password);
        if (res == -1)
            goto back;
        user_id=res;
    }


    // now we determine if the user is admin or not
    if (user_id == -1337) {
    admin_back:
        std::printf("Please choose one of the following actions:\r\n1. Show all Members\r\n2. Add Book\r\n3. Remove Book\r\n4. Book Insights\r\n");
        const auto admin_action = collect_input<int>(4);
        if (admin_action == 0) goto end;

        if (admin_action == 1) {
            for (const auto& member : librarian.members) {
                std::printf("\r\n##############   Member #%d   ############## \r\nName: %s\r\n", member.id+1, member.name.c_str());
                if (member.books.empty())
                    std::printf("The member has no books taken out.\r\n");

                for (const auto& book_id : member.books) {
                    const auto& book = librarian.books[book_id]; // i trust my data validation this much
                    std::printf("%s by %s (Genre: %s) [%d]", book.title.c_str(), book.author.c_str(), book.genre.c_str(), book.id+1);
                }
                std::printf("######################################%s\r\n",
                    std::string(std::to_string(user_id).length(), '#').c_str()); // this is just because of OCD
            }
        }

        else if (admin_action == 2) {
        add_book_back:
            const auto year = collect_input<int>(2026, "Please enter year (0 to return to menu): ");
            if (year == 0) goto admin_back;
            const auto author = collect_input_str("Please enter author: ");
            const auto title = collect_input_str("Please enter title: ");
            const auto genre = collect_input_str("Please enter genre: ");
            const auto res = librarian.add_book(year,author,title, genre );
            if (res == -1) goto add_book_back;

            std::printf("Book successfully created!\r\n");
        }

        else if (admin_action == 3) {
            if (librarian.books.empty()) {
                std::printf("Unfortunately, no books currently exist.\r\n\r\n");
                goto admin_back;
            }
            std::printf("###################    Books    ###################\r\n");
            for (const auto& book : librarian.books) {
                std::printf("[%d] %s - %s (%d) (Genre: %s) [Status: %s]\r\n", book.id+1, book.author.c_str(), book.title.c_str(), book.year, book.genre.c_str(), book.availability ? "AVAILABLE" : "BORROWED");
            }

        retry_selection_1:
            const auto sel = collect_input<int>(librarian.books.size(), "Please enter book number to remove (0 to return to menu): ");
            if (sel == 0) goto admin_back;
            if (librarian.books[sel-1].availability == 0) {
                std::printf("The book cannot be removed, as it is currently being borrowed by %s\r\n", librarian.members[librarian.books[sel-1].owner].name.c_str());
                goto retry_selection_1;
            }
            // remove books entry at sel-1
            librarian.books.erase(librarian.books.begin() + (sel-1));
            int adj_idx = 0;
            for (auto& book : librarian.books)
                book.id = adj_idx++;

            for (auto& member : librarian.members)
                for (auto & book : member.books)
                    if (book >= sel) book--;
        }

        else if (admin_action == 4) {
            std::vector<std::pair<int, book>> popular_books;
            std::vector<std::pair<int,std::string>> popular_authors, popular_genres;

            for (const auto& book : librarian.books) {
                popular_books.emplace_back(book.lease_count,book);
                // check if author is alr there
                if (const auto pos = std::ranges::find_if(popular_authors, [book](const std::pair<int,std::string>& val) -> bool  {
                    return val.second == book.author;
                }); pos == popular_authors.end())
                    popular_authors.emplace_back(book.lease_count, book.author);
                else
                    (pos->first)++;

                if (const auto pos = std::ranges::find_if(popular_genres, [book](const std::pair<int,std::string>& val) -> bool  {
                    return val.second == book.genre;
                }); pos == popular_genres.end())
                    popular_genres.emplace_back(book.lease_count, book.genre);
                else
                    (pos->first)++;
            }

            std::printf("Top books:\r\n");
            for (int i = 0; i < std::min(static_cast<int>(popular_books.size()), 5); i++) {
                const auto& book = popular_books[i].second;
                std::printf("%d) %s - %s (%d leases)\r\n", i+1, book.author.c_str(), book.title.c_str(), book.lease_count);
            }
            std::printf("Top authors:\r\n");
            for (int i = 0; i < std::min(static_cast<int>(popular_authors.size()), 5); i++) {
                const auto& [first,second] = popular_authors[i];
                std::printf("%d) %s (%d leases)\r\n", i+1, second.c_str(), first);
            }
            std::printf("Top genres:\r\n");
            for (int i = 0; i < std::min(static_cast<int>(popular_genres.size()), 5); i++) {
                const auto& [first,second] = popular_genres[i];
                std::printf("%d) %s (%d leases)\r\n", i+1, second.c_str(), first);
            }
        }
        std::printf("\r\n");
        goto admin_back;
    }

    else {
    user_back:
        std::printf("Please choose one of the following actions:\r\n1. Show outstanding books\r\n2. Browse Catalog & Borrow\r\n3. Search for Book\r\n4. Return Book\r\n");
        const auto user_action  = collect_input<int>(4);
        if (user_action == 0) goto end;

        if (user_action == 1) {
            if (librarian.members[user_id].books.empty()) {
                std::printf("You have no outstanding books!\r\n\r\n");
                goto user_back;
            }
            for (const auto& book_id : librarian.members[user_id].books) {
                const auto& book = librarian.books[book_id];
                std::printf("[%d] %s - %s (%d) (Genre: %s)\r\n", book.id+1, book.author.c_str(), book.title.c_str(), book.year, book.genre.c_str());
            }
            std::printf("\r\n\r\n");
        }

        else if (user_action == 2) {
            if (librarian.books.empty()) {
                std::printf("The library does not have any books right now, please contact the mayor.\r\n\r\n");
                goto user_back;
            }
            std::printf("###################    Books    ###################\r\n");
            for (const auto& book : librarian.books) {
                std::printf("[%d] %s - %s (%d) (Genre: %s) [Status: %s]\r\n", book.id+1, book.author.c_str(), book.title.c_str(), book.year, book.genre.c_str(), book.availability ? "AVAILABLE" : "BORROWED");
            }

        retry_selection_2:
            const auto sel = collect_input<int>(librarian.books.size(), "Please enter book number to borrow (0 to return to menu): ");
            if (sel == 0) goto user_back;
            if (librarian.books[sel-1].availability == 0) {
                std::printf("The book you chose is not currently available, please come back at a later date.\r\n");
                goto retry_selection_2;
            }

            if (librarian.members[user_id].books.size() == 3) {
                std::printf("You have borrowed 3 books, please return one before attempting to borrow another one.\r\n\r\n");
                goto user_back;
            }

            librarian.books[sel-1].availability = false;
            librarian.books[sel-1].owner = user_id;
            librarian.books[sel-1].lease_count++;
            librarian.members[user_id].books.push_back(sel-1);

            std::printf("Congratulations, enjoy your book!\r\n\r\n");
        }

        if (user_action == 3) {
        search_start:
            std::printf("Please choose a search method:\r\n1. By title\r\n2. By author\r\n3. By genre\r\n");
            const auto search_choice = collect_input<int>(3, "Please enter search choice (0 to return to menu): ");
            if (search_choice == 0) goto user_back;
            std::vector<std::pair<double, std::uint16_t>> results;
            int decision = 0;
            if (search_choice == 1) {
        title_search:
                results.clear();
                const auto title_prompt = collect_input_str("Enter book title search (0 to return to previous menu): ");
                if (title_prompt == "0") goto search_start;
                int idx=0;
                for (const auto& book : librarian.books)
                    results.emplace_back(weighted_string_score(title_prompt, book.title), idx++);

                std::ranges::sort(results, [](const auto& a, const auto& b) {
                    return a.first > b.first;
                });

                // this is like a super new feature that clion suggested (std::views)
                for (const auto &val: results | std::views::values) {
                    const auto book = librarian.books[val];
                    std::printf("[%d] %s - %s (%d) (Genre: %s) [Status: %s]\r\n", book.id+1, book.author.c_str(), book.title.c_str(), book.year, book.genre.c_str(), book.availability ? "AVAILABLE" : "BORROWED");
                }

            retry_selection_4:
                decision = collect_input<int>(librarian.books.size(), "Please enter book number to borrow (0 to return to previous menu): ");
                if (decision == 0) goto title_search;
                if (librarian.books[decision-1].availability == 0) {
                    std::printf("The book you chose is not currently available, please come back at a later date.\r\n");
                    goto retry_selection_4;
                }
            }

            if (search_choice == 2) {
                author_search:
                results.clear();
                const auto author_prompt = collect_input_str("Enter book author search (0 to return to previous menu): ");
                if (author_prompt == "0") goto search_start;
                int idx=0;
                for (const auto& book : librarian.books)
                    results.emplace_back(weighted_string_score(author_prompt, book.author), idx++);

                std::ranges::sort(results, [](const auto& a, const auto& b) {
                    return a.first > b.first;
                });

                // this is like a super new feature that clion suggested (std::views)
                for (const auto &val: results | std::views::values) {
                    const auto book = librarian.books[val];
                    std::printf("[%d] %s - %s (%d) (Genre: %s) [Status: %s]\r\n", book.id+1, book.author.c_str(), book.title.c_str(), book.year, book.genre.c_str(), book.availability ? "AVAILABLE" : "BORROWED");
                }

                retry_selection_5:
                decision = collect_input<int>(librarian.books.size(), "Please enter book number to borrow (0 to return to previous menu): ");
                if (decision == 0) goto genre_search;
                if (librarian.books[decision-1].availability == 0) {
                    std::printf("The book you chose is not currently available, please come back at a later date.\r\n");
                    goto retry_selection_5;
                }
            }

            if (search_choice == 3) {
                genre_search:
                results.clear();
                const auto author_prompt = collect_input_str("Enter book genre search (0 to return to previous menu): ");
                if (author_prompt == "0") goto search_start;
                int idx=0;
                for (const auto& book : librarian.books)
                    results.emplace_back(weighted_string_score(author_prompt, book.genre), idx++);

                std::ranges::sort(results, [](const auto& a, const auto& b) {
                    return a.first > b.first;
                });

                // this is like a super new feature that clion suggested (std::views)
                for (const auto &val: results | std::views::values) {
                    const auto book = librarian.books[val];
                    std::printf("[%d] %s - %s (%d) (Genre: %s) [Status: %s]\r\n", book.id+1, book.author.c_str(), book.title.c_str(), book.year, book.genre.c_str(), book.availability ? "AVAILABLE" : "BORROWED");
                }

                retry_selection_6:
                    decision = collect_input<int>(librarian.books.size(), "Please enter book number to borrow (0 to return to previous menu): ");
                if (decision == 0) goto genre_search;
                if (librarian.books[decision-1].availability == 0) {
                    std::printf("The book you chose is not currently available, please come back at a later date.\r\n");
                    goto retry_selection_6;
                }
            }

            if (librarian.members[user_id].books.size() == 3) {
                std::printf("You have borrowed 3 books, please return one before attempting to borrow another one.\r\n\r\n");
                goto user_back;
            }

            librarian.books[decision-1].availability = false;
            librarian.books[decision-1].owner = user_id;
            librarian.books[decision-1].lease_count++;
            librarian.members[user_id].books.push_back(decision-1);

            std::printf("Congratulations, enjoy your book!\r\n\r\n");
        }

        if (user_action == 4) {
            if (librarian.members[user_id].books.empty()) {
                std::printf("You have no outstanding books!\r\n\r\n");
                goto user_back;
            }
            for (const auto& book_id : librarian.members[user_id].books) {
                const auto& book = librarian.books[book_id];
                std::printf("[%d] %s - %s (%d) (Genre: %s)\r\n", book.id+1, book.author.c_str(), book.title.c_str(), book.year, book.genre.c_str());
            }
        retry_selection_3:
            const auto sel = collect_input<int>(librarian.books.size(), "Please enter book number to return (0 to return to menu): ");
            if (sel == 0) goto user_back;
            bool matched = false;
            for (const auto& book_id : librarian.members[user_id].books)
                if (book_id == sel-1) matched=true;

            if (!matched) {
                std::printf("You do not have possession of this book, nor do you owe us it. Please select a book from the list.\r\n");
                goto retry_selection_3;
            }

            librarian.books[sel-1].availability = true;
            librarian.members[user_id].books.erase(
                std::ranges::remove_if(librarian.members[user_id].books,
                                       [&](const auto& book_id) { // lambda here
                                           return book_id == sel-1;
                                       }).begin());
            librarian.books[sel-1].owner = -1;

            std::printf("Thank you for returning your book!\r\n\r\n");
        }
        goto user_back;
    }

end:
    return librarian.close_db();
}