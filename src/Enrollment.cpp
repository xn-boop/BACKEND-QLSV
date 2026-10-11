#include "../include/Enrollment.h"
#include "../include/CsvUtils.h"

#include <iomanip>
#include <sstream>

namespace {
std::vector<std::string> splitGrades(const std::string& value, char delimiter) {
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

std::optional<float> Enrollment::gradeFor(const std::string& component) const {
    for (const auto& grade : grades) if (grade.first == component) return grade.second;
    return std::nullopt;
}

void Enrollment::setGrade(const std::string& component, float score) {
    for (auto& grade : grades) {
        if (grade.first == component) { grade.second = score; return; }
    }
    grades.emplace_back(component, score);
}

std::optional<float> Enrollment::calculateFinalGrade() const {
    const auto midterm = gradeFor("Midterm");
    const auto finalExam = gradeFor("Final");
    if (!midterm || !finalExam) return std::nullopt;
    return *midterm * 0.30F + *finalExam * 0.70F;
}

std::string Enrollment::toCsv() const {
    return CsvUtils::writeRow({id, student_id, offering_id, status, serializeGrades(grades)});
}

bool Enrollment::fromCsv(const std::string& line, Enrollment& enrollment) {
    std::vector<std::string> fields;
    if (!CsvUtils::parseRow(line, fields) || (fields.size() != 4 && fields.size() != 5) || fields[0].empty()) return false;
    enrollment.id = fields[0]; enrollment.student_id = fields[1]; enrollment.offering_id = fields[2];
    enrollment.status = fields[3].empty() ? "active" : fields[3]; enrollment.grades.clear();
    if (fields.size() == 5 && !fields[4].empty()) {
        for (const auto& item : splitGrades(fields[4], '|')) {
            const auto separator = item.find(':');
            float score = 0.0F;
            if (separator == std::string::npos || separator == 0 || !CsvUtils::parseScore(item.substr(separator + 1), score)) return false;
            enrollment.grades.emplace_back(item.substr(0, separator), score);
        }
    }
    return !enrollment.student_id.empty() && !enrollment.offering_id.empty();
}
