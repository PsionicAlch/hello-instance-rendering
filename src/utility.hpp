#pragma once

#include <glad/glad.h>

#include <iostream>

#define GL_CHECK(x) do { x; checkOpenGLError(#x, __FILE__, __LINE__); } while (0)

void checkOpenGLError(const char* function, const char* file, int line);