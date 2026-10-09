/*
 * ============================================================
 *  Student.h
 *  Declares the Student class — data model for one student.
 *
 *  OOP Concepts: Encapsulation (private fields), Abstraction
 *  (public interface), Constructors (default + parameterized)
 * ============================================================
 */

#ifndef STUDENT_H
#define STUDENT_H

#include <string>
using namespace std;

// ── Grade scale ──────────────────────────────────────────────
//  90-100 → A   80-89 → B   70-79 → C   60-69 → D   <60 → F
// ─────────────────────────────────────────────────────────────

class Student
{
private:
    int    id;
    string name;
    int    age;
    string course;
    float  marks;

public:
    // ── Constructors ─────────────────────────────────────────
    Student();
    Student(int id, const string& name, int age,
            const string& course, float marks);

    // ── Getters ──────────────────────────────────────────────
    int         getId()     const;
    string      getName()   const;
    int         getAge()    const;
    string      getCourse() const;
    float       getMarks()  const;

    // ── Setters ──────────────────────────────────────────────
    void setName  (const string& n);
    void setAge   (int a);
    void setCourse(const string& c);
    void setMarks (float m);

    // ── Core member functions ─────────────────────────────────
    string calculateGrade() const;   // returns letter grade
    void   displayStudent() const;   // prints detail card
    void   displayRow()     const;   // prints one table row

    // ── Serialisation helpers ─────────────────────────────────
    string serialize()                      const;  // → "id,name,age,course,marks"
    bool   deserialize(const string& line);          // parse CSV line
};

#endif // STUDENT_H
