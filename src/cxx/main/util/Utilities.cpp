//
// Created by 30367 on 2025/10/28.
//
#include <fstream>
#include "Utilities.h"
#include <fstream>
#include <stdexcept>
#include <string>

namespace Utilities {
    const char* rdFile(const char* filePath) {
        std::ifstream file(filePath, std::ios::in | std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            throw std::runtime_error("file not found: " + std::string(filePath));
        }

        // 获取文件大小
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        // 分配缓冲区
        char* buf = new char[size + 1];
        if (!file.read(buf, size)) {
            delete[] buf;
            throw std::runtime_error("failed to read file: " + std::string(filePath));
        }

        buf[size] = '\0';
        file.close();
        return buf;
    }
}
