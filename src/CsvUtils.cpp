#include "../include/CsvUtils.h"

#include <cmath>
#include <limits>
#include <sstream>

namespace CsvUtils {
bool parseRow(const std::string& row, std::vector<std::string>& fields) {
    fields.clear();
    std::string field;
    bool quoted = false;
    bool fieldStarted = false;

    for (std::size_t index = 0; index < row.size(); ++index) {
        const char character = row[index];
        if (quoted) {
            if (character == '"') {
                if (index + 1 < row.size() && row[index + 1] == '"') { field += '"'; ++index; }
                else quoted = false;
            } else field += character;
        } else if (character == ',') {
            fields.push_back(field); field.clear(); fieldStarted = false;
        } else if (character == '"' && !fieldStarted) {
            quoted = true; fieldStarted = true;
        } else {
            if (character == '"') return false;
            field += character; fieldStarted = true;
        }
    }
    if (quoted) return false;
    fields.push_back(field);
    return true;
}

std::string writeRow(const std::vector<std::string>& fields) {
    std::ostringstream output;
    for (std::size_t index = 0; index < fields.size(); ++index) {
        if (index != 0) output << ',';
        const std::string& field = fields[index];
        const bool quote = field.find_first_of(",\"\r\n") != std::string::npos;
        if (!quote) output << field;
        else {
            output << '"';
            for (const char character : field) output << (character == '"' ? "\"\"" : std::string(1, character));
            output << '"';
        }
    }
    return output.str();
}

bool parseInt(const std::string& text, int& value) {
    try {
        std::size_t parsed = 0;
        const long long result = std::stoll(text, &parsed);
        if (parsed != text.size() || result < std::numeric_limits<int>::min() || result > std::numeric_limits<int>::max()) return false;
        value = static_cast<int>(result);
        return true;
    } catch (...) { return false; }
}

bool parseScore(const std::string& text, float& value) {
    try {
        std::size_t parsed = 0;
        const float result = std::stof(text, &parsed);
        if (parsed != text.size() || !std::isfinite(result) || result < 0.0F || result > 10.0F) return false;
        value = result;
        return true;
    } catch (...) { return false; }
}
}
