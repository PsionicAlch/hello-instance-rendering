#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "utility.hpp"

#define INFOLOG_SIZE 1024

class Shader {
public:
    Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath);
    ~Shader();

    void Use();

    void Set(const std::string& name, bool value) const;
    void Set(const std::string& name, int value) const;
    void Set(const std::string& name, float value) const;
    void Set(const std::string& name, const glm::vec2& value) const;
    void Set(const std::string& name, float x, float y) const;
    void Set(const std::string& name, const glm::vec3& value) const;
    void Set(const std::string& name, float x, float y, float z) const;
    void Set(const std::string& name, const glm::vec4& value) const;
    void Set(const std::string& name, float x, float y, float z, float w) const;
    void Set(const std::string& name, const glm::mat2& value) const;
    void Set(const std::string& name, const glm::mat3& value) const;
    void Set(const std::string& name, const glm::mat4& value) const; 
    
private:
    GLuint m_ID = 0;
};