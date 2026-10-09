/*
 * ============================================================
 *  Student.cpp
 *  Implements the Student class defined in Student.h
 *
 *  Covers: Constructors, Getters, Setters, calculateGrade(),
 *          displayStudent(), displayRow(), serialize(),
 *          deserialize()
 * ============================================================
 */

#include "../include/Student.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

// ─────────────────────────────────────────────
//  Constructors
// ─────────────────────────────────────────────

Student::Student()
    : id(0), name(""), age(0), course(""), marks(0.0f) {}

Student::Student(int id, const string& name, int age,
                 const string& course, float marks)
    : id(id), name(name), age(age), course(course), marks(marks) {}

// ─────────────────────────────────────────────
//  Getters
// ─────────────────────────────────────────────

int    Student::getId()     const { return id;     }
string Student::getName()   const { return name;   }
int    Student::getAge()    const { return age;    }
string Student::getCourse() const { return course; }
float  Student::getMarks()  const { return marks;  }

// ─────────────────────────────────────────────
//  Setters
// ─────────────────────────────────────────────

void Student::setName  (const string& n) { name   = n; }
void Student::setAge   (int a)           { age    = a; }
void Student::setCourse(const string& c) { course = c; }
void Student::setMarks (float m)         { marks  = m; }

// ─────────────────────────────────────────────
//  calculateGrade()
//  Returns a single letter grade based on marks.
//  Grade Scale: A(90-100) B(80-89) C(70-79) D(60-69) F(<60)
// ─────────────────────────────────────────────

string Student::calculateGrade() const
{
    if (marks >= 90) return "A";
    if (marks >= 80) return "B";
    if (marks >= 70) return "C";
    if (marks >= 60) return "D";
    return "F";
}

// ─────────────────────────────────────────────
//  displayStudent()
//  Prints a full detail card for this student.
// ─────────────────────────────────────────────

void Student::displayStudent() const
{
    string line(52, '=');
    cout << "\n" << line << "\n";
    cout << "  Student ID  : " << id                                        << "\n";
    cout << "  Name        : " << name                                      << "\n";
    cout << "  Age         : " << age                                       << "\n";
    cout << "  Course      : " << course                                    << "\n";
    cout << "  Marks       : " << fixed << setprecision(1) << marks         << "\n";
    cout << "  Grade       : " << calculateGrade()                          << "\n";
    cout << "  Status      : " << (marks >= 60 ? "PASS" : "FAIL")          << "\n";
    cout << line << "\n";
}

// ─────────────────────────────────────────────
//  displayRow()
//  Prints one formatted row in the student table.
// ─────────────────────────────────────────────

void Student::displayRow() const
{
    cout << "| "
         << left  << setw(6)  << id
         << "| "  << setw(22) << name
         << "| "  << setw(5)  << age
         << "| "  << setw(14) << course
         << "| "  << right << setw(7) << fixed << setprecision(1) << marks
         << " | " << setw(2)  << calculateGrade()
         << " | " << setw(5)  << (marks >= 60 ? "PASS" : "FAIL")
         << " |\n";
}

// ─────────────────────────────────────────────
//  serialize()
//  Returns "id,name,age,course,marks" for file storage.
// ─────────────────────────────────────────────

string Student::serialize() const
{
    ostringstream oss;
    oss << id      << ","
        << name    << ","
        << age     << ","
        << course  << ","
        << fixed   << setprecision(1) << marks;
    return oss.str();
}

// ─────────────────────────────────────────────
//  deserialize()
//  Parses one CSV line into this Student object.
//  Returns false if the line is malformed.
// ─────────────────────────────────────────────

bool Student::deserialize(const string& line)
{
    if (line.empty()) return false;

    istringstream ss(line);
    string token;

    try
    {
        getline(ss, token, ','); id     = stoi(token);
        getline(ss, token, ','); name   = token;
        getline(ss, token, ','); age    = stoi(token);
        getline(ss, token, ','); course = token;
        getline(ss, token, ','); marks  = stof(token);
    }
    catch (...) { return false; }

    return true;
}
