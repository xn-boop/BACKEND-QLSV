#include "../include/StudentManager.h"
#include "../include/AuditLogger.h"
#include "../include/CsvUtils.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace {
namespace fs = std::filesystem;
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
    std::ifstream input(path);
    if (!input) return true;

    std::string line;
    std::size_t lineNumber = 1;
    if (!std::getline(input, line)) return true;
    if (line != header) {
        std::cerr << "  [ERROR] Invalid header in " << path << ".\n";
        return false;
    }
    while (std::getline(input, line)) {
        ++lineNumber;
        if (line.empty()) continue;
        T object;
        if (!T::fromCsv(line, object)) {
            std::cerr << "  [ERROR] Invalid record in " << path << " at line " << lineNumber << ".\n";
            return false;
        }
        const std::string& key = csvKey(object);
        if (data.count(key)) {
            std::cerr << "  [ERROR] Duplicate key '" << key << "' in " << path << ".\n";
            return false;
        }
        data.emplace(key, std::move(object));
    }
    return !input.bad();
}

template <typename T>
bool writeCsvTemp(const char* path, const std::string& header, const std::unordered_map<std::string, T>& data) {
    const fs::path temporary = std::string(path) + ".tmp";
    std::ofstream output(temporary, std::ios::trunc);
    if (!output) return false;
    output << header << '\n';
    for (const auto& entry : data) output << entry.second.toCsv() << '\n';
    output.flush();
    return static_cast<bool>(output);
}

struct FileSwap {
    fs::path target;
    fs::path temporary;
    fs::path backup;
    bool backedUp = false;
    bool published = false;
};

