# Student Management System v3.0

> A console-based **Student Management System** built in **C++17** using **Object-Oriented Programming**, **STL**, and **File Handling** — designed as a resume-ready project for BCA / BCA Cyber Security students.

![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue)
![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey)
![IDE](https://img.shields.io/badge/IDE-Visual%20Studio%20Code-blue)
![Build](https://img.shields.io/badge/Build-g%2B%2B%20%7C%20MinGW-green)
![License](https://img.shields.io/badge/License-MIT-yellow)

---

## Table of Contents

- [Why This Project?](#why-this-project)
- [Features](#features)
- [Project Structure](#project-structure)
- [Technologies Used](#technologies-used)
- [OOP Concepts Applied](#oop-concepts-applied)
- [How to Compile and Run](#how-to-compile-and-run)
- [Default Admin Credentials](#default-admin-credentials)
- [Data Format](#data-format)
- [Grade Scale](#grade-scale)
- [Analytics Report](#analytics-report)
- [Project Architecture](#project-architecture)
- [How to Demonstrate (Viva Guide)](#how-to-demonstrate-viva-guide)
- [Interview Q&A](#interview-qa)
- [Resume Description](#resume-description)
- [Author](#author)

---

## Why This Project?

Managing student records manually using spreadsheets or paper is error-prone and inefficient.
This project solves that by building a **fully automated, file-persistent, menu-driven system** that:

- Stores student records permanently in a CSV text file
- Allows full CRUD operations (Create, Read, Update, Delete)
- Provides analytics (average, topper, pass/fail %, grade distribution)
- Generates a timestamped analytics report saved as a `.txt` file
- Runs entirely in a terminal — no internet, no database, no external tools required

**Built entirely in C++ — zero Python, zero external libraries.**

---

## Features

| # | Feature | Description |
|---|---|---|
| 1 | **Admin Login** | Credential-based login with 3-attempt lockout |
| 2 | **Add Student** | Input validation, unique ID check, auto grade assignment |
| 3 | **View Students** | Tabular display with Grade and Pass/Fail status |
| 4 | **Search Student** | Lookup by Student ID with full detail card |
| 5 | **Update Student** | Edit any field; press Enter to keep current value |
| 6 | **Delete Student** | Confirmation prompt before permanent deletion |
| 7 | **Sort Students** | Sort by ID / Name / Marks using STL `sort()` with lambdas |
| 8 | **Student Analytics** | Total, avg, highest, lowest, topper, pass%, grade distribution |
| 9 | **Save Data** | Explicit save + auto-saves unsaved changes on exit |
| 10 | **Export Report** | Generates a formatted `.txt` analytics report (pure C++) |

---

## Project Structure

```
StudentManagementSystem/
│
├── src/                        ← Source files (.cpp)
│   ├── main.cpp                ← Entry point, menu loop, clearScreen, pause
│   ├── Student.cpp             ← Student class implementation
│   └── StudentManager.cpp      ← CRUD, file I/O, sort, analytics, login
│
├── include/                    ← Header files (.h)
│   ├── Student.h               ← Student class declaration
│   └── StudentManager.h        ← StudentManager class declaration + constants
│
├── data/                       ← Persistent data files
│   ├── students.txt            ← CSV student records (auto-load/save)
│   └── admin.txt               ← Admin credentials (username + password)
│
├── reports/                    ← Auto-generated analytics reports
│   └── report_YYYYMMDD_HHMMSS.txt  ← Timestamped text report
│
├── .vscode/                    ← VS Code integration
│   ├── tasks.json              ← Ctrl+Shift+B to build
│   └── launch.json             ← Debugger config (F5 to debug)
│
├── build.bat                   ← Windows one-click build & run
├── Makefile                    ← Cross-platform build (make)
├── .gitignore                  ← Excludes executables and generated files
└── README.md                   ← This file
```

---

## Technologies Used

| Technology | Purpose |
|---|---|
| **C++17** | Primary language |
| **OOP** | Encapsulation, Abstraction, Constructors |
| **STL `vector<>`** | In-memory student storage |
| **STL `sort()` + Lambda** | Dynamic sorting by ID, Name, Marks |
| **`fstream` / `ifstream` / `ofstream`** | File I/O for CSV data persistence |
| **`sstream` / `ostringstream`** | CSV serialization and deserialization |
| **`iomanip`** | Formatted table output (setw, fixed, setprecision) |
| **`ctime`** | Timestamped report generation |
| **g++ (MinGW on Windows)** | Compiler |
| **VS Code** | IDE with task and debug integration |

> **No Python. No Pandas. No Matplotlib. No external libraries.**

---

## OOP Concepts Applied

| Concept | Where Applied |
|---|---|
| **Encapsulation** | `Student` has private fields (`id`, `name`, `age`, `course`, `marks`) accessed only via public getters/setters |
| **Abstraction** | `StudentManager` exposes only high-level operations; all file I/O and business logic are hidden in private methods |
| **Constructors** | `Student` has a default constructor and a parameterized constructor |
| **Member Functions** | `calculateGrade()`, `displayStudent()`, `displayRow()`, `serialize()`, `deserialize()` |
| **Separation of Concerns** | Data model (`Student`), controller (`StudentManager`), and entry point (`main`) are all separate files |

---

## How to Compile and Run

### Prerequisites

Install **MinGW** (g++ compiler for Windows):
```
winget install mingw
```
Or download from: https://www.mingw-w64.org/

Verify installation:
```bash
g++ --version
```

---

### Option 1 — Double-click (Easiest)

Just double-click **`build.bat`** in the project folder.
It compiles the project and launches it automatically.

---

### Option 2 — VS Code (Recommended for Viva)

1. Open the `StudentManagementSystem/` folder in VS Code
2. Press **`Ctrl + Shift + B`** → Select **"Build & Run SMS"**
3. The program compiles and runs in the integrated terminal

---

### Option 3 — Terminal (Manual)

```bash
# Windows (PowerShell or CMD)
g++ src/main.cpp src/Student.cpp src/StudentManager.cpp -I include -std=c++17 -o sms.exe
.\sms.exe

# Linux / macOS
g++ src/main.cpp src/Student.cpp src/StudentManager.cpp -I include -std=c++17 -o sms
./sms
```

---

### Option 4 — Makefile

```bash
# Windows (MinGW)
mingw32-make

# Linux / macOS
make

# Clean build
make clean
```

---

## Default Admin Credentials

Stored in `data/admin.txt` (auto-created if missing):

```
Username : admin
Password : admin123
```

To change: edit `data/admin.txt` directly.

---

## Data Format

Students are stored in `data/students.txt` as CSV:

```
101,Rishi Sharma,20,BCA Cyber Security,88.0
102,Rahul Verma,19,BCA,75.5
103,Ankit Patel,21,BSc IT,91.0
```

Format: `id,name,age,course,marks`

- Loaded automatically on startup
- Saved on exit (if changes detected) or via menu option 8

---

## Grade Scale

| Marks | Grade | Status |
|-------|-------|--------|
| 90 – 100 | A | PASS |
| 80 – 89 | B | PASS |
| 70 – 79 | C | PASS |
| 60 – 69 | D | PASS |
| Below 60 | F | FAIL |

---

## Analytics Report

Selecting option **9 → Export Analytics Report** generates a file like:

```
=======================================================
   STUDENT MANAGEMENT SYSTEM  --  Analytics Report
   Generated  : 2026-06-23 20:30:15
   Total Recs : 5
=======================================================

  SUMMARY
  -------------------------------------------------------
  Total Students   : 5
  Average Marks    : 78.40
  Highest Marks    : 91.0
  Lowest Marks     : 55.0
  Topper           : Ankit Patel (91.0)
  Pass Count       : 4 (80.0%)
  Fail Count       : 1 (20.0%)

  GRADE DISTRIBUTION
  -------------------------------------------------------
  A (90-100) [####................] 1 student(s)
  B (80-89)  [########............] 2 student(s)
  C (70-79)  [####................] 1 student(s)
  D (60-69)  [....................] 0 student(s)
  F (<60)    [####................] 1 student(s)

  TOP 5 STUDENTS
  ...
```

Reports are saved to: `reports/report_YYYYMMDD_HHMMSS.txt`

---

## Project Architecture

```
┌─────────────┐     uses      ┌──────────────────┐
│   main.cpp  │ ──────────►  │  StudentManager   │
│  (Entry)    │              │  (Controller)     │
└─────────────┘              │                  │
                             │  - CRUD          │
                             │  - File I/O      │
                             │  - Auth          │
                             │  - Sort          │
                             │  - Analytics     │
                             │  - Report Export │
                             └────────┬─────────┘
                                      │ owns many
                                      ▼
                             ┌──────────────────┐
                             │    Student        │
                             │   (Data Model)   │
                             │                  │
                             │  - id, name      │
                             │  - age, course   │
                             │  - marks         │
                             │  - grade logic   │
                             └──────────────────┘
                                      │ persists to
                                      ▼
                             ┌──────────────────┐
                             │  data/           │
                             │  students.txt    │
                             │  admin.txt       │
                             └──────────────────┘
```

**Flow:**
1. `main()` starts → `adminLogin()` authenticates
2. `loadFromFile()` reads `data/students.txt` into `vector<Student>`
3. Menu loop calls the appropriate `StudentManager` method
4. On exit → `saveToFile()` persists all records back to disk

---

## How to Demonstrate (Viva Guide)

Follow this exact flow during your assessment:

### Step 1 — Open Project
```
Open VS Code → File → Open Folder → StudentManagementSystem
```

### Step 2 — Build & Run
```
Press Ctrl+Shift+B → Select "Build & Run SMS"
```

### Step 3 — Login
```
Username: admin
Password: admin123
```

### Step 4 — Add 2 Students
```
Option 1 → Add Student
  ID: 201, Name: Test Student, Age: 20, Course: BCA, Marks: 85
Option 1 → Add Student
  ID: 202, Name: Demo User, Age: 21, Course: MCA, Marks: 45
```

### Step 5 — View All
```
Option 2 → View Students
→ Show the table with Grade and Pass/Fail columns
```

### Step 6 — Search
```
Option 3 → Search → Enter ID: 201
→ Shows full detail card
```

### Step 7 — Sort by Marks
```
Option 6 → Sort → Option 3 (Marks, highest first)
→ Explain: "Using STL sort() with a lambda comparator"
```

### Step 8 — Analytics
```
Option 7 → Student Analytics
→ Show stats: average, topper, pass%
```

### Step 9 — Export Report
```
Option 9 → Export Report
→ Open reports/ folder → Show the generated .txt file
→ "This is generated entirely in C++ using ctime and fstream — no Python needed"
```

### Step 10 — Save & Exit
```
Option 8 → Save Data
Option 10 → Exit
```

---

## Interview Q&A

### Q1: What is the objective of this project?

> "The objective of this project is to create a fully functional Student Management System using C++ that demonstrates core programming concepts like Object-Oriented Programming, STL data structures, and file handling. It can add, view, search, update, delete, and sort student records, and generate analytics reports — all without any external libraries."

---

### Q2: What technologies did you use?

> "I used C++17 as the primary language. For data storage, I used STL `vector<Student>` as an in-memory container and `fstream` for CSV file persistence. For sorting, I used STL `sort()` with lambda comparators. The analytics report is generated using `ctime` for timestamps and `ofstream` for file writing. The build system uses g++ with a `build.bat` script for Windows and a `Makefile` for cross-platform builds."

---

### Q3: What OOP concepts did you apply?

> "I applied encapsulation by keeping the Student fields (id, name, age, marks) private and accessing them through public getters and setters. I applied abstraction through the StudentManager class, which hides all file I/O and business logic from the main function. I used both default and parameterized constructors in the Student class. I also followed separation of concerns by splitting the code into three layers: the data model (Student), the controller (StudentManager), and the entry point (main.cpp)."

---

### Q4: What challenges did you face?

> "One challenge was handling CSV parsing correctly, especially when student names contain special characters. I solved this using `istringstream` with `getline` and a `try-catch` block to skip malformed lines. Another challenge was replacing the Python-based analytics — I redesigned the report generation in pure C++ using `ctime` for timestamps and `ofstream` to write a formatted text report with ASCII bar charts."

---

### Q5: What are the future improvements?

> "In the future, I would like to:
> 1. Add a SQLite or MySQL database backend instead of flat-file CSV storage
> 2. Build a GUI frontend using Qt or a web interface
> 3. Add password hashing for admin security
> 4. Implement multi-user roles (admin, teacher, student)
> 5. Add attendance tracking as an additional feature"

---

### Q6: What is your individual contribution?

> "I designed the complete architecture from scratch — the class hierarchy, the file format, and the menu system. I implemented all CRUD operations with full input validation, wrote the CSV serialization/deserialization logic, designed the STL-based sorting with lambda functions, and built the C++ analytics report generator. I also structured the project professionally with separate source/include/data folders and VS Code build integration."

---

### Q7: Why no Python?

> "The project is designed to be fully self-contained and runnable on any Windows machine with just g++ installed. Python adds an external dependency that may not be available on all systems. By implementing the analytics report in pure C++, the project is more portable, simpler to deploy, and demonstrates stronger C++ skills."

---

### Q8: How does file handling work?

> "Student records are stored in `data/students.txt` in CSV format. On startup, `loadFromFile()` reads each line using `getline()` and parses it with `istringstream` into a `Student` object, which is pushed into a `vector<Student>`. When saving, `saveToFile()` iterates the vector and writes each record using `serialize()`, which builds a CSV string with `ostringstream`. Auto-save triggers on exit if unsaved changes are detected."

---

## Resume Description

```
Student Management System — C++ | OOP | STL | File Handling

• Developed a multi-file console application with Admin Login, CRUD operations,
  sorting, analytics, and automated report generation for student records.

• Applied OOP principles (encapsulation, abstraction, constructors) with separate
  header and source files following professional C++ project structure.

• Used STL vector<> for in-memory storage and sort() with lambda comparators
  for dynamic multi-criteria sorting (by ID, Name, Marks).

• Implemented CSV file persistence using fstream with auto-load on startup,
  explicit save, and auto-save on exit with full input validation.

• Built a pure C++ analytics report generator using ctime and ofstream,
  producing timestamped .txt reports with ASCII bar charts and ranked student tables.
```

---

## Author

**Rishi** — BCA Cyber Security Student

> Built to demonstrate C++ fundamentals in a structured, interview-ready format for the propViVO Drive assessment.

---

*Last updated: June 2026*
