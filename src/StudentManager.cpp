/*
 * ============================================================
 *  StudentManager.cpp
 *  Implements StudentManager — CRUD, file I/O, sort,
 *  analytics, admin login, and C++ report export.
 *
 *  NOTE: All Python dependencies have been removed.
 *  Analytics report is now generated in pure C++ and saved
 *  as a formatted .txt file inside the reports/ folder.
 * ============================================================
 */

#include "../include/StudentManager.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <climits>
#include <ctime>

using namespace std;

// ═════════════════════════════════════════════
//  Constructor
// ═════════════════════════════════════════════

StudentManager::StudentManager()
    : dataModified(false) {}

// ═════════════════════════════════════════════
//  Private helpers — input utilities
// ═════════════════════════════════════════════

// Read a validated integer in [lo, hi]
int StudentManager::readInt(const string& prompt, int lo, int hi) const
{
    int val;
    while (true)
    {
        cout << prompt;
        if (cin >> val && val >= lo && val <= hi)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Please enter a number between " << lo << " and " << hi << ".\n";
    }
}

// Read a validated float in [lo, hi]
float StudentManager::readFloat(const string& prompt, float lo, float hi) const
{
    float val;
    while (true)
    {
        cout << prompt;
        if (cin >> val && val >= lo && val <= hi)
        {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "  [!] Please enter a value between " << lo << " and " << hi << ".\n";
    }
}

// Read a non-empty trimmed string
string StudentManager::readString(const string& prompt) const
{
    string val;
    while (true)
    {
        cout << prompt;
        getline(cin, val);

        // trim leading and trailing whitespace
        size_t s = val.find_first_not_of(" \t\r\n");
        size_t e = val.find_last_not_of (" \t\r\n");
        if (s != string::npos)
        {
            val = val.substr(s, e - s + 1);
            return val;
        }
        cout << "  [!] This field cannot be empty. Try again.\n";
    }
}

// Yes/No confirmation
bool StudentManager::confirm(const string& prompt) const
{
    cout << prompt;
    char c;
    cin >> c;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return (c == 'Y' || c == 'y');
}

// ═════════════════════════════════════════════
//  Private helpers — data utilities
// ═════════════════════════════════════════════

bool StudentManager::idExists(int id) const
{
    for (const auto& s : students)
        if (s.getId() == id) return true;
    return false;
}

int StudentManager::findIndexById(int id) const
{
    for (int i = 0; i < (int)students.size(); ++i)
        if (students[i].getId() == id) return i;
    return -1;
}

void StudentManager::printTableHeader() const
{
    string line(76, '=');
    cout << "\n" << line << "\n";
    cout << "| "
         << left  << setw(6)  << "ID"
         << "| "  << setw(22) << "Name"
         << "| "  << setw(5)  << "Age"
         << "| "  << setw(14) << "Course"
         << "| "  << right << setw(7) << "Marks"
         << " | " << setw(2)  << "Gr"
         << " | " << setw(5)  << "Pass?"
         << " |\n";
    cout << line << "\n";
}

void StudentManager::printTableFooter() const
{
    cout << string(76, '=') << "\n";
}

// ═════════════════════════════════════════════
//  Admin File helpers
// ═════════════════════════════════════════════

// Creates data/admin.txt with default credentials if missing
void StudentManager::createDefaultAdminFile() const
{
    ofstream f(ADMIN_FILE);
    if (f.is_open())
    {
        f << "admin\n";
        f << "admin123\n";
        f.close();
    }
}

// ═════════════════════════════════════════════
//  Admin Login
// ═════════════════════════════════════════════

bool StudentManager::adminLogin()
{
    // Ensure the credentials file exists
    {
        ifstream check(ADMIN_FILE);
        if (!check.is_open()) createDefaultAdminFile();
    }

    // Read stored credentials
    string storedUser, storedPass;
    {
        ifstream f(ADMIN_FILE);
        if (!f.is_open())
        {
            cerr << "\n  [ERROR] Cannot read " << ADMIN_FILE << ". Using defaults.\n";
            storedUser = "admin";
            storedPass = "admin123";
        }
        else
        {
            getline(f, storedUser);
            getline(f, storedPass);
            f.close();
        }
    }

    // Trim CR if file has Windows line endings
    if (!storedUser.empty() && storedUser.back() == '\r') storedUser.pop_back();
    if (!storedPass.empty() && storedPass.back() == '\r') storedPass.pop_back();

    cout << "\n";
    cout << string(40, '=') << "\n";
    cout << "    STUDENT MANAGEMENT SYSTEM\n";
    cout << "         Admin Login\n";
    cout << string(40, '=') << "\n\n";

    for (int attempt = 1; attempt <= MAX_ATTEMPTS; ++attempt)
    {
        cout << "  Attempt " << attempt << " of " << MAX_ATTEMPTS << "\n";

        cout << "  Username : ";
        string user;
        getline(cin, user);
        if (!user.empty() && user.back() == '\r') user.pop_back();

        cout << "  Password : ";
        string pass;
        getline(cin, pass);
        if (!pass.empty() && pass.back() == '\r') pass.pop_back();

        if (user == storedUser && pass == storedPass)
        {
            cout << "\n  [OK] Login successful! Welcome, " << user << ".\n\n";
            return true;
        }

        int remaining = MAX_ATTEMPTS - attempt;
        if (remaining > 0)
            cout << "\n  [X] Invalid credentials. " << remaining << " attempt(s) remaining.\n\n";
        else
            cout << "\n  [X] Invalid credentials.\n";
    }

    cout << "\n  [!] Too many failed attempts. Exiting.\n\n";
    return false;
}

// ═════════════════════════════════════════════
//  File I/O
// ═════════════════════════════════════════════

void StudentManager::loadFromFile()
{
    ifstream f(STUDENTS_FILE);
    if (!f.is_open())
    {
        cout << "  [i] No existing data file found. Starting fresh.\n";
        return;
    }

    string line;
    int    loaded = 0, skipped = 0;

    while (getline(f, line))
    {
        if (line.empty()) continue;
        Student s;
        if (s.deserialize(line))
        {
            students.push_back(s);
            ++loaded;
        }
        else ++skipped;
    }
    f.close();

    cout << "  [OK] Loaded " << loaded << " record(s) from " << STUDENTS_FILE;
    if (skipped) cout << "  (" << skipped << " malformed line(s) skipped)";
    cout << ".\n";

    dataModified = false;
}

void StudentManager::saveToFile() const
{
    ofstream f(STUDENTS_FILE, ios::trunc);
    if (!f.is_open())
    {
        cerr << "\n  [ERROR] Cannot write to " << STUDENTS_FILE << ".\n";
        return;
    }
    for (const auto& s : students)
        f << s.serialize() << "\n";
    f.close();
    cout << "  [OK] " << students.size() << " record(s) saved to " << STUDENTS_FILE << ".\n";
}

// ═════════════════════════════════════════════
//  1. Add Student
// ═════════════════════════════════════════════

void StudentManager::addStudent()
{
    cout << "\n" << string(52, '=') << "\n";
    cout << "  Add New Student\n";
    cout << string(52, '=') << "\n\n";

    // ─ ID (must be unique) ─
    int id;
    while (true)
    {
        id = readInt("  Student ID   : ", 1, 99999);
        if (!idExists(id)) break;
        cout << "  [!] ID " << id << " already exists. Choose a different ID.\n";
    }

    string name   = readString("  Name          : ");
    int    age    = readInt   ("  Age           : ", 1, 120);
    string course = readString("  Course        : ");
    float  marks  = readFloat ("  Marks (0-100) : ", 0.0f, 100.0f);

    Student s(id, name, age, course, marks);
    students.push_back(s);
    dataModified = true;

    cout << "\n  [OK] Student added!  Grade : " << s.calculateGrade()
         << "   Status : " << (marks >= 60 ? "PASS" : "FAIL") << "\n";
}

// ═════════════════════════════════════════════
//  2. View All Students
// ═════════════════════════════════════════════

void StudentManager::viewStudents() const
{
    cout << "\n" << string(52, '=') << "\n";
    cout << "  All Student Records\n";
    cout << string(52, '=') << "\n";

    if (students.empty())
    {
        cout << "\n  No records found. Add students first.\n\n";
        return;
    }

    printTableHeader();
    for (const auto& s : students)
        s.displayRow();
    printTableFooter();

    cout << "  Total records : " << students.size() << "\n";
}

// ═════════════════════════════════════════════
//  3. Search Student
// ═════════════════════════════════════════════

void StudentManager::searchStudent() const
{
    cout << "\n" << string(52, '=') << "\n";
    cout << "  Search Student by ID\n";
    cout << string(52, '=') << "\n\n";

    if (students.empty())
    {
        cout << "  No records to search.\n\n";
        return;
    }

    int id = readInt("  Enter Student ID : ", 1, 99999);
    int idx = findIndexById(id);

    if (idx == -1)
        cout << "\n  [X] No student found with ID " << id << ".\n";
    else
    {
        cout << "\n  [OK] Student found:\n";
        students[idx].displayStudent();
    }
}

// ═════════════════════════════════════════════
//  4. Update Student
// ═════════════════════════════════════════════

void StudentManager::updateStudent()
{
    cout << "\n" << string(52, '=') << "\n";
    cout << "  Update Student Record\n";
    cout << string(52, '=') << "\n\n";

    if (students.empty())
    {
        cout << "  No records to update.\n\n";
        return;
    }

    int id = readInt("  Enter Student ID to update : ", 1, 99999);
    int idx = findIndexById(id);

    if (idx == -1)
    {
        cout << "\n  [X] No student found with ID " << id << ".\n\n";
        return;
    }

    cout << "\n  Current record:\n";
    students[idx].displayStudent();

    if (!confirm("\n  Proceed with update? (Y/N) : "))
    {
        cout << "  Update cancelled.\n\n";
        return;
    }

    cout << "\n  Enter new values (press Enter to keep the current value):\n\n";

    // ─ Name ─
    cout << "  Name [" << students[idx].getName() << "] : ";
    string newName;
    getline(cin, newName);
    if (!newName.empty()) students[idx].setName(newName);

    // ─ Age ─
    cout << "  Age  [" << students[idx].getAge() << "] : ";
    string ageStr;
    getline(cin, ageStr);
    if (!ageStr.empty())
    {
        try
        {
            int a = stoi(ageStr);
            if (a > 0 && a <= 120) students[idx].setAge(a);
            else cout << "  [!] Age out of range — keeping original.\n";
        }
        catch (...) { cout << "  [!] Invalid age — keeping original.\n"; }
    }

    // ─ Course ─
    cout << "  Course [" << students[idx].getCourse() << "] : ";
    string newCourse;
    getline(cin, newCourse);
    if (!newCourse.empty()) students[idx].setCourse(newCourse);

    // ─ Marks ─
    cout << "  Marks [" << students[idx].getMarks() << "] : ";
    string marksStr;
    getline(cin, marksStr);
    if (!marksStr.empty())
    {
        try
        {
            float m = stof(marksStr);
            if (m >= 0.0f && m <= 100.0f) students[idx].setMarks(m);
            else cout << "  [!] Marks out of range — keeping original.\n";
        }
        catch (...) { cout << "  [!] Invalid marks — keeping original.\n"; }
    }

    dataModified = true;
    cout << "\n  [OK] Record updated. New grade: " << students[idx].calculateGrade() << "\n\n";
}

// ═════════════════════════════════════════════
//  5. Delete Student
// ═════════════════════════════════════════════

void StudentManager::deleteStudent()
{
    cout << "\n" << string(52, '=') << "\n";
    cout << "  Delete Student Record\n";
    cout << string(52, '=') << "\n\n";

    if (students.empty())
    {
        cout << "  No records to delete.\n\n";
        return;
    }

    int id = readInt("  Enter Student ID to delete : ", 1, 99999);
    int idx = findIndexById(id);

    if (idx == -1)
    {
        cout << "\n  [X] No student found with ID " << id << ".\n\n";
        return;
    }

    cout << "\n  Record to be deleted:\n";
    students[idx].displayStudent();

    if (!confirm("\n  Are you sure? (Y/N) : "))
    {
        cout << "  Deletion cancelled.\n\n";
        return;
    }

    students.erase(students.begin() + idx);
    dataModified = true;
    cout << "\n  [OK] Student ID " << id << " has been deleted.\n\n";
}

// ═════════════════════════════════════════════
//  6. Sort Menu
// ═════════════════════════════════════════════

void StudentManager::sortMenu()
{
    if (students.empty())
    {
        cout << "\n  No records to sort.\n\n";
        return;
    }

    cout << "\n" << string(52, '-') << "\n";
    cout << "  Sort Options\n";
    cout << string(52, '-') << "\n";
    cout << "    1. Sort by ID\n";
    cout << "    2. Sort by Name\n";
    cout << "    3. Sort by Marks (highest first)\n";
    cout << string(52, '-') << "\n";

    int choice = readInt("  Enter choice (1-3) : ", 1, 3);

    switch (choice)
    {
        case 1: sortById();    break;
        case 2: sortByName();  break;
        case 3: sortByMarks(); break;
    }
}

void StudentManager::sortById()
{
    sort(students.begin(), students.end(),
         [](const Student& a, const Student& b)
         { return a.getId() < b.getId(); });

    cout << "\n  [OK] Sorted by ID (ascending).\n";
    viewStudents();
    dataModified = true;
}

void StudentManager::sortByName()
{
    sort(students.begin(), students.end(),
         [](const Student& a, const Student& b)
         { return a.getName() < b.getName(); });

    cout << "\n  [OK] Sorted by Name (A to Z).\n";
    viewStudents();
    dataModified = true;
}

void StudentManager::sortByMarks()
{
    sort(students.begin(), students.end(),
         [](const Student& a, const Student& b)
         { return a.getMarks() > b.getMarks(); });   // descending

    cout << "\n  [OK] Sorted by Marks (highest first).\n";
    viewStudents();
    dataModified = true;
}

// ═════════════════════════════════════════════
//  7. Student Analytics (console display)
// ═════════════════════════════════════════════

void StudentManager::showAnalytics() const
{
    cout << "\n" << string(52, '=') << "\n";
    cout << "  Student Analytics\n";
    cout << string(52, '=') << "\n";

    if (students.empty())
    {
        cout << "\n  No data available for analytics.\n\n";
        return;
    }

    int   total     = (int)students.size();
    float sum       = 0.0f;
    float highest   = students[0].getMarks();
    float lowest    = students[0].getMarks();
    int   passCount = 0;
    int   failCount = 0;
    string topperName;
    float  topperMarks = -1.0f;

    for (const auto& s : students)
    {
        float m = s.getMarks();
        sum += m;

        if (m > highest) { highest = m; }
        if (m < lowest ) { lowest  = m; }

        if (m >= 60) ++passCount;
        else         ++failCount;

        if (m > topperMarks)
        {
            topperMarks = m;
            topperName  = s.getName();
        }
    }

    float avg = sum / total;

    // ─ Grade distribution ─
    int gradeA = 0, gradeB = 0, gradeC = 0, gradeD = 0, gradeF = 0;
    for (const auto& s : students)
    {
        string g = s.calculateGrade();
        if      (g == "A") ++gradeA;
        else if (g == "B") ++gradeB;
        else if (g == "C") ++gradeC;
        else if (g == "D") ++gradeD;
        else               ++gradeF;
    }

    string sep(52, '-');
    cout << "\n";
    cout << "  " << sep << "\n";
    cout << "  Total Students   : " << total                               << "\n";
    cout << "  Average Marks    : " << fixed << setprecision(2) << avg     << "\n";
    cout << "  Highest Marks    : " << highest                             << "\n";
    cout << "  Lowest Marks     : " << lowest                              << "\n";
    cout << "  Topper           : " << topperName << "  (" << topperMarks << ")\n";
    cout << "  " << sep << "\n";
    cout << "  Pass Count       : " << passCount << "\n";
    cout << "  Fail Count       : " << failCount << "\n";
    cout << "  Pass Percentage  : "
         << fixed << setprecision(1)
         << (100.0f * passCount / total) << "%\n";
    cout << "  " << sep << "\n";
    cout << "  Grade A (90-100) : " << gradeA << " student(s)\n";
    cout << "  Grade B (80-89)  : " << gradeB << " student(s)\n";
    cout << "  Grade C (70-79)  : " << gradeC << " student(s)\n";
    cout << "  Grade D (60-69)  : " << gradeD << " student(s)\n";
    cout << "  Grade F (<60)    : " << gradeF << " student(s)\n";
    cout << "  " << sep << "\n\n";
}

// ═════════════════════════════════════════════
//  8. Save Data (explicit menu option)
// ═════════════════════════════════════════════

void StudentManager::saveData()
{
    cout << "\n";
    saveToFile();
    dataModified = false;
    cout << "\n";
}

// ─────────────────────────────────────────────
bool StudentManager::hasUnsavedChanges() const
{
    return dataModified;
}

// ═════════════════════════════════════════════
//  9. Export Analytics Report (pure C++)
//
//  Generates a timestamped formatted text report
//  with summary stats, ASCII bar chart, top 5
//  students, and full student list.
//  Saves to: reports/report_YYYYMMDD_HHMMSS.txt
//
//  NO Python. NO external libraries. 100% C++17.
// ═════════════════════════════════════════════

void StudentManager::exportReport() const
{
    cout << "\n" << string(52, '=') << "\n";
    cout << "  Export Analytics Report\n";
    cout << string(52, '=') << "\n\n";

    if (students.empty())
    {
        cout << "  [!] No student data to report. Add students first.\n\n";
        return;
    }

    // ── Step 1: Create reports/ directory ──────────────────
#ifdef _WIN32
    system("if not exist reports mkdir reports");
#else
    system("mkdir -p reports");
#endif

    // ── Step 2: Build timestamped filename ─────────────────
    time_t now = time(nullptr);
    char   timeBuf[32];
    strftime(timeBuf, sizeof(timeBuf), "%Y%m%d_%H%M%S", localtime(&now));
    string filename = REPORTS_DIR + "/report_" + timeBuf + ".txt";

    ofstream f(filename);
    if (!f.is_open())
    {
        cerr << "  [ERROR] Cannot create report file: " << filename << "\n";
        return;
    }

    // ── Step 3: Compute all statistics ─────────────────────
    int    total      = (int)students.size();
    float  sum        = 0.0f;
    float  highest    = students[0].getMarks();
    float  lowest     = students[0].getMarks();
    int    passCount  = 0, failCount = 0;
    string topperName;
    float  topperMarks = -1.0f;
    int    gradeA = 0, gradeB = 0, gradeC = 0, gradeD = 0, gradeF = 0;

    for (const auto& s : students)
    {
        float m = s.getMarks();
        sum += m;
        if (m > highest) highest = m;
        if (m < lowest)  lowest  = m;
        if (m >= 60) ++passCount; else ++failCount;
        if (m > topperMarks) { topperMarks = m; topperName = s.getName(); }

        string g = s.calculateGrade();
        if      (g == "A") ++gradeA;
        else if (g == "B") ++gradeB;
        else if (g == "C") ++gradeC;
        else if (g == "D") ++gradeD;
        else               ++gradeF;
    }
    float avg = sum / total;

    // ── ASCII bar helper (20-char wide) ────────────────────
    // Each '#' represents a proportional share of students
    auto makeBar = [&](int count) -> string
    {
        int filled = (total > 0) ? (count * 20 / total) : 0;
        return string(filled, '#') + string(20 - filled, '.');
    };

    // ── Step 4: Write report header ────────────────────────
    char displayTime[64];
    strftime(displayTime, sizeof(displayTime), "%Y-%m-%d %H:%M:%S", localtime(&now));

    string sep(55, '=');
    string thin(55, '-');

    f << sep << "\n";
    f << "   STUDENT MANAGEMENT SYSTEM  --  Analytics Report\n";
    f << "   Generated  : " << displayTime << "\n";
    f << "   Total Recs : " << total << "\n";
    f << sep << "\n\n";

    // ── Summary ────────────────────────────────────────────
    f << "  SUMMARY\n";
    f << "  " << thin << "\n";
    f << "  Total Students   : " << total << "\n";
    f << "  Average Marks    : " << fixed << setprecision(2) << avg << "\n";
    f << "  Highest Marks    : " << fixed << setprecision(1) << highest << "\n";
    f << "  Lowest Marks     : " << fixed << setprecision(1) << lowest  << "\n";
    f << "  Topper           : " << topperName
      << " (" << fixed << setprecision(1) << topperMarks << ")\n";
    f << "  Pass Count       : " << passCount
      << " (" << fixed << setprecision(1) << (100.0f * passCount / total) << "%)\n";
    f << "  Fail Count       : " << failCount
      << " (" << fixed << setprecision(1) << (100.0f * failCount / total) << "%)\n";
    f << "\n";

    // ── ASCII Grade Distribution Chart ─────────────────────
    f << "  GRADE DISTRIBUTION  [each # = approx 1 student unit]\n";
    f << "  " << thin << "\n";
    f << "  A (90-100) [" << makeBar(gradeA) << "] " << gradeA << " student(s)\n";
    f << "  B (80-89)  [" << makeBar(gradeB) << "] " << gradeB << " student(s)\n";
    f << "  C (70-79)  [" << makeBar(gradeC) << "] " << gradeC << " student(s)\n";
    f << "  D (60-69)  [" << makeBar(gradeD) << "] " << gradeD << " student(s)\n";
    f << "  F (<60)    [" << makeBar(gradeF) << "] " << gradeF << " student(s)\n";
    f << "\n";

    // ── Top 5 Students ─────────────────────────────────────
    vector<Student> sorted = students;
    sort(sorted.begin(), sorted.end(),
         [](const Student& a, const Student& b)
         { return a.getMarks() > b.getMarks(); });

    int topN = min(5, total);
    f << "  TOP " << topN << " STUDENTS  (by Marks)\n";
    f << "  " << thin << "\n";
    f << "  " << left
      << setw(5)  << "Rank"
      << setw(8)  << "ID"
      << setw(22) << "Name"
      << setw(8)  << "Marks"
      << "Grade\n";
    f << "  " << thin << "\n";
    for (int i = 0; i < topN; ++i)
    {
        f << "  " << left
          << setw(5)  << (i + 1)
          << setw(8)  << sorted[i].getId()
          << setw(22) << sorted[i].getName()
          << setw(8)  << fixed << setprecision(1) << sorted[i].getMarks()
          << sorted[i].calculateGrade() << "\n";
    }
    f << "\n";

    // ── Full Student List ───────────────────────────────────
    f << "  ALL STUDENTS\n";
    f << "  " << thin << "\n";
    f << "  " << left
      << setw(6)  << "ID"
      << setw(22) << "Name"
      << setw(5)  << "Age"
      << setw(18) << "Course"
      << setw(8)  << "Marks"
      << setw(6)  << "Grade"
      << "Status\n";
    f << "  " << thin << "\n";
    for (const auto& s : students)
    {
        f << "  " << left
          << setw(6)  << s.getId()
          << setw(22) << s.getName()
          << setw(5)  << s.getAge()
          << setw(18) << s.getCourse()
          << setw(8)  << fixed << setprecision(1) << s.getMarks()
          << setw(6)  << s.calculateGrade()
          << (s.getMarks() >= 60 ? "PASS" : "FAIL") << "\n";
    }
    f << "\n" << sep << "\n";
    f << "  END OF REPORT\n";
    f << sep << "\n";
    f.close();

    // ── Step 5: Confirm to user ─────────────────────────────
    cout << "  [OK] Analytics report generated successfully!\n\n";
    cout << "       File : " << filename << "\n";
    cout << "       Open the file in Notepad or VS Code to view.\n\n";
}
