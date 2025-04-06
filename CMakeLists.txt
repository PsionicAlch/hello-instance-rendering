cmake_minimum_required(VERSION 3.30)

project(HelloInstanceRendering LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(DEP_DIR ${CMAKE_SOURCE_DIR}/vendor)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/output)

set(SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/stb_image.cpp
    ${SRC_DIR}/camera.cpp
    ${SRC_DIR}/mesh.cpp
    ${SRC_DIR}/model.cpp
    ${SRC_DIR}/shader.cpp
    ${SRC_DIR}/utility.cpp
)

set(GLAD_SRC ${DEP_DIR}/glad/src/glad.c)
add_library(glad STATIC ${GLAD_SRC})
target_include_directories(glad PUBLIC ${DEP_DIR}/glad/include)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${DEP_DIR}/glfw)

add_subdirectory(${DEP_DIR}/glm)

set(BUILD_SHARED_LIBS OFF)
set(ASSIMP_BUILD_TESTS OFF)
set(ASSIMP_INSTALL OFF)
set(ASSIMP_WARNINGS_AS_ERRORS OFF)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF)

add_subdirectory(${DEP_DIR}/assimp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw glm assimp)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${SRC_DIR}
    ${DEP_DIR}/glad/include
    ${DEP_DIR}/glfw/include
    ${DEP_DIR}/glm
    ${DEP_DIR}/stb_image/include
    ${DEP_DIR}/assimp/include
)

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
    COMMENT "Copying assets directory..."
)

add_dependencies(${PROJECT_NAME} copy_assets)