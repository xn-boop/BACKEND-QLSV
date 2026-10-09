#ifndef STUDENT_H
#define STUDENT_H

#include <string>

struct Student {
    std::string id;
    std::string name;
    int age = 0;
    std::string course;
    std::string status = "active";

    std::string toCsv() const;
    static bool fromCsv(const std::string& line, Student& student);
};

#endif // STUDENT_H
