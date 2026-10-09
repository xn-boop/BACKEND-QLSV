#ifndef ENROLLMENT_H
#define ENROLLMENT_H

#include <string>
#include <vector>

struct Enrollment {
    std::string id;
    std::string student_id;
    std::string offering_id;
    std::string status = "active";
    std::vector<std::pair<std::string, float>> grades;

    float gradeFor(const std::string& component) const;
    void setGrade(const std::string& component, float score);
    float calculateFinalGrade() const;
    std::string toCsv() const;
    static bool fromCsv(const std::string& line, Enrollment& enrollment);
};

#endif
