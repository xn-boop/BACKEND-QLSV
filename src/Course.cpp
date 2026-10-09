#include "../include/Course.h"

#include <sstream>
#include <vector>

std::string Course::toCsv() const { return id + "," + name + "," + std::to_string(credits); }

bool Course::fromCsv(const std::string& line, Course& course) {
    std::istringstream input(line); std::vector<std::string> fields; std::string field;
    while (std::getline(input, field, ',')) fields.push_back(field);
    if (fields.size() != 3 || fields[0].empty()) return false;
    try { course.credits = std::stoi(fields[2]); } catch (...) { return false; }
    course.id = fields[0]; course.name = fields[1]; return true;
}
