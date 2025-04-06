#include "shader.hpp"

Shader::Shader(const char* vertexPath, const char* geometryPath, const char* fragmentPath) {
    std::string vertexCode, geometryCode, fragmentCode;
    std::ifstream vShaderFile, gShaderFile, fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        if (geometryPath != nullptr) {
            gShaderFile.open(geometryPath);

            std::stringstream gShaderStream;
            
            gShaderStream << gShaderFile.rdbuf();
            
            gShaderFile.close();
            
            geometryCode = gShaderStream.str();
        }
    } catch(const std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ\n" << e.what() << std::endl; 
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    int success;
    char infoLog[INFOLOG_SIZE];

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GL_CHECK(glShaderSource(vertexShader, 1, &vShaderCode, nullptr));
    GL_CHECK(glCompileShader(vertexShader));
    GL_CHECK(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success));

    if (!success) {
        GL_CHECK(glGetShaderInfoLog(vertexShader, INFOLOG_SIZE, nullptr, infoLog));
        std::cerr << "ERROR: Failed to compile vertex shader \"" << vertexPath << "\"\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    GL_CHECK(glShaderSource(fragmentShader, 1, &fShaderCode, nullptr));
    GL_CHECK(glCompileShader(fragmentShader));
    GL_CHECK(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success));

    if (!success) {
        GL_CHECK(glGetShaderInfoLog(fragmentShader, INFOLOG_SIZE, nullptr, infoLog));
        std::cerr << "ERROR: Failed to compile fragment shader \"" << fragmentPath << "\"\n" << infoLog << std::endl;
    }

    GLuint geometryShader;
    if (geometryPath != nullptr) {
        const char * gShaderCode = geometryCode.c_str();

        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        GL_CHECK(glShaderSource(geometryShader, 1, &gShaderCode, nullptr));
        GL_CHECK(glCompileShader(geometryShader));
        GL_CHECK(glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success));

        if (!success) {
            GL_CHECK(glGetShaderInfoLog(geometryShader, INFOLOG_SIZE, nullptr, infoLog));
            std::cerr << "ERROR: Failed to compile geometry shader \"" << geometryPath << "\"\n" << infoLog << std::endl;
        }
    }

    this->m_ID = glCreateProgram();
    GL_CHECK(glAttachShader(this->m_ID, vertexShader));
    GL_CHECK(glAttachShader(this->m_ID, fragmentShader));

    if (geometryPath != nullptr) {
        GL_CHECK(glAttachShader(this->m_ID, geometryShader));
    }

    GL_CHECK(glLinkProgram(this->m_ID));
    GL_CHECK(glGetProgramiv(this->m_ID, GL_LINK_STATUS, &success));

    if (!success) {
        GL_CHECK(glGetProgramInfoLog(this->m_ID, INFOLOG_SIZE, nullptr, infoLog));
        std::cerr << "ERROR: Failed to link shader program for vertex shader (\"" << vertexPath << "\") and fragment shader (\"" << fragmentPath << "\")\n" << infoLog << std::endl;
    }

    GL_CHECK(glDeleteShader(vertexShader));
    GL_CHECK(glDeleteShader(fragmentShader));

    if (geometryPath != nullptr) {
        GL_CHECK(glDeleteShader(geometryShader));
    }
}

Shader::~Shader() {
    GL_CHECK(glDeleteProgram(this->m_ID));

    std::cout << "Shader has been deleted!\n";
}

void Shader::Use() {
    GL_CHECK(glUseProgram(this->m_ID));
}

void Shader::Set(const std::string& name, bool value) const {
    static bool notified = false;

    GLint location = glGetUniformLocation(this->m_ID, name.c_str());
    if (location == -1) {
        if (!notified) {
            std::cerr << "Failed to find \"" << name << "\"" << std::endl;
            notified = true;
        }
    } else {
        GL_CHECK(glUniform1i(location, static_cast<int>(value)));
    }
}
    
void Shader::Set(const std::string& name, int value) const {
    static bool notified = false;

    GLint location = glGetUniformLocation(this->m_ID, name.c_str());
    if (location == -1) {
        if (!notified) {
            std::cerr << "Failed to find \"" << name << "\"" << std::endl;
            notified = true;
        }
    } else {
        GL_CHECK(glUniform1i(location, value));
    }
}

