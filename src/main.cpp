#include "../include/StudentManager.h"

#include <iostream>
#include <limits>

namespace {
int readChoice() {
    int choice = -1;
    std::cout << "Choice: ";
    if (!(std::cin >> choice)) { std::cin.clear(); choice = -1; }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return choice;
}

void pause() { std::cout << "\nPress Enter to continue..."; std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); }

void runAdminMenu(StudentManager& manager) {
    while (true) {
        std::cout << "\n=== ADMIN MENU ===\n1. Add Student\n2. View Active Students\n3. Search Active Student\n"
                  << "4. Deactivate Student\n5. Save All CSV Data\n6. Show Data Summary\n7. Enter/Edit Enrollment Grades\n0. Logout\n";
        switch (readChoice()) {
            case 1: manager.addStudent(); pause(); break;
            case 2: manager.viewStudents(); pause(); break;
            case 3: manager.searchStudent(); pause(); break;
            case 4: manager.deleteStudent(); pause(); break;
            case 5: manager.saveAllData(); pause(); break;
            case 6: manager.showDataSummary(); pause(); break;
            case 7: manager.updateEnrollmentGrades(); pause(); break;
            case 0: return;
            default: std::cout << "Invalid choice.\n";
        }
    }
}

void runStudentMenu(StudentManager& manager, const User& user) {
    while (true) {
        std::cout << "\n=== STUDENT MENU ===\n1. View My Profile\n2. View Course Offerings\n"
                  << "3. Enroll in Course Offering\n4. View My Enrollments\n0. Logout\n";
        switch (readChoice()) {
            case 1: manager.viewStudentProfile(user); pause(); break;
            case 2: manager.viewOpenOfferings(); pause(); break;
            case 3: manager.enrollInOffering(user); pause(); break;
            case 4: manager.viewMyEnrollments(user); pause(); break;
            case 0: return;
            default: std::cout << "Invalid choice.\n";
        }
    }
}
}

int main() {
    StudentManager manager;
    manager.loadAllData();

    while (true) {
        User currentUser;
        if (!manager.loginUser(currentUser)) break;
        if (currentUser.role == "ADMIN") runAdminMenu(manager);
        else if (currentUser.role == "STUDENT") runStudentMenu(manager, currentUser);
        else std::cout << "  [!] Role " << currentUser.role << " has no menu in this phase.\n";
        if (manager.hasUnsavedChanges()) manager.saveAllData();
        if (std::cin.eof()) break;
        std::cout << "\nLogged out.\n";
    }
    std::cout << "Goodbye.\n";
    return 0;
}
