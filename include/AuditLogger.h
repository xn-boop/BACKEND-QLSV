#ifndef AUDIT_LOGGER_H
#define AUDIT_LOGGER_H

#include <string>

class AuditLogger {
public:
    static void logAction(const std::string& username, const std::string& action, const std::string& details);
};

#endif
