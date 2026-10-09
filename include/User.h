#ifndef USER_H
#define USER_H

#include <string>

struct User {
    std::string username;
    std::string password;
    std::string role;
    std::string ref_id;

    std::string toCsv() const;
    static bool fromCsv(const std::string& line, User& user);
};

#endif
