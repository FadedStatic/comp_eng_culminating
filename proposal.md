# Project
Library Management System \
The Library has lost its software for managing students and books. \
Your team has been hired to overhaul and create a new Library Management System. \
The system should be able to:
1. **Store details about books** (title, author, year, genre, availability, track books).
2. **Store member information** (name, member ID, borrowed books).
3. **Allow borrowing and returning books** while enforcing borrowing limits. (max of three books at once, no already borrowed books)
4. **Search for books based** on title, author, or genre.
5. **Reports** Generate reports on overdue books or popular genres. 
### Additional improvements 
6. Account functionality, register/login & preset admin account
# Planning

## Structures
The first issue whenever designing a system is the class structure. \
Type book - will contain fields:
>    author - string \
    title - string \
    year - uint16 (dont need more than that) \
    genre - string \
    availability - bool \
    owner - uint16 (owners will be assigned a unique ID, probably shared with their pos in the members array) \
    id - uint16 (this is its index in books array) 

Type member - will contain fields:  
>    name - string \
    member id - uint16 (same as owner from book) \
    books - vector<uint16> (id-references to books) \
challenge_code - string (holds hashed password)

Type library - owns book and member.

## Functionality
Each array of things will be file-backed, being saved & loaded on exit. \
it's basically going to be a CLI program \
When program starts, ask user to login / register, with a numbered list. \
When registering, ensure no user with same name (strcmp against known members) and sanitize inputs. \
After that, use custom enc algorithm on password, going to use some silly bitwise ops and string manip 
### Normal users
1) Check borrowing status 
2) Browse catalog (maybe make a nice menu)
3) Search for book (author, title, genre)
3) Allow for borrrowing of books, and return of books.
4) Exit feature

### Admin User
1) Get list of members, and the list of books they took out.
2) Create new books
3) Get reports on popular genres

