#include "../include/Enrollment.h"

#include <iomanip>
#include <sstream>
#include <vector>

namespace {
std::vector<std::string> split(const std::string& value, char delimiter) {
    std::vector<std::string> parts;
    std::istringstream input(value);
    std::string part;
    while (std::getline(input, part, delimiter)) parts.push_back(part);
    return parts;
}

std::string serializeGrades(const std::vector<std::pair<std::string, float>>& grades) {
    std::ostringstream output;
    for (std::size_t index = 0; index < grades.size(); ++index) {
        if (index != 0) output << '|';
        output << grades[index].first << ':' << std::fixed << std::setprecision(2) << grades[index].second;
    }
    return output.str();
}
}

float Enrollment::gradeFor(const std::string& component) const {
    for (const auto& grade : grades) if (grade.first == component) return grade.second;
    return 0.0F;
}

void Enrollment::setGrade(const std::string& component, float score) {
    for (auto& grade : grades) {
        if (grade.first == component) { grade.second = score; return; }
    }
    grades.emplace_back(component, score);
}

float Enrollment::calculateFinalGrade() const {
    return gradeFor("Midterm") * 0.30F + gradeFor("Final") * 0.70F;
}

std::string Enrollment::toCsv() const {
    return id + "," + student_id + "," + offering_id + "," + status + "," + serializeGrades(grades);
}

bool Enrollment::fromCsv(const std::string& line, Enrollment& enrollment) {
    const auto fields = split(line, ',');
    if ((fields.size() != 4 && fields.size() != 5) || fields[0].empty()) return false;
    enrollment.id = fields[0]; enrollment.student_id = fields[1]; enrollment.offering_id = fields[2];
    enrollment.status = fields[3].empty() ? "active" : fields[3]; enrollment.grades.clear();
    if (fields.size() == 5 && !fields[4].empty()) {
        for (const auto& item : split(fields[4], '|')) {
            const auto separator = item.find(':');
            if (separator == std::string::npos || separator == 0) return false;
            try { enrollment.grades.emplace_back(item.substr(0, separator), std::stof(item.substr(separator + 1))); }
            catch (...) { return false; }
        }
    }
    return true;
}
