#include "../include/StudentManager.h"
#include "../include/AuditLogger.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace {
constexpr const char* STUDENTS_FILE = "data/students.csv";
constexpr const char* COURSES_FILE = "data/courses.csv";
constexpr const char* OFFERINGS_FILE = "data/offerings.csv";
constexpr const char* ENROLLMENTS_FILE = "data/enrollments.csv";
constexpr const char* USERS_FILE = "data/users.csv";

template <typename T>
const std::string& csvKey(const T& object) { return object.id; }

const std::string& csvKey(const User& user) { return user.username; }

template <typename T>
bool loadCsv(const char* path, const std::string& header, std::unordered_map<std::string, T>& data) {
    std::ifstream in(path);
    if (!in) return true;
    std::string line;
    if (!std::getline(in, line)) return true;
    if (line != header) { T object; if (T::fromCsv(line, object)) data[csvKey(object)] = object; }
    while (std::getline(in, line)) { T object; if (T::fromCsv(line, object)) data[csvKey(object)] = object; }
    return !in.bad();
}

template <typename T>
bool saveCsv(const char* path, const std::string& header, const std::unordered_map<std::string, T>& data) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) return false;
    out << header << '\n';
    for (const auto& entry : data) out << entry.second.toCsv() << '\n';
    return static_cast<bool>(out);
}
}

std::string StudentManager::trim(const std::string& value) {
    const auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    return value.substr(first, value.find_last_not_of(" \t\r\n") - first + 1);
}

bool StudentManager::loginUser(User& authenticatedUser) {
    std::string user, pass;
    for (int attempt = 0; attempt < 3; ++attempt) {
        std::cout << "  Username: ";
        if (!std::getline(std::cin, user)) return false;
        std::cout << "  Password: ";
        if (!std::getline(std::cin, pass)) return false;
        const auto found = users.find(trim(user));
        if (found != users.end() && pass == found->second.password) {
            authenticatedUser = found->second;
            currentUsername = authenticatedUser.username;
            std::cout << "  [OK] Login successful as " << authenticatedUser.role << ".\n";
            AuditLogger::logAction(currentUsername, "LOGIN", "Role=" + authenticatedUser.role);
            return true;
        }
        std::cout << "  [!] Invalid credentials.\n";
    }
    return false;
}

bool StudentManager::loadStudents() { return loadCsv(STUDENTS_FILE, "id,name,age,course,status", students); }
bool StudentManager::loadCourses() { return loadCsv(COURSES_FILE, "id,name,credits", courses); }
bool StudentManager::loadOfferings() { return loadCsv(OFFERINGS_FILE, "id,course_id,semester,capacity", offerings); }
bool StudentManager::loadEnrollments() { return loadCsv(ENROLLMENTS_FILE, "id,student_id,offering_id,status,grades", enrollments); }
bool StudentManager::loadUsers() {
    const bool loaded = loadCsv(USERS_FILE, "username,password,role,ref_id", users);
    if (users.empty()) {
        users.emplace("admin", User{"admin", "admin123", "ADMIN", "none"});
        return loaded && saveUsers();
    }
    return loaded;
}
bool StudentManager::saveStudents() const { return saveCsv(STUDENTS_FILE, "id,name,age,course,status", students); }
bool StudentManager::saveCourses() const { return saveCsv(COURSES_FILE, "id,name,credits", courses); }
bool StudentManager::saveOfferings() const { return saveCsv(OFFERINGS_FILE, "id,course_id,semester,capacity", offerings); }
bool StudentManager::saveEnrollments() const { return saveCsv(ENROLLMENTS_FILE, "id,student_id,offering_id,status,grades", enrollments); }
bool StudentManager::saveUsers() const { return saveCsv(USERS_FILE, "username,password,role,ref_id", users); }

void StudentManager::loadAllData() {
    students.clear(); courses.clear(); offerings.clear(); enrollments.clear(); users.clear();
    const bool success = loadStudents() && loadCourses() && loadOfferings() && loadEnrollments() && loadUsers();
    std::cout << (success ? "  [OK]" : "  [!]") << " Loaded data from four CSV files.\n";
    showDataSummary();
}

