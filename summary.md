# Description
The project is a command-line **library management system** written in C++. \
It features both admin and member interfaces for things like login & register, adding and removing books, borrowing and returning, searching w/ fuzzy matching, and generating reports for which books, genres and artists are popular. \
All data is stored persistently in text files (member.txt, books.txt) and loaded and modified during runtime.
# Technologies & Resources used
- Language and build tools: Modern C++ is used, with find_if and sort being utilized wherever applicable. 
- Hashing implementation for passwords, using rshash from https://github.com/richardwtf/rshash
  - I would have written my own, but it wouldn't have been guaranteed to generate unique enough outputs.
- Fuzzy search for titles, authors, and genres
  - It uses the levenshtein string similarity algorithm which gives a score for how many transformations in a matrix are required to turn one string into the compared string. 
  - It's as accurate as the project needs it to be, I would have tokenized the entire string and made some sort of indexing function, but it would shadow the entire project in complexity.
  - Reference was from https://github.com/guilhermeagostinelli/levenshtein/blob/master/levenshtein.cpp
- File I/O - String manipulation was used to serialize/deserialize structs to the intermediate representation stored on the disk
- Goto (labelled jumps) were used to make the menu loops cleaner.
- Sorting/search also using std::ranges and std::views for sorting.
# Successses & Challenges
### Successes
- The fuzzy search algorithm was a great success, it sorts results quite well provided its low complexity
- I'm proud of how well the ownership of books works, and how well put-together the menus are.
### Challenges
- Finding a good algorithm for searching was hard because I went too far into search indexing when a less complex solution was in front of me.
- There were a lot of issues related to saving files, sometimes there would be edge cases 
- Fixing an out of bounds error caused by members' book indexes not being updated when a book is removed, allowing for dangerous refs.
  - Originally I accounted for books being borrowed, but I found the other unintended consequence.
# What did I learn?
The biggest learning experience I had with this project was the search algorithm, some of the solutions are incredibly complex yet work very well. \
Prior to starting I had never heard of the levenshtein algorithm, and I tried implementing it on my own but I kept encountering issues.\
I also tried to implement my own hashing algorithm, but I did not trust that the outputs were unique enough.