void removeIfExists(const fs::path& path) {
    std::error_code error;
    fs::remove(path, error);
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
        std::cout << "  Username: "; if (!std::getline(std::cin, user)) return false;
        std::cout << "  Password: "; if (!std::getline(std::cin, pass)) return false;
        const auto found = users.find(trim(user));
        if (found != users.end() && pass == found->second.password) {
            authenticatedUser = found->second; currentUsername = authenticatedUser.username;
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
    if (loaded && users.empty()) { users.emplace("admin", User{"admin", "admin123", "ADMIN", "none"}); dataModified = true; }
    return loaded;
}

bool StudentManager::saveStudents() const { return writeCsvTemp(STUDENTS_FILE, "id,name,age,course,status", students); }
bool StudentManager::saveCourses() const { return writeCsvTemp(COURSES_FILE, "id,name,credits", courses); }
bool StudentManager::saveOfferings() const { return writeCsvTemp(OFFERINGS_FILE, "id,course_id,semester,capacity", offerings); }
bool StudentManager::saveEnrollments() const { return writeCsvTemp(ENROLLMENTS_FILE, "id,student_id,offering_id,status,grades", enrollments); }
bool StudentManager::saveUsers() const { return writeCsvTemp(USERS_FILE, "username,password,role,ref_id", users); }

bool StudentManager::validateRelationships() const {
    bool valid = true;
    for (const auto& entry : offerings) {
        if (!courses.count(entry.second.course_id)) {
            std::cerr << "  [ERROR] Offering " << entry.second.id << " references missing course " << entry.second.course_id << ".\n";
            valid = false;
        }
    }
    for (const auto& entry : enrollments) {
        const Enrollment& enrollment = entry.second;
        const auto offering = offerings.find(enrollment.offering_id);
        if (!students.count(enrollment.student_id) || offering == offerings.end() || !courses.count(offering->second.course_id)) {
            std::cerr << "  [ERROR] Enrollment " << enrollment.id << " has an invalid reference.\n";
            valid = false;
        }
    }
    for (const auto& entry : users) {
        const User& user = entry.second;
        if (user.role == "STUDENT" && (!students.count(user.ref_id) || user.ref_id.empty())) {
            std::cerr << "  [ERROR] Student user " << user.username << " references a missing student.\n";
            valid = false;
        }
    }
    return valid;
}

void StudentManager::loadAllData() {
    students.clear(); courses.clear(); offerings.clear(); enrollments.clear(); users.clear(); dataModified = false;
    const bool loaded = loadStudents() && loadCourses() && loadOfferings() && loadEnrollments() && loadUsers();
    dataLoadComplete = loaded && validateRelationships();
    std::cout << (dataLoadComplete ? "  [OK]" : "  [!]") << " Loaded data from five CSV files.\n";
    showDataSummary();
}

bool StudentManager::saveAllData() {
    if (!dataLoadComplete) {
        std::cerr << "  [ERROR] Refusing to save because CSV data did not load completely.\n";
        return false;
    }
    if (!saveStudents() || !saveCourses() || !saveOfferings() || !saveEnrollments() || !saveUsers()) {
        std::cerr << "  [ERROR] Could not prepare all CSV files; existing data was not replaced.\n";
        removeIfExists(std::string(STUDENTS_FILE) + ".tmp"); removeIfExists(std::string(COURSES_FILE) + ".tmp");
        removeIfExists(std::string(OFFERINGS_FILE) + ".tmp"); removeIfExists(std::string(ENROLLMENTS_FILE) + ".tmp"); removeIfExists(std::string(USERS_FILE) + ".tmp");
        return false;
    }

    std::vector<FileSwap> files;
    for (const char* path : {STUDENTS_FILE, COURSES_FILE, OFFERINGS_FILE, ENROLLMENTS_FILE, USERS_FILE})
        files.push_back({path, std::string(path) + ".tmp", std::string(path) + ".bak"});

    std::error_code error;
    for (auto& file : files) {
        if (fs::exists(file.backup, error) || error) { std::cerr << "  [ERROR] Stale backup prevents save: " << file.backup << ".\n"; goto rollback; }
        if (fs::exists(file.target, error)) {
            fs::rename(file.target, file.backup, error);
            if (error) { std::cerr << "  [ERROR] Could not back up " << file.target << ".\n"; goto rollback; }
            file.backedUp = true;
        }
    }
    for (auto& file : files) {
        fs::rename(file.temporary, file.target, error);
        if (error) { std::cerr << "  [ERROR] Could not publish " << file.target << ".\n"; goto rollback; }
        file.published = true;
    }
    for (const auto& file : files) removeIfExists(file.backup);
    dataModified = false;
    std::cout << "  [OK] All CSV data saved.\n";
    return true;

rollback:
    bool restored = true;
    for (auto& file : files) if (file.published) { std::error_code removeError; fs::remove(file.target, removeError); if (removeError) restored = false; }
    for (auto& file : files) if (file.backedUp) { std::error_code restoreError; fs::rename(file.backup, file.target, restoreError); if (restoreError) restored = false; }
    for (const auto& file : files) removeIfExists(file.temporary);
    if (restored) std::cerr << "  [ERROR] Save rolled back; existing CSV files were restored.\n";
    else std::cerr << "  [ERROR] Save recovery needs manual attention; backup files were retained.\n";
    return false;
}

void StudentManager::addStudent() {
    Student student; std::string ageText;
    std::cout << "  Student ID: "; std::getline(std::cin, student.id); student.id = trim(student.id);
    if (student.id.empty() || students.count(student.id)) { std::cout << "  [!] Invalid or duplicate ID.\n"; return; }
    std::cout << "  Name: "; std::getline(std::cin, student.name); student.name = trim(student.name);
    std::cout << "  Age: "; std::getline(std::cin, ageText);
    std::cout << "  Course: "; std::getline(std::cin, student.course); student.course = trim(student.course);
    if (!CsvUtils::parseInt(trim(ageText), student.age) || student.name.empty() || student.course.empty() || student.age < 1 || student.age > 120) {
        std::cout << "  [!] Invalid student data.\n"; return;
    }
    students.emplace(student.id, student); dataModified = true;
    std::cout << "  [OK] Student added.\n";
    AuditLogger::logAction(currentUsername, "CREATE_STUDENT", "Student ID=" + student.id);
}

void StudentManager::deleteStudent() {
    std::string id; std::cout << "  Student ID to deactivate: "; std::getline(std::cin, id);
    const auto found = students.find(trim(id));
    if (found == students.end() || found->second.status != "active") { std::cout << "  [!] Active student not found.\n"; return; }
    found->second.status = "inactive"; dataModified = true;
    std::cout << "  [OK] Student deactivated. Historical data was retained.\n";
    AuditLogger::logAction(currentUsername, "DEACTIVATE_STUDENT", "Student ID=" + found->second.id);
}

void StudentManager::viewStudents() const {
    std::cout << "\nID\tName\tAge\tCourse\tStatus\n";
    for (const auto& entry : students) { const Student& student = entry.second; if (student.status == "active") std::cout << student.id << '\t' << student.name << '\t' << student.age << '\t' << student.course << '\t' << student.status << '\n'; }
}

void StudentManager::searchStudent() const {
    std::string id; std::cout << "  Student ID: "; std::getline(std::cin, id);
    const auto found = students.find(trim(id));
    if (found == students.end() || found->second.status != "active") { std::cout << "  [!] Active student not found.\n"; return; }
    const Student& student = found->second; std::cout << "  " << student.id << " | " << student.name << " | " << student.age << " | " << student.course << " | " << student.status << '\n';
}

void StudentManager::showDataSummary() const {
    std::size_t activeStudents = 0; for (const auto& entry : students) if (entry.second.status == "active") ++activeStudents;
    std::cout << "  Active students: " << activeStudents << ", Courses: " << courses.size() << ", Offerings: " << offerings.size() << ", Enrollments: " << enrollments.size() << '\n';
}

void StudentManager::viewStudentProfile(const User& user) const {
    const auto found = students.find(user.ref_id);
    if (found == students.end() || found->second.status != "active") { std::cout << "  [!] No active student profile is linked to this account.\n"; return; }
    const Student& student = found->second; std::cout << "  " << student.id << " | " << student.name << " | " << student.age << " | " << student.course << " | " << student.status << '\n';
}

void StudentManager::viewOpenOfferings() const {
    std::cout << "\nOffering ID\tCourse ID\tSemester\tCapacity\n";
    for (const auto& entry : offerings) { const CourseOffering& offering = entry.second; std::cout << offering.id << '\t' << offering.course_id << '\t' << offering.semester << '\t' << offering.capacity << '\n'; }
}

void StudentManager::enrollInOffering(const User& user) {
    const auto student = students.find(user.ref_id);
    if (user.ref_id.empty() || user.ref_id == "none" || student == students.end() || student->second.status != "active") { std::cout << "  [!] This account is not linked to an active student profile.\n"; return; }
    std::string offeringId; std::cout << "  Offering ID: "; std::getline(std::cin, offeringId); offeringId = trim(offeringId);
    const auto offering = offerings.find(offeringId);
    if (offering == offerings.end() || !courses.count(offering->second.course_id)) { std::cout << "  [!] Offering has an invalid course reference.\n"; return; }
    if (offering->second.capacity <= 0) { std::cout << "  [!] Lớp học phần đã đầy!\n"; return; }

    auto tempEnrollments = enrollments;
    std::size_t activeCount = 0;
    for (const auto& entry : tempEnrollments) {
        const Enrollment& enrollment = entry.second;
        if (enrollment.student_id == user.ref_id && enrollment.offering_id == offeringId && enrollment.status == "active") { std::cout << "  [!] Bạn đã đăng ký lớp học phần này rồi!\n"; return; }
        if (enrollment.offering_id == offeringId && enrollment.status == "active") ++activeCount;
    }
    if (activeCount >= static_cast<std::size_t>(offering->second.capacity)) { std::cout << "  [!] Lớp học phần đã đầy!\n"; return; }

    std::size_t sequence = tempEnrollments.size() + 1; std::string enrollmentId;
    do { enrollmentId = "ENR-" + std::to_string(sequence++); } while (tempEnrollments.count(enrollmentId));
    tempEnrollments.emplace(enrollmentId, Enrollment{enrollmentId, user.ref_id, offeringId, "active", {}});

    const auto originalEnrollments = enrollments;
    const bool originalModified = dataModified;
    enrollments = std::move(tempEnrollments); dataModified = true;
    if (!saveAllData()) {
        enrollments = originalEnrollments; dataModified = originalModified;
        std::cerr << "  [ERROR] Đăng ký không được lưu; giao dịch đã rollback.\n";
        return;
    }
    std::cout << "  [OK] Đăng ký thành công!\n";
    AuditLogger::logAction(currentUsername, "ENROLL_COURSE", "Enrollment ID=" + enrollmentId + ", Offering ID=" + offeringId);
}

void StudentManager::viewMyEnrollments(const User& user) const {
    std::cout << "\nCourse\tMidterm\tFinal\tFinal Grade\n";
    for (const auto& entry : enrollments) {
        const Enrollment& enrollment = entry.second;
        if (enrollment.student_id != user.ref_id || enrollment.status != "active") continue;
        std::string courseName = "Unknown course"; const auto offering = offerings.find(enrollment.offering_id);
        if (offering != offerings.end()) { const auto course = courses.find(offering->second.course_id); if (course != courses.end()) courseName = course->second.name; }
        const auto midterm = enrollment.gradeFor("Midterm"); const auto finalExam = enrollment.gradeFor("Final"); const auto finalGrade = enrollment.calculateFinalGrade();
        std::cout << courseName << '\t' << (midterm ? std::to_string(*midterm) : "Chưa có điểm") << '\t'
                  << (finalExam ? std::to_string(*finalExam) : "Chưa có điểm") << '\t'
                  << (finalGrade ? std::to_string(*finalGrade) : "Chưa có điểm") << '\n';
    }
}

void StudentManager::updateEnrollmentGrades() {
    std::string enrollmentId, midtermText, finalText;
    std::cout << "  Enrollment ID: "; std::getline(std::cin, enrollmentId);
    const auto found = enrollments.find(trim(enrollmentId));
    if (found == enrollments.end() || found->second.status != "active") { std::cout << "  [!] Active enrollment not found.\n"; return; }
    std::cout << "  Midterm (0-10): "; std::getline(std::cin, midtermText);
    std::cout << "  Final (0-10): "; std::getline(std::cin, finalText);
    float midterm = 0.0F, finalExam = 0.0F;
    if (!CsvUtils::parseScore(trim(midtermText), midterm) || !CsvUtils::parseScore(trim(finalText), finalExam)) { std::cout << "  [!] Scores must be finite numbers from 0 to 10.\n"; return; }
    found->second.setGrade("Midterm", midterm); found->second.setGrade("Final", finalExam); dataModified = true;
    const auto finalGrade = found->second.calculateFinalGrade();
    std::cout << "  [OK] Grades updated. Final grade: " << (finalGrade ? std::to_string(*finalGrade) : "pending") << '\n';
    AuditLogger::logAction(currentUsername, "UPDATE_GRADE", "Enrollment ID=" + found->second.id);
}

bool StudentManager::hasUnsavedChanges() const { return dataModified; }
