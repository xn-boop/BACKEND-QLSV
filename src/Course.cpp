#include "../include/Course.h"
#include "../include/CsvUtils.h"

std::string Course::toCsv() const { return CsvUtils::writeRow({id, name, std::to_string(credits)}); }

bool Course::fromCsv(const std::string& line, Course& course) {
    std::vector<std::string> fields;
    if (!CsvUtils::parseRow(line, fields) || fields.size() != 3 || fields[0].empty()) return false;
    int credits = 0;
    if (!CsvUtils::parseInt(fields[2], credits) || credits <= 0 || fields[1].empty()) return false;
    course.id = fields[0]; course.name = fields[1]; course.credits = credits;
    return true;
}
