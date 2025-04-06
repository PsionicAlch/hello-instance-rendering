#include "utility.hpp"

void checkOpenGLError(const char* function, const char* file, int line) {
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR) {
        std::cerr << "[OpenGL Error] (" << error << "): " << function << " in " << file << " at line " << line << std::endl;
    }
}