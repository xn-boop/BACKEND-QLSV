#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include <string>
#include <vector>

namespace CsvUtils {
bool parseRow(const std::string& row, std::vector<std::string>& fields);
std::string writeRow(const std::vector<std::string>& fields);
bool parseInt(const std::string& text, int& value);
bool parseScore(const std::string& text, float& value);
}

#endif
