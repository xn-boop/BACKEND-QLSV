#include "../include/CourseOffering.h"

#include <sstream>
#include <vector>

std::string CourseOffering::toCsv() const { return id + "," + course_id + "," + semester + "," + std::to_string(capacity); }

bool CourseOffering::fromCsv(const std::string& line, CourseOffering& offering) {
    std::istringstream input(line); std::vector<std::string> fields; std::string field;
    while (std::getline(input, field, ',')) fields.push_back(field);
    if (fields.size() != 4 || fields[0].empty()) return false;
    try { offering.capacity = std::stoi(fields[3]); } catch (...) { return false; }
    offering.id = fields[0]; offering.course_id = fields[1]; offering.semester = fields[2]; return true;
}
