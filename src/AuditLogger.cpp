#include "../include/AuditLogger.h"

#include <ctime>
#include <fstream>
#include <iomanip>

void AuditLogger::logAction(const std::string& username, const std::string& action, const std::string& details) {
    std::ofstream output("data/audit.log", std::ios::app);
    if (!output) return;

    const std::time_t now = std::time(nullptr);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    output << '[' << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] | User: " << username
           << " | Action: " << action << " | Details: " << details << '\n';
}
