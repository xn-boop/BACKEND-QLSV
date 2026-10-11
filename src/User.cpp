#include "../include/User.h"
#include "../include/CsvUtils.h"

std::string User::toCsv() const { return CsvUtils::writeRow({username, password, role, ref_id}); }

bool User::fromCsv(const std::string& line, User& user) {
    std::vector<std::string> fields;
    if (!CsvUtils::parseRow(line, fields) || fields.size() != 4 || fields[0].empty() || fields[1].empty() || fields[2].empty()) return false;
    user.username = fields[0]; user.password = fields[1]; user.role = fields[2]; user.ref_id = fields[3];
    return true;
}
