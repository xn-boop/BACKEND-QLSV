#ifndef COURSE_H
#define COURSE_H

#include <string>

struct Course {
    std::string id;
    std::string name;
    int credits = 0;

    std::string toCsv() const;
    static bool fromCsv(const std::string& line, Course& course);
};

#endif
