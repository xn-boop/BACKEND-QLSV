#include "../include/Student.h"
#include "../include/CsvUtils.h"

std::string Student::toCsv() const {
    return CsvUtils::writeRow({id, name, std::to_string(age), course, status});
}

bool Student::fromCsv(const std::string& line, Student& student) {
    std::vector<std::string> fields;
    if (!CsvUtils::parseRow(line, fields) || fields.size() != 5 || fields[0].empty()) return false;
    int age = 0;
    if (!CsvUtils::parseInt(fields[2], age) || age < 1 || age > 120) return false;
    student.id = fields[0]; student.name = fields[1]; student.age = age;
    student.course = fields[3]; student.status = fields[4].empty() ? "active" : fields[4];
    return !student.name.empty() && !student.course.empty();
}
