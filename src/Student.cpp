#include "../include/Student.h"

#include <sstream>
#include <vector>

namespace {
std::vector<std::string> splitCsv(const std::string& line) {
    std::vector<std::string> fields;
    std::istringstream input(line);
    std::string field;
    while (std::getline(input, field, ',')) fields.push_back(field);
    return fields;
}
}

std::string Student::toCsv() const {
    return id + "," + name + "," + std::to_string(age) + "," + course + "," + status;
}

bool Student::fromCsv(const std::string& line, Student& student) {
    const auto fields = splitCsv(line);
    if (fields.size() != 5 || fields[0].empty()) return false;
    try { student.age = std::stoi(fields[2]); }
    catch (...) { return false; }
    student.id = fields[0];
    student.name = fields[1];
    student.course = fields[3];
    student.status = fields[4].empty() ? "active" : fields[4];
    return true;
}
