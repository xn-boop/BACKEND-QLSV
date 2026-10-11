#include "../include/CourseOffering.h"
#include "../include/CsvUtils.h"

std::string CourseOffering::toCsv() const { return CsvUtils::writeRow({id, course_id, semester, std::to_string(capacity)}); }

bool CourseOffering::fromCsv(const std::string& line, CourseOffering& offering) {
    std::vector<std::string> fields;
    if (!CsvUtils::parseRow(line, fields) || fields.size() != 4 || fields[0].empty()) return false;
    int capacity = 0;
    if (!CsvUtils::parseInt(fields[3], capacity) || capacity < 0 || fields[1].empty() || fields[2].empty()) return false;
    offering.id = fields[0]; offering.course_id = fields[1]; offering.semester = fields[2]; offering.capacity = capacity;
    return true;
}
