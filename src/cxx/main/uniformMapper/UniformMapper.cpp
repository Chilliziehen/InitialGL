//
// Created by 30367 on 2025/10/31.
//

#include "UniformMapper.h"

// 简单构造/析构
UniformMapper::UniformMapper() = default;
UniformMapper::~UniformMapper() = default;

// 解析并缓存 uniform 位置
UniformLocation UniformMapper::resolveAndCacheLocation(ProgramID programID, const std::string &uniformName) {
    if (programID == 0 || uniformName.empty()) return static_cast<UniformLocation>(-1);

    auto &uMap = ProgramUniformMap[programID];
    auto it = uMap.find(uniformName);
    if (it != uMap.end()) {
        return it->second;
    }

    UniformLocation loc = glGetUniformLocation(programID, uniformName.c_str());
    uMap[uniformName] = loc; // 缓存 -1 也一并缓存，避免重复查询
    return loc;
}

void UniformMapper::mapUniform(ProgramID programID, const std::string &uniformName, UniformLocation location) {
    if (programID == 0 || uniformName.empty()) return;
    ProgramUniformMap[programID][uniformName] = location;
}

UniformLocation UniformMapper::getUniformLocation(ProgramID programID, const std::string &uniformName) {
    return resolveAndCacheLocation(programID, uniformName);
}

const std::unordered_map<ProgramID, UniformMap> &UniformMapper::getProgramUniformMap() const {
    return ProgramUniformMap;
}
