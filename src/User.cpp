#include "../include/User.h"

#include <sstream>
#include <vector>

std::string User::toCsv() const { return username + "," + password + "," + role + "," + ref_id; }

bool User::fromCsv(const std::string& line, User& user) {
    std::istringstream input(line); std::vector<std::string> fields; std::string field;
    while (std::getline(input, field, ',')) fields.push_back(field);
    if (fields.size() != 4 || fields[0].empty() || fields[2].empty()) return false;
    user.username = fields[0]; user.password = fields[1]; user.role = fields[2]; user.ref_id = fields[3];
    return true;
}