bool StudentManager::saveAllData() {
    const bool success = saveStudents() && saveCourses() && saveOfferings() && saveEnrollments() && saveUsers();
    if (success) { dataModified = false; std::cout << "  [OK] All CSV data saved.\n"; }
    else std::cerr << "  [ERROR] Could not save all CSV data.\n";
    return success;
}

void StudentManager::addStudent() {
    Student student; std::string age;
    std::cout << "  Student ID: "; std::getline(std::cin, student.id); student.id = trim(student.id);
    if (student.id.empty() || students.count(student.id)) { std::cout << "  [!] Invalid or duplicate ID.\n"; return; }
    std::cout << "  Name: "; std::getline(std::cin, student.name); student.name = trim(student.name);
    std::cout << "  Age: "; std::getline(std::cin, age);
    std::cout << "  Course: "; std::getline(std::cin, student.course); student.course = trim(student.course);
    try { student.age = std::stoi(trim(age)); } catch (...) { std::cout << "  [!] Invalid age.\n"; return; }
    if (student.name.empty() || student.course.empty() || student.age < 1 || student.age > 120) { std::cout << "  [!] Invalid student data.\n"; return; }
    students.emplace(student.id, student); dataModified = true; std::cout << "  [OK] Student added.\n";
    AuditLogger::logAction(currentUsername, "CREATE_STUDENT", "Student ID=" + student.id);
}

void StudentManager::deleteStudent() {
    std::string id;
    std::cout << "  Student ID to deactivate: ";
    std::getline(std::cin, id);

    const auto found = students.find(trim(id));
    if (found == students.end() || found->second.status != "active") {
        std::cout << "  [!] Active student not found.\n";
        return;
    }

    found->second.status = "inactive";
    dataModified = true;
    std::cout << "  [OK] Student deactivated. Historical data was retained.\n";
    AuditLogger::logAction(currentUsername, "DEACTIVATE_STUDENT", "Student ID=" + found->second.id);
}

void StudentManager::viewStudents() const {
    std::cout << "\nID\tName\tAge\tCourse\tStatus\n";
    for (const auto& entry : students) {
        const auto& s = entry.second;
        if (s.status == "active") std::cout << s.id << '\t' << s.name << '\t' << s.age << '\t' << s.course << '\t' << s.status << '\n';
    }
}

void StudentManager::searchStudent() const {
    std::string id; std::cout << "  Student ID: "; std::getline(std::cin, id);
    const auto found = students.find(trim(id));
    if (found == students.end() || found->second.status != "active") { std::cout << "  [!] Active student not found.\n"; return; }
    const auto& s = found->second; std::cout << "  " << s.id << " | " << s.name << " | " << s.age << " | " << s.course << " | " << s.status << '\n';
}

void StudentManager::showDataSummary() const {
    std::size_t activeStudents = 0;
    for (const auto& entry : students) if (entry.second.status == "active") ++activeStudents;
    std::cout << "  Active students: " << activeStudents << ", Courses: " << courses.size()
              << ", Offerings: " << offerings.size() << ", Enrollments: " << enrollments.size() << '\n';
}

void StudentManager::viewStudentProfile(const User& user) const {
    const auto found = students.find(user.ref_id);
    if (found == students.end() || found->second.status != "active") {
        std::cout << "  [!] No active student profile is linked to this account.\n";
        return;
    }
    const Student& student = found->second;
    std::cout << "  " << student.id << " | " << student.name << " | " << student.age
              << " | " << student.course << " | " << student.status << '\n';
}

void StudentManager::viewOpenOfferings() const {
    std::cout << "\nOffering ID\tCourse ID\tSemester\tCapacity\n";
    for (const auto& entry : offerings) {
        const CourseOffering& offering = entry.second;
        std::cout << offering.id << '\t' << offering.course_id << '\t'
                  << offering.semester << '\t' << offering.capacity << '\n';
    }
}

