//
// Created by 30367 on 2025/10/31.
//

#ifndef INITIALGL_UNIFORMMAPPER_H
#define INITIALGL_UNIFORMMAPPER_H

// 先包含 GLEW，避免与其它 GL 头冲突
#include <GL/glew.h>

#include "glm.hpp"
#include "gtc/type_ptr.hpp"

#include <string>
#include <unordered_map>
#include <type_traits>

// 统一别名
using UniformLocation = GLint;      // glGetUniformLocation 返回 GLint，需保留 -1
using ProgramID = GLuint;
using UniformMap = std::unordered_map<std::string, UniformLocation>;

class UniformMapper {
private:
    // ProgramID -> (uniformName -> location)
    std::unordered_map<ProgramID, UniformMap> ProgramUniformMap;

    // 获取并缓存 uniform 位置（若不存在则调用 glGetUniformLocation 并缓存）
    UniformLocation resolveAndCacheLocation(ProgramID programID, const std::string& uniformName);

public:
    UniformMapper();
    ~UniformMapper();

    // 手动映射/覆盖某个 uniform 的位置
    void mapUniform(ProgramID programID, const std::string& uniformName, UniformLocation location);

    // 从缓存中获取；若未缓存则尝试查询并缓存
    UniformLocation getUniformLocation(ProgramID programID, const std::string& uniformName);

    // 设置 uniform 值（要求调用方确保 programID 当前已被 glUseProgram 绑定）
    template<class T>
    void setUniform(ProgramID programID, const std::string& uniformName, const T& value) {
        UniformLocation loc = resolveAndCacheLocation(programID, uniformName);
        if (loc < 0)
            return; // 未找到则忽略

        using std::is_same_v;
        // 标量
        if constexpr (is_same_v<T, GLint> || is_same_v<T, int>) {
            glUniform1i(loc, static_cast<GLint>(value));
        } else if constexpr (is_same_v<T, GLuint> || is_same_v<T, unsigned int>) {
            glUniform1ui(loc, static_cast<GLuint>(value));
        } else if constexpr (is_same_v<T, GLfloat> || is_same_v<T, float>) {
            glUniform1f(loc, static_cast<GLfloat>(value));
        } else if constexpr (is_same_v<T, bool>) {
            glUniform1i(loc, value ? 1 : 0);

        // 向量（float）
        } else if constexpr (is_same_v<T, glm::vec2>) {
            glUniform2fv(loc, 1, glm::value_ptr(value));
        } else if constexpr (is_same_v<T, glm::vec3>) {
            glUniform3fv(loc, 1, glm::value_ptr(value));
        } else if constexpr (is_same_v<T, glm::vec4>) {
            glUniform4fv(loc, 1, glm::value_ptr(value));

        // 向量（int）
        } else if constexpr (is_same_v<T, glm::ivec2>) {
            glUniform2iv(loc, 1, glm::value_ptr(value));
        } else if constexpr (is_same_v<T, glm::ivec3>) {
            glUniform3iv(loc, 1, glm::value_ptr(value));
        } else if constexpr (is_same_v<T, glm::ivec4>) {
            glUniform4iv(loc, 1, glm::value_ptr(value));

        // 向量（uint）
        } else if constexpr (is_same_v<T, glm::uvec2>) {
            glUniform2uiv(loc, 1, glm::value_ptr(value));
        } else if constexpr (is_same_v<T, glm::uvec3>) {
            glUniform3uiv(loc, 1, glm::value_ptr(value));
        } else if constexpr (is_same_v<T, glm::uvec4>) {
            glUniform4uiv(loc, 1, glm::value_ptr(value));

        // 矩阵
        } else if constexpr (is_same_v<T, glm::mat3>) {
            glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        } else if constexpr (is_same_v<T, glm::mat4>) {
            glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
        } else {
            // 若需要更多类型，后续可扩展
            static_assert(!sizeof(T), "Unsupported uniform type in uniformMapper::setUniform");
        }
    }

    // 只读访问整个映射
    const std::unordered_map<ProgramID, UniformMap>& getProgramUniformMap() const;
};


#endif //INITIALGL_UNIFORMMAPPER_H