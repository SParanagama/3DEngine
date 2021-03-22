#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <fstream>

bool ReadFile(const std::string& fileName, std::string& outString)
{
    std::ifstream file(fileName.c_str());

    if(file.is_open()) {
        std::string line;
        while(getline(file, line)) {
            outString.append(line);
            outString.append("\n");    
        }

        file.close();
        return true;
    } else {
        return false;
    }
}

#endif