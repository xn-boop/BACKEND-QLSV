/*
 * ============================================================
 *  StudentManager.h
 *  Declares StudentManager — handles all CRUD, file I/O,
 *  sorting, analytics, admin authentication, and report export.
 *
 *  OOP Concepts: Abstraction (hides all business logic),
 *  Encapsulation (private helpers), Separation of Concerns
 * ============================================================
 */

#ifndef STUDENTMANAGER_H
#define STUDENTMANAGER_H

#include "Student.h"

#include <vector>
#include <string>
using namespace std;

// ─────────────────────────────────────────────
//  File path constants
// ─────────────────────────────────────────────
const string STUDENTS_FILE = "data/students.txt";
const string ADMIN_FILE    = "data/admin.txt";
const string REPORTS_DIR   = "reports";
const int    MAX_ATTEMPTS  = 3;

// ─────────────────────────────────────────────
//  StudentManager class
// ─────────────────────────────────────────────

class StudentManager
{
private:
    vector<Student> students;   // in-memory store (STL vector)
    bool dataModified;          // tracks unsaved changes

    // ── Internal helpers ──────────────────────
    bool   idExists(int id)                          const;
    void   printTableHeader()                        const;
    void   printTableFooter()                        const;
    int    findIndexById(int id)                     const;

    // ── Input helpers ─────────────────────────
    int    readInt   (const string& prompt, int  lo, int  hi) const;
    float  readFloat (const string& prompt, float lo, float hi) const;
    string readString(const string& prompt)                    const;
    bool   confirm   (const string& prompt)                    const;

    // ── Admin helpers ─────────────────────────
    void   createDefaultAdminFile()                  const;

public:
    // ── Constructor ───────────────────────────
    StudentManager();

    // ── Authentication ────────────────────────
    bool adminLogin();                      // returns true on success

    // ── File I/O ──────────────────────────────
    void loadFromFile();
    void saveToFile()  const;

    // ── CRUD ──────────────────────────────────
    void addStudent();
    void viewStudents()   const;
    void searchStudent()  const;
    void updateStudent();
    void deleteStudent();

    // ── Sort ──────────────────────────────────
    void sortMenu();
    void sortById();
    void sortByName();
    void sortByMarks();

    // ── Analytics (console) ───────────────────
    void showAnalytics() const;

    // ── Export Analytics Report (pure C++) ────
    void exportReport() const;   // saves formatted .txt report to reports/

    // ── Explicit save (menu option 8) ─────────
    void saveData();

    // ── Unsaved-changes flag ──────────────────
    bool hasUnsavedChanges() const;
};

#endif // STUDENTMANAGER_H
