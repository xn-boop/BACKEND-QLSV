#ifndef COURSE_OFFERING_H
#define COURSE_OFFERING_H

#include <string>

struct CourseOffering {
    std::string id;
    std::string course_id;
    std::string semester;
    int capacity = 0;

    std::string toCsv() const;
    static bool fromCsv(const std::string& line, CourseOffering& offering);
};

#endif
