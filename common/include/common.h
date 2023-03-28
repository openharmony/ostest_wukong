#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <iostream>
#include <sstream>

namespace Common {
    // cmd calls
    std::string runProcess(const std::string &cmd) {
        if (FILE* fp = popen(cmd.c_str(), "r")) {
            std::ostringstream stm;
            char line[128];
            while (fgets(line, 128, fp)) {
                stm << line;
            }
            pclose(fp);
            return stm.str();
        }
        return "";
    }
}
#endif  // COMMON_H