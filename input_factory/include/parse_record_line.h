#ifndef WUKONG_PARSE_RECORD_LINE_H
#define WUKONG_PARSE_RECORD_LINE_H

#include <charconv>
#include <string>
#include <system_error>
#include <vector>

struct RecordPoint {
    int xPosi = 0;
    int yPosi = 0;
    int interval = 0;
};

inline bool ParseIntToken(const std::string &s, int &out)
{
    if (s.empty()) {
        return false;
    }
    int value = 0;
    const char *first = s.data();
    const char *last = first + s.size();
    auto result = std::from_chars(first, last, value);
    if (result.ec != std::errc() || result.ptr != last) {
        return false;
    }
    out = value;
    return true;
}

inline bool ParseRecordLine(const std::string &line, RecordPoint &out)
{
    std::vector<std::string> parts;
    size_t start = 0;
    while (true) {
        size_t pos = line.find(',', start);
        if (pos == std::string::npos) {
            parts.push_back(line.substr(start));
            break;
        }
        parts.push_back(line.substr(start, pos - start));
        start = pos + 1;
    }
    if (parts.size() < 3) {
        return false;
    }
    return ParseIntToken(parts[0], out.xPosi) && ParseIntToken(parts[1], out.yPosi) &&
           ParseIntToken(parts[2], out.interval);
}

#endif
