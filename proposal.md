# Project
Library Management System
The Library has lost its software for managing students and books. Your team has been hired to overhaul and create a new Library Management System. The system should be able to:
1. **Store details about books** (title, author, year, genre, availability, track books).
2. **Store member information** (name, member ID, borrowed books).
3. **Allow borrowing and returning books** while enforcing borrowing limits.
4. **Search for books based** on title, author, or genre.
5. **Reports** Generate reports on overdue books or popular genres.

# Planning
The first issue whenever designing a system is the class structure.
Type book - will contain fields:
    author - string
    title - string
    year - uint16 (dont need more than that)
    genre - string
    availability - bool
    owner - uint16 (owners will be assigned a unique ID, probably shared with their pos in the members array)

Type member - will contain fields:
    name - string
    member id - 
Type store - owns book and member.