#ifndef STUDENT_MANAGER_H
#define STUDENT_MANAGER_H

#include "Course.h"
#include "CourseOffering.h"
#include "Enrollment.h"
#include "Student.h"
#include "User.h"

#include <string>
#include <unordered_map>

class StudentManager {
private:
    std::unordered_map<std::string, Student> students;
    std::unordered_map<std::string, Course> courses;
    std::unordered_map<std::string, CourseOffering> offerings;
    std::unordered_map<std::string, Enrollment> enrollments;
    std::unordered_map<std::string, User> users;
    std::string currentUsername;
    bool dataModified = false;

    bool loadStudents(); bool loadCourses(); bool loadOfferings(); bool loadEnrollments(); bool loadUsers();
    bool saveStudents() const; bool saveCourses() const; bool saveOfferings() const; bool saveEnrollments() const; bool saveUsers() const;
    static std::string trim(const std::string& value);

public:
    bool loginUser(User& authenticatedUser);
    void loadAllData();
    bool saveAllData();
    void addStudent();
    void deleteStudent();
    void viewStudents() const;
    void searchStudent() const;
    void showDataSummary() const;
    void viewStudentProfile(const User& user) const;
    void viewOpenOfferings() const;
    void enrollInOffering(const User& user);
    void viewMyEnrollments(const User& user) const;
    void updateEnrollmentGrades();
    bool hasUnsavedChanges() const;
};

#endif