void StudentManager::enrollInOffering(const User& user) {
    const auto student = students.find(user.ref_id);
    if (user.ref_id.empty() || user.ref_id == "none" || student == students.end() || student->second.status != "active") {
        std::cout << "  [!] This account is not linked to a student profile.\n";
        return;
    }
    std::string offeringId;
    std::cout << "  Offering ID: "; std::getline(std::cin, offeringId);
    offeringId = trim(offeringId);
    const auto offering = offerings.find(offeringId);
    if (offering == offerings.end()) { std::cout << "  [!] Offering not found.\n"; return; }

    auto temp_enrollments = enrollments;
    std::size_t activeCount = 0;
    for (const auto& entry : temp_enrollments) {
        const Enrollment& enrollment = entry.second;
        if (enrollment.student_id == user.ref_id && enrollment.offering_id == offeringId && enrollment.status == "active") {
            std::cout << "  [!] Bạn đã đăng ký lớp học phần này rồi!\n";
            return;
        }
        if (enrollment.offering_id == offeringId && enrollment.status == "active") ++activeCount;
    }

    if (activeCount >= static_cast<std::size_t>(offering->second.capacity)) {
        std::cout << "  [!] Lớp học phần đã đầy!\n";
        return;
    }

    std::size_t sequence = temp_enrollments.size() + 1;
    std::string enrollmentId;
    do { enrollmentId = "ENR-" + std::to_string(sequence++); }
    while (temp_enrollments.find(enrollmentId) != temp_enrollments.end());
    temp_enrollments.emplace(enrollmentId, Enrollment{enrollmentId, user.ref_id, offeringId, "active", {}});

    enrollments = temp_enrollments;
    dataModified = true;
    if (saveAllData()) {
        std::cout << "  [OK] Đăng ký thành công!\n";
        AuditLogger::logAction(currentUsername, "ENROLL_COURSE", "Enrollment ID=" + enrollmentId + ", Offering ID=" + offeringId);
    }
}

void StudentManager::viewMyEnrollments(const User& user) const {
    std::cout << "\nCourse\tMidterm\tFinal\tFinal Grade\n";
    for (const auto& entry : enrollments) {
        const Enrollment& enrollment = entry.second;
        if (enrollment.student_id != user.ref_id || enrollment.status != "active") continue;
        std::string courseName = "Unknown course";
        const auto offering = offerings.find(enrollment.offering_id);
        if (offering != offerings.end()) {
            const auto course = courses.find(offering->second.course_id);
            if (course != courses.end()) courseName = course->second.name;
        }
        std::cout << courseName << '\t' << enrollment.gradeFor("Midterm") << '\t'
                  << enrollment.gradeFor("Final") << '\t' << enrollment.calculateFinalGrade() << '\n';
    }
}

void StudentManager::updateEnrollmentGrades() {
    std::string enrollmentId, midtermText, finalText;
    std::cout << "  Enrollment ID: "; std::getline(std::cin, enrollmentId);
    const auto found = enrollments.find(trim(enrollmentId));
    if (found == enrollments.end() || found->second.status != "active") { std::cout << "  [!] Active enrollment not found.\n"; return; }
    std::cout << "  Midterm (0-10): "; std::getline(std::cin, midtermText);
    std::cout << "  Final (0-10): "; std::getline(std::cin, finalText);
    try {
        const float midterm = std::stof(trim(midtermText));
        const float finalExam = std::stof(trim(finalText));
        if (midterm < 0.0F || midterm > 10.0F || finalExam < 0.0F || finalExam > 10.0F) throw std::out_of_range("score");
        found->second.setGrade("Midterm", midterm);
        found->second.setGrade("Final", finalExam);
        dataModified = true;
        std::cout << "  [OK] Grades updated. Final grade: " << found->second.calculateFinalGrade() << '\n';
        AuditLogger::logAction(currentUsername, "UPDATE_GRADE", "Enrollment ID=" + found->second.id);
    } catch (...) { std::cout << "  [!] Scores must be numbers from 0 to 10.\n"; }
}

bool StudentManager::hasUnsavedChanges() const { return dataModified; }