void Shader::Set(const std::string& name, float value) const {
    static bool notified = false;

    GLint location = glGetUniformLocation(this->m_ID, name.c_str());
    if (location == -1) {
        if (!notified) {
            std::cerr << "Failed to find \"" << name << "\"" << std::endl;
            notified = true;
        }
    } else {
        GL_CHECK(glUniform1f(location, value));
    }
}

void Shader::Set(const std::string& name, const glm::vec2& value) const {
    static bool notified = false;

    GLint location = glGetUniformLocation(this->m_ID, name.c_str());
    if (location == -1) {
        if (!notified) {
            std::cerr << "Failed to find \"" << name << "\"" << std::endl;
            notified = true;
        }
    } else {
        GL_CHECK(glUniform2fv(location, 1, &value[0]));
    }
}

void Shader::Set(const std::string& name, float x, float y) const {
    static bool notified = false;

    GLint location = glGetUniformLocation(this->m_ID, name.c_str());
    if (location == -1) {
        if (!notified) {
            std::cerr << "Failed to find \"" << name << "\"" << std::endl;
            notified = true;
        }
    } else {
        GL_CHECK(glUniform2f(location, x, y));
    }
}

void Shader::Set(const std::string& name, const glm::vec3& value) const {
    static bool notified = false;

    GLint location = glGetUniformLocation(this->m_ID, name.c_str());
    if (location == -1) {
        if (!notified) {
            std::cerr << "Failed to find \"" << name << "\"" << std::endl;
            notified = true;
        }
    } else {
        GL_CHECK(glUniform3fv(location, 1, &value[0]));
    }
}

void Shader::Set(const std::string& name, float x, float y, float z) const {
    static bool notified = false;

    GLint location = glGetUniformLocation(this->m_ID, name.c_str());
    if (location == -1) {
        if (!notified) {
            std::cerr << "Failed to find \"" << name << "\"" << std::endl;
            notified = true;
        }
    } else {
        GL_CHECK(glUniform3f(location, x, y, z));
    }
}

void Shader::Set(const std::string& name, const glm::vec4& value) const {
    static bool notified = false;

    GLint location = glGetUniformLocation(this->m_ID, name.c_str());
    if (location == -1) {
        if (!notified) {
            std::cerr << "Failed to find \"" << name << "\"" << std::endl;
            notified = true;
        }
    } else {
        GL_CHECK(glUniform4fv(location, 1, &value[0]));
    }
}

void Shader::Set(const std::string& name, float x, float y, float z, float w) const {
    static bool notified = false;

    GLint location = glGetUniformLocation(this->m_ID, name.c_str());
    if (location == -1) {
        if (!notified) {
            std::cerr << "Failed to find \"" << name << "\"" << std::endl;
            notified = true;
        }
    } else {
        GL_CHECK(glUniform4f(location, x, y, z, w));
    }
}

void Shader::Set(const std::string& name, const glm::mat2& value) const {
    static bool notified = false;

    GLint location = glGetUniformLocation(this->m_ID, name.c_str());
    if (location == -1) {
        if (!notified) {
            std::cerr << "Failed to find \"" << name << "\"" << std::endl;
            notified = true;
        }
    } else {
        GL_CHECK(glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value)));
    }
}

void Shader::Set(const std::string& name, const glm::mat3& value) const {
    static bool notified = false;

    GLint location = glGetUniformLocation(this->m_ID, name.c_str());
    if (location == -1) {
        if (!notified) {
            std::cerr << "Failed to find \"" << name << "\"" << std::endl;
            notified = true;
        }
    } else {
        GL_CHECK(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value)));
    }
}

void Shader::Set(const std::string& name, const glm::mat4& value) const {
    static bool notified = false;

    GLint location = glGetUniformLocation(this->m_ID, name.c_str());
    if (location == -1) {
        if (!notified) {
            std::cerr << "Failed to find \"" << name << "\"" << std::endl;
            notified = true;
        }
    } else {
        GL_CHECK(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value)));
    }
}